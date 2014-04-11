#include"global.h"
#include "xptServer.h"
#include <bits/stl_vector.h>

/*
 * Sends the response for an auth packet
 */
bool xptServer_sendAuthResponse(xptServer_t* xptServer, xptServerClient_t* xptServerClient, uint32_t authErrorCode, std::string &rejectReason)
{
	bool sendError = false;
	xptPacketbuffer_beginWritePacket(xptServer->sendBuffer, XPT_OPC_S_AUTH_ACK);
	xptPacketbuffer_writeU32(xptServer->sendBuffer, &sendError, authErrorCode);
	if(sendError)
	{
		fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
		return false;
	}

	// write reject reason string (or motd in case of no error)
	sint32 rejectReasonLength = rejectReason.length();
	xptPacketbuffer_writeU16(xptServer->sendBuffer, &sendError, (uint16_t)rejectReasonLength);
	if(sendError)
	{
		fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
		return false;
	}
	xptPacketbuffer_writeData(xptServer->sendBuffer, (uint8_t*)rejectReason.c_str(), (uint32_t)rejectReasonLength, &sendError);
	if(sendError)
	{
		fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
		return false;
	}
	// finalize
	xptPacketbuffer_finalizeWritePacket(xptServer->sendBuffer);
	// send to client
	send(xptServerClient->clientSocket, (const char*)(xptServer->sendBuffer->buffer), xptServer->sendBuffer->parserIndex, MSG_NOSIGNAL );
	return true;
}

/*
 * Sends the response for an auth packet
 */
bool xptServer_sendShareResponse(xptServer_t* xptServer, xptServerClient_t* xptServerClient, uint32_t shareErrorCode, std::string &rejectReason, float shareValue)
{
	bool sendError = false;
	xptPacketbuffer_beginWritePacket(xptServer->sendBuffer, XPT_OPC_S_SHARE_ACK);
	xptPacketbuffer_writeU32(xptServer->sendBuffer, &sendError, shareErrorCode); // 0 == okay; 0 != not okay
	if(sendError){
		fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
		return false;
	}

	xptPacketbuffer_writeString(xptServer->sendBuffer, rejectReason.c_str(), 512, &sendError);	// username
		if(sendError){
		fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
		return false;
	}
	xptPacketbuffer_writeFloat(xptServer->sendBuffer, &sendError, shareValue);
		if(sendError){
		fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
		return false;
	}

	// finalize
	xptPacketbuffer_finalizeWritePacket(xptServer->sendBuffer);

	// send to client
	send(xptServerClient->clientSocket, (const char*)(xptServer->sendBuffer->buffer), xptServer->sendBuffer->parserIndex, MSG_NOSIGNAL);
	return true;
}

/*
 * Generates the block data and sends it to the client
 */
bool xptServer_sendBlockData(xptServer_t* xptServer, xptServerClient_t* xptServerClient)
{
	if(xptServerClient == NULL || xptServer == NULL )
	{
		fprintf(stderr, "Send Blockdata, Nullpointer.");
		return false;
	}

	// we need several callbacks to the main work manager:
	if( xptServerClient->payloadNum < 1 || xptServerClient->payloadNum > 128 )
	{
		fprintf(stderr, "xptServer_sendBlockData(): payloadNum out of range for worker %s\n", xptServerClient->workerName);
		return false;
	}
	// generate work
	xptBlockWorkInfo_t blockWorkInfo;
	xptWorkData_t workData[128];

	{
	    if( xptServer->xptCallback_generateWork(xptServer, xptServerClient, xptServerClient->payloadNum, xptServerClient->coinTypeIndex, &blockWorkInfo, workData) == false )
		{
			fprintf(stderr, "xptServer_sendBlockData(): Unable to generate work data for worker %s\n", xptServerClient->workerName);
			return false;
		}

	} // mutex automatically freed because lock object destroyed

	// build the packet
	bool sendError = false;
	xptPacketbuffer_beginWritePacket(xptServer->sendBuffer, XPT_OPC_S_WORKDATA1);
	// add general block info
	xptPacketbuffer_writeU32(xptServer->sendBuffer, &sendError, blockWorkInfo.version);				// block version
	if(sendError){
		fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
		return false;
	}

	xptPacketbuffer_writeU32(xptServer->sendBuffer, &sendError, blockWorkInfo.height);				// block height
	if(sendError){
		fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
		return false;
	}

	xptPacketbuffer_writeU32(xptServer->sendBuffer, &sendError, blockWorkInfo.nBits);				// nBits
	if(sendError){
		fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
		return false;
	}

	xptPacketbuffer_writeU32(xptServer->sendBuffer, &sendError, blockWorkInfo.nBitsShare);			// nBitsRecommended / nBitsShare
	if(sendError){
		fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
		return false;
	}

	xptPacketbuffer_writeU32(xptServer->sendBuffer, &sendError, blockWorkInfo.nTime);				// nTimestamp
	if(sendError){
		fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
		return false;
	}

	xptPacketbuffer_writeData(xptServer->sendBuffer, blockWorkInfo.prevBlock, 32, &sendError);		// prevBlockHash
	if(sendError){
		fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
		return false;
	}

	xptPacketbuffer_writeU32(xptServer->sendBuffer, &sendError, xptServerClient->payloadNum);		// payload num
	if(sendError){
		fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
		return false;
	}

	for(uint32_t i=0; i<xptServerClient->payloadNum; i++)
	{
		// add merkle root for each work data entry
		xptPacketbuffer_writeData(xptServer->sendBuffer, workData[i].merkleRoot, 32, &sendError);
		if(sendError){
			fprintf(stderr, "Send failed: %d in file %s\n", __LINE__, __FILE__);
			return false;
		}

	}
	// finalize
	xptPacketbuffer_finalizeWritePacket(xptServer->sendBuffer);
	// send to client
	send(xptServerClient->clientSocket, (const char*)(xptServer->sendBuffer->buffer), xptServer->sendBuffer->parserIndex, MSG_NOSIGNAL);
	return true;
}

/*
 * Called when an authentication request packet is received
 * This packet must arrive before any other packet
 */
bool xptServer_processPacket_authRequest(xptServer_t* xptServer, xptServerClient_t* xptServerClient)
{

	if( xptServerClient->clientState != XPT_CLIENT_STATE_NEW )
		return false; // client already logged in or has other invalid state
	xptPacketbuffer_t* cpb = xptServerClient->packetbuffer;
	// read data from the packet
	xptPacketbuffer_beginReadPacket(cpb);

	//uint32_t 	version;				//version of the x.pushthrough protocol used
	uint32_t	usernameLength;			// range 1-128
	char	username[128+4];		// workername
	uint32_t	passwordLength;			// range 1-128
	char	password[128+4];		// workername
	uint32_t	payloadNum;				// number of different merkleRoots the server will generate for each block data request. Valid range: 1-128
	// start parsing
	bool readError = false;
	// read version field
	xptPacketbuffer_readU32(cpb, &readError); // we dont need it
	if( readError )
		return false;
	// read username length field
	usernameLength = xptPacketbuffer_readU8(cpb, &readError);
	if( readError )
		return false;
	if( usernameLength < 1 || usernameLength > 128 )
		return false;
	// read username
	memset(username, 0x00, sizeof(username));
	xptPacketbuffer_readData(cpb, (uint8*)username, usernameLength, &readError);
	username[128] = '\0';
	if( readError )
		return false;
	// read password length field
	passwordLength = xptPacketbuffer_readU8(cpb, &readError);
	if( readError )
		return false;
	if( passwordLength < 1 || passwordLength > 128 )
		return false;
	// read password
	memset(password, 0x00, sizeof(password));
	xptPacketbuffer_readData(cpb, (uint8*)password, passwordLength, &readError);
	password[128] = '\0';
	if( readError )
		return false;
	// read workloadPayRequest
	payloadNum = xptPacketbuffer_readU32(cpb, &readError);
	if( readError )
		return false;
	// prepare reject stuff
	uint8 rejectReasonCode = XPT_ERROR_NONE; // 0 -> no error

	std::string rejectReasonText = "";
	// worker login found - validate remaining data
	if( payloadNum < 1 || payloadNum > 128 )
	{
		rejectReasonCode = XPT_ERROR_INVALID_WORKLOAD;
		rejectReasonText = "Workload is out of range. 1-128 allowed.";
		// send rejection response
		xptServer_sendAuthResponse(xptServer, xptServerClient, rejectReasonCode, rejectReasonText);
		// leave and disconnect client
		return false;
	}

	// check pass and workername
	try
	{
		mysqlpp::Connection *conn = xptServer->conn();
		mysqlpp::Query query = conn->query();

		if( query == NULL )
		{
			rejectReasonCode = XPT_ERROR_INVALID_LOGIN;
			rejectReasonText = "Database is offline, sorry !";
			xptServer_sendAuthResponse(xptServer, xptServerClient, rejectReasonCode, rejectReasonText);
			return false;
		}
		query << "select * from worker where `name` = "<< mysqlpp::quote << username;

		mysqlpp::StoreQueryResult res = query.store();
		if( res.num_rows() <= 0 || res.num_rows() > 1 )
		{
			// insert a worker ( private pool so no signup required )
			// new worker get added to user with id 1
			query << "INSERT INTO "<< mapArgs["-sqldatabase"] << ".`worker` (`id`, `user_id`, `name`, `cointype`, `value_per_hour`, `pass`, `online`) "
					"VALUES (NULL, '1', "
					""<< mysqlpp::quote << username << ", '1', NULL, '', '0');";
			query.execute();

			query << "select * from worker where `name` = "<< mysqlpp::quote << username;
			mysqlpp::StoreQueryResult res = query.store();
			if( res.num_rows() <= 0 || res.num_rows() > 1 )
			{
				rejectReasonCode = XPT_ERROR_INVALID_LOGIN;
				rejectReasonText = "SQL Error, Worker invalid.";
				xptServer_sendAuthResponse(xptServer, xptServerClient, rejectReasonCode, rejectReasonText);
				return false;
			}
			else
			{
				xptServerClient->workerId = res[0]["id"];
				xptServerClient->userId = res[0]["user_id"];
			}
		}
		else
		{
			xptServerClient->workerId = res[0]["id"];
			xptServerClient->userId = res[0]["user_id"];
		}

		// set worker to online state
		query << "UPDATE "<< mapArgs["-sqldatabase"] << ".`worker` SET `online` = "<< mysqlpp::quote << payloadNum << " WHERE `id` =" << mysqlpp::quote << xptServerClient->workerId;
		query.execute();
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		fprintf(stderr, "%s", er.what());
		return false;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		fprintf(stderr, "%s", er.what());
		return false;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		fprintf(stderr, "%s", er.what());
		return false;
	}

	// everything good so far, mark client as logged in and proceed
	strncpy(xptServerClient->workerName, username, usernameLength);
//	xptServerClient->workerName[128] = '\0';
	xptServerClient->coinTypeIndex = 0;
	xptServerClient->clientState = XPT_CLIENT_STATE_LOGGED_IN;
	xptServerClient->payloadNum = payloadNum;

	// send success response
	std::string motd = "Welcome and happy XPM mining!";
	if( !xptServer_sendAuthResponse(xptServer, xptServerClient, XPT_ERROR_NONE, motd) )
		return false;
	// immediately send first block of data
	if( !xptServer_sendBlockData(xptServer, xptServerClient) )
		return false;

	return true;
}

/*
 * Called when an a share is recieved
 */
bool xptServer_processPacket_submitShareRequest(xptServer_t* xptServer, xptServerClient_t* xptServerClient)
{
	// yeah a share
	// we need to check it and send
	// XPT_OPC_S_SHARE_ACK
	if( xptServerClient == NULL || xptServerClient->disconnected == true || xptServerClient->clientState != XPT_CLIENT_STATE_LOGGED_IN )
			return false; // client not logged in

	// get packet buffer
	xptPacketbuffer_t* cpb = xptServerClient->packetbuffer;

	std::auto_ptr<xptSubmittedShare_t> submittedShare( new xptSubmittedShare_t() );

	// start reading
	bool readError = false;
	xptPacketbuffer_beginReadPacket(cpb);

	xptPacketbuffer_readData(cpb, (uint8*)submittedShare->merkleRoot, 32, &readError);		// merkleRoot
	if( readError )
		return false;

	xptPacketbuffer_readData(cpb, (uint8*)submittedShare->prevBlockHash, 32, &readError);	// prevBlock
	if( readError )
		return false;

	submittedShare->version = xptPacketbuffer_readU32(cpb, &readError);				// version
	if( readError )
		return false;

	submittedShare->nTime = xptPacketbuffer_readU32(cpb, &readError);				// nTime
	if( readError )
			return false;

	submittedShare->nonce = xptPacketbuffer_readU32(cpb, &readError);				// nNonce
	if( readError )
			return false;

	submittedShare->nBits = xptPacketbuffer_readU32(cpb, &readError);				// nBits
	if( readError )
			return false;

	submittedShare->sieveSize = xptPacketbuffer_readU32(cpb, &readError);			// sieveSize
	if( readError )
			return false;

	submittedShare->sieveCandidate = xptPacketbuffer_readU32(cpb, &readError);		// sieveCandidate
	if( readError )
			return false;

	// bnFixedMultiplier
	submittedShare->fixedMultiplierSize = xptPacketbuffer_readU8(cpb, &readError);
	if( readError )
			return false;

	xptPacketbuffer_readData(cpb, (uint8*)submittedShare->fixedMultiplier, submittedShare->fixedMultiplierSize, &readError);
	if( readError )
			return false;

	// bnChainMultiplier
	submittedShare->chainMultiplierSize = xptPacketbuffer_readU8(cpb, &readError);
	if( readError )
			return false;

	xptPacketbuffer_readData(cpb, (uint8*)submittedShare->chainMultiplier, submittedShare->chainMultiplierSize, &readError);
	if( readError )
			return false;

	// share id (server sends this back in shareAck, so we can identify share response)
	submittedShare->share_id = xptPacketbuffer_readU32(cpb, &readError);
	if( readError )
			return false;

	uint32_t iShareErrorCode = 1;
	std::string rejectReasonText = "";
	double fShareValue = 0.;
	// check share here
	if( xptServer->xptCallback_checkWork(xptServer, xptServerClient, submittedShare, &fShareValue, rejectReasonText ) == true )
		iShareErrorCode = 0;

	xptServer_sendShareResponse(xptServer, xptServerClient, iShareErrorCode, rejectReasonText, fShareValue);

	return true;
}

// reply to a ping
bool xptServer_processPacket_pingRequest(xptServer_t* xptServer, xptServerClient_t* xptServerClient)
{
	if( xptServerClient->clientState != XPT_CLIENT_STATE_LOGGED_IN )
			return false; // client not logged in

	// get packet buffer
	xptPacketbuffer_t* cpb = xptServerClient->packetbuffer;

	// start reading
	bool readError = false;
	xptPacketbuffer_beginReadPacket(cpb);

	// read timestamp
	uint64 timestamp = xptPacketbuffer_readU64(cpb, &readError);

	if( readError )
		return false;

	// echo the recieved stuff back
	bool sendError = false;
	xptPacketbuffer_beginWritePacket(xptServer->sendBuffer, XPT_OPC_S_PING);
	xptPacketbuffer_writeU64(xptServer->sendBuffer, &sendError, timestamp);

	// finalize
	xptPacketbuffer_finalizeWritePacket(xptServer->sendBuffer);
	send(xptServerClient->clientSocket, (const char*)(xptServer->sendBuffer->buffer), xptServer->sendBuffer->parserIndex, MSG_NOSIGNAL);

	if( sendError )
		return false;

	return true;
}
