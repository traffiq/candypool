
#include "simpleList.h"
#include "unistd.h"
#include <string>
#include <iostream>
#include <mysql++.h>
#include <memory>
#include <iostream>
#include "wallet.h"
#include "bitcoinrpc.h"
#include <map>
#include <string>


// client states
#define XPT_CLIENT_STATE_NEW		(0)
#define XPT_CLIENT_STATE_LOGGED_IN	(1)

typedef struct _xptServer_t xptServer_t;

typedef struct xptPacketbuffer_t {
	uint8_t* buffer;
	uint32_t parserIndex;
	uint32_t bufferLimit; // current maximal size of buffer
	uint32_t bufferSize; // current effective size of buffer

	xptPacketbuffer_t( uint32_t initialSize )
	{
		// setup buffer object
		bufferLimit = 4096;
		// alloc buffer space
		buffer = (uint8_t*)malloc(bufferLimit);
		memset(buffer, 0x00, bufferLimit);

		parserIndex = 0;
		bufferSize 	= 0;
	}

	~xptPacketbuffer_t()
	{
		free(buffer);
	}

} xptPacketbuffer_t;

typedef struct {
	uint8_t merkleRoot[32];
	uint32_t seed;
} xptWorkData_t;

typedef struct {
	uint32_t height;
	uint32_t version;
	uint32_t nTime;
	uint32_t nBits;
	uint32_t nBitsShare;
	uint8_t prevBlock[32];
} xptBlockWorkInfo_t;


typedef struct xptServerClient_t{
	SOCKET clientSocket;
	bool disconnected;
	// recv buffer
	xptPacketbuffer_t* packetbuffer;
	uint32_t recvIndex;
	uint32_t recvSize;
	// recv header info
	uint32_t opcode;
	// authentication info
	uint8_t clientState;
	uint32_t workerId;
	char workerName[128];
	char workerPass[128];
	uint32_t userId;
	uint32_t coinTypeIndex;
	uint32_t payloadNum;

	boost::mutex mutex;

	// is the client already monitored by a server instance ?
	bool bMonitored;

	// current block of the worker
	std::vector<CBlock> arrCurrentBlocks;
	CReserveKey *currentKey;

	xptServerClient_t()
	{
		userId = 0;
		workerId = 0;
		clientSocket = 0;
		payloadNum = 0;
		coinTypeIndex = 0;
		packetbuffer = new xptPacketbuffer_t(4 * 1024);
		disconnected = false;
		recvIndex = 0;
		recvSize = 0;
		opcode = -1;
		clientState = XPT_CLIENT_STATE_NEW;
		arrCurrentBlocks.reserve(4);
		currentKey = NULL;
		bMonitored = false;
	}

	~xptServerClient_t()
	{
		arrCurrentBlocks.clear();
		disconnected = true;
		bMonitored = false;
		currentKey = NULL;
		delete packetbuffer;
		close(clientSocket);
	}

} xptServerClient_t;

typedef struct
{
	uint32_t version;
	uint8_t prevBlockHash[32];
	uint8_t merkleRoot[32];
	uint32_t nTime;
	uint32_t nBits;
	uint32_t nonce;
	// primecoin specific
	uint32_t sieveSize;
	uint64 sieveCandidate; // index of sieveCandidate for this share
	uint8_t fixedMultiplierSize;
	uint8_t fixedMultiplier[201];
	uint8_t chainMultiplierSize;
	uint8_t chainMultiplier[201];
	uint32_t share_id; // has no use atm
}xptSubmittedShare_t;

typedef struct _xptServer_t {

	std::vector<xptServerClient_t*> list_connections;
	xptPacketbuffer_t* sendBuffer; // shared buffer for sending data
	// last known block height (for new block detection)
	uint32_t coinTypeBlockHeight[32];
	CWallet* pWallet;
	CBlock *currentBlock;

	// constructor
	_xptServer_t()
	{
		sendBuffer = new xptPacketbuffer_t(4 * 1024);
		list_connections.reserve(20000);
		pWallet = NULL;
		currentBlock = NULL;
	}

	~_xptServer_t()
	{
		list_connections.clear();
		pWallet = NULL;
		currentBlock = NULL;
		if(con.connect())
		{
			con.disconnect();
		}
	}


	// callbacks
	bool xptCallback_generateWork(xptServer_t* xptServer,
									xptServerClient_t* xptServerClient,
									uint32_t numOfWorkEntries,
									uint32_t coinTypeIndex,
									xptBlockWorkInfo_t* xptBlockWorkInfo,
									xptWorkData_t* xptWorkData);

	void xptCallback_getBlockHeight(xptServer_t* xptServer,
									uint32_t* coinTypeNum, uint32_t* blockHeightPerCoinType);

	bool xptCallback_checkWork(	xptServer_t* xptServer,
								xptServerClient_t* xptServerClient, std::auto_ptr<xptSubmittedShare_t> submittedShare,
								double *shareValue, std::string &errorText );

	//mysqlpp::Connection *connection;
	mysqlpp::Connection con;
	mysqlpp::Connection *conn()
	{
		if( con.connected() == false )
		{
			std::string strUser = mapArgs["-sqluser"];
			std::string strPw = mapArgs["-sqlpassword"];
			std::string strDb = mapArgs["-sqldatabase"];
			std::string strHost = mapArgs["-sqlhost"];
			con = mysqlpp::Connection( strDb.c_str(), strHost.c_str(), strUser.c_str(), strPw.c_str() );
			if( con.connected() == false)
			{
				fprintf(stderr, "DB connection failed for miner thread ! \n");
				abort();
			}
		}
		return &con;
	};
} xptServer_t;

#define POOL_DIFFICULT	(123246208) // minimal difficult 113246208 -> 6,75


// list of known opcodes

#define XPT_OPC_C_AUTH_REQ		1
#define XPT_OPC_S_AUTH_ACK		2
#define XPT_OPC_S_WORKDATA1		3
#define XPT_OPC_C_SUBMIT_SHARE	4
#define XPT_OPC_S_SHARE_ACK		5

// ping package
#define XPT_OPC_S_MESSAGE       7
#define XPT_OPC_C_PING			8
#define XPT_OPC_S_PING			8

// list of error codes

#define XPT_ERROR_NONE				(0)
#define XPT_ERROR_INVALID_LOGIN		(1)
#define XPT_ERROR_INVALID_WORKLOAD	(2)
#define XPT_ERROR_INVALID_COINTYPE	(3)

// epoll socket methods
void xptServer_startProcessingEPoll(CWallet* pwallet);
void StartListeningThread( CWallet* pwallet, int efd);

// private packet handlers
bool xptServer_processPacket_authRequest(xptServer_t* xptServer, xptServerClient_t* xptServerClient);

bool xptServer_processPacket_submitShareRequest(xptServer_t* xptServer, xptServerClient_t* xptServerClient);

bool xptServer_processPacket_pingRequest(xptServer_t* xptServer, xptServerClient_t* xptServerClient);

// public packet methods
bool xptServer_sendBlockData(xptServer_t* xptServer,
		xptServerClient_t* xptServerClient);

void xptPacketbuffer_changeSizeLimit(xptPacketbuffer_t* pb, uint32_t sizeLimit);

void xptPacketbuffer_beginReadPacket(xptPacketbuffer_t* pb);
uint32_t xptPacketbuffer_getReadSize(xptPacketbuffer_t* pb);
float xptPacketbuffer_readFloat(xptPacketbuffer_t* pb, bool* error);
uint64 xptPacketbuffer_readU64(xptPacketbuffer_t* pb, bool* error);
uint32_t xptPacketbuffer_readU32(xptPacketbuffer_t* pb, bool* error);
uint16_t xptPacketbuffer_readU16(xptPacketbuffer_t* pb, bool* error);
uint8_t xptPacketbuffer_readU8(xptPacketbuffer_t* pb, bool* error);
void xptPacketbuffer_readData(xptPacketbuffer_t* pb, uint8_t* data,
		uint32_t length, bool* error);

void xptPacketbuffer_beginWritePacket(xptPacketbuffer_t* pb, uint8_t opcode);
void xptPacketbuffer_writeU64(xptPacketbuffer_t* pb, bool* error, uint64 v);
void xptPacketbuffer_writeU32(xptPacketbuffer_t* pb, bool* error, uint32_t v);
void xptPacketbuffer_writeU16(xptPacketbuffer_t* pb, bool* error, uint16_t v);
void xptPacketbuffer_writeU8(xptPacketbuffer_t* pb, bool* error, uint8_t v);
void xptPacketbuffer_writeData(xptPacketbuffer_t* pb, uint8_t* data,
		uint32_t length, bool* error);
void xptPacketbuffer_writeFloat(xptPacketbuffer_t* pb, bool* error, float v);
void xptPacketbuffer_finalizeWritePacket(xptPacketbuffer_t* pb);

void xptPacketbuffer_writeString(xptPacketbuffer_t* pb, const char* stringData,
		uint32_t maxStringLength, bool* error);
void xptPacketbuffer_readString(xptPacketbuffer_t* pb, char* stringData,
		uint32_t maxStringLength, bool* error);

// checks blocks in the db for confirmation
void xptServer_checkBlockConfirmations(mysqlpp::Connection *conn, CWallet* pwallet);

// checks for confirmed blocks and pays them
void xptServer_payConfirmedBlocks(mysqlpp::Connection *conn, CWallet* pwallet);

// give the money to the people
void xptServer_payWallet(mysqlpp::Connection *conn, CWallet* pwallet);

// pay xpm
bool xptServer_payOut(mysqlpp::Connection *conn, const uint32_t &user_id, CWallet* pwallet );

// generate a hash
std::string sha256(const std::string str);

// generate share value
void getShareValue( sint32 iChainLength, double *shareValue );

// check for double submissions to pool
bool CheckDoubleSubmit(	xptServer_t* xptServer,
						const uint32_t &blockHeight,
						const std::string &strPrimeChain,
						const uint32_t &workerId );

// save share to db
bool SaveShare( xptServer_t* xptServer,
				const std::string &strPrimeChain,
				const uint32_t &chainLength,
				const uint32_t &blockHeight,
				const double &shareValue,
				const uint32_t &workerId,
				const uint32_t &userId );

// save block
bool SaveBlock( xptServer_t* xptServer, CBlock *pBlock );

// get a query object
mysqlpp::Query* GetQuery( xptServer_t* xptServer );


// proxy stuff
typedef struct
{
	uint8 merkleRoot[32];
	uint8 prevBlockHash[32];
	uint32_t version;
	uint32_t nonce;
	uint32_t nTime;
	uint32_t nBits;
	// primecoin specific
	uint32_t sieveSize;
	uint64 sieveCandidate; // index of sieveCandidate for this share
	uint8 fixedMultiplierSize;
	uint8 fixedMultiplier[201];
	uint8 chainMultiplierSize;
	uint8 chainMultiplier[201];
}xptShareToSubmit_t;


extern char* minerVersionString;

