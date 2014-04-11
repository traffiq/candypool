#include"global.h"
#include "xptServer.h"
#include "time.h"
#include <stddef.h>
#include <sys/ioctl.h>
#include "base58.h"
#include <netinet/tcp.h>

#include <openssl/sha.h>
#include <sys/epoll.h>

// generate a hash
std::string sha256(const std::string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// generate share value
void getShareValue( sint32 iChainLength, double *shareValue )
{
	if( iChainLength < 6 )
		*shareValue = 0.;
	else if( iChainLength == 6 )
		*shareValue = 0.0003;
	else if( iChainLength == 7 )
		*shareValue = 0.0006;
	else if( iChainLength == 8 )
		*shareValue = 0.006;
	else if( iChainLength == 9 )
		*shareValue = 0.038;
	else if( iChainLength == 10 )
		*shareValue = 0.1;
	else if( iChainLength >= 11 )
		*shareValue = 0.15;
}

/*
 * Called whenever we received a full packet from a client
 * Return false if the packet is invalid and the client should be disconnected
 */
bool xptServer_processPacket(xptServer_t* xptServer, xptServerClient_t* xptServerClient)
{
	if(xptServerClient == NULL)
		return false;

	if( xptServerClient->opcode == XPT_OPC_C_AUTH_REQ )
	{
		return xptServer_processPacket_authRequest(xptServer, xptServerClient);
	}
	else if( xptServerClient->opcode == XPT_OPC_C_SUBMIT_SHARE )
	{
		return xptServer_processPacket_submitShareRequest(xptServer, xptServerClient);
	}
	else if( xptServerClient->opcode == XPT_OPC_C_PING )
	{
		// client is sending pings, who cares :)
		return xptServer_processPacket_pingRequest(xptServer, xptServerClient);;
	}

	return true; // unknown opcode but we do not disconnect, maybe someone uses a custom miner or something
}

/*
 * Called whenever we received some bytes from a client
 */
bool xptServer_receiveData(xptServer_t* xptServer, xptServerClient_t* xptServerClient, sint32 &readReturnValue )
{
	if( xptServerClient == NULL )
		return false;

	sint32 packetFullSize = 4; // the packet always has at least the size of the header
	if( xptServerClient->recvSize > 0 )
		packetFullSize += xptServerClient->recvSize;
	sint32 bytesToReceive = (sint32)(packetFullSize - xptServerClient->recvIndex);
	// packet buffer is always large enough at this point
	readReturnValue = read(xptServerClient->clientSocket, (char*)(xptServerClient->packetbuffer->buffer+xptServerClient->recvIndex), bytesToReceive);
	if( readReturnValue <= 0 )
	{

		if (errno != EAGAIN || readReturnValue == 0) {
			xptServerClient->disconnected = true;
			return false;
		}

		// receive error, client disconnected
		return true;
	}
	// process header
	xptServerClient->recvIndex += readReturnValue;
	// header just received?
	if( xptServerClient->recvIndex == packetFullSize && packetFullSize == 4 )
	{
		// process header
		uint32_t headerVal = *(uint32_t*)xptServerClient->packetbuffer->buffer;
		uint32_t opcode = (headerVal&0xFF);
		uint32_t packetDataSize = (headerVal>>8)&0xFFFFFF;
		// validate header size
		if( packetDataSize >= (1024*1024*2-4) )
		{
			// packets larger than 4mb are not allowed
			fprintf(stderr, "xptServer_receiveData(): Packet exceeds 2mb size limit %s %d\n", xptServerClient->workerName, xptServerClient->payloadNum );
			return false;
		}
		xptServerClient->recvSize = packetDataSize;
		xptServerClient->opcode = opcode;
		// enlarge packetBuffer if too small
		if( (xptServerClient->recvSize+4) > xptServerClient->packetbuffer->bufferLimit )
		{
			xptPacketbuffer_changeSizeLimit(xptServerClient->packetbuffer, (xptServerClient->recvSize+4));
		}
	}
	// have we received the full packet?
	if( xptServerClient->recvIndex >= (xptServerClient->recvSize+4) )
	{
		// process packet
		xptServerClient->packetbuffer->bufferSize = (xptServerClient->recvSize+4);
		if( xptServer_processPacket(xptServer, xptServerClient) == false )
		{
			xptServerClient->disconnected = true;
			return false;
		}
		xptServerClient->recvIndex = 0;
		xptServerClient->opcode = 0;
		xptServerClient->recvSize = 0;
	}
	return true;
}

/*
 * Sends new block data to each client
 */
void xptServer_sendNewBlockToAll(xptServer_t* xptServer, std::vector<xptServerClient_t*> *arrClients, uint32_t coinTypeIndex)
{
	struct timespec ts;
	if(clock_gettime(1,&ts) != 0) {
	 //error
	}
	uint32_t time1 = ts.tv_sec * 1000; // to ms
	sint32 workerCount = 0;

	for( std::vector<xptServerClient_t*>::iterator it = arrClients->begin(); it != arrClients->end(); ++it)
	{
		xptServerClient_t* xptServerClient = (xptServerClient_t*)*it;
		if( xptServerClient == NULL || xptServerClient->disconnected || xptServerClient->clientState != XPT_CLIENT_STATE_LOGGED_IN )
			continue;

		// send block data
		if(xptServer_sendBlockData(xptServer, xptServerClient) == false )
		{
			xptServerClient->disconnected = true;
		}
		workerCount++;
	}

	if(clock_gettime(1,&ts) != 0) {
		 //error
	}
	uint32_t time2 = ( ts.tv_sec * 1000 ) - time1;
	fprintf(stderr, "Send blocks to %d workers in %dms\n", workerCount, time2);

	// we need a place to to store the number of workloads send to miners.
	// Why ? Because it is possible there are multiple machines behind a single worker account
	mysqlpp::Connection *conn = xptServer->conn();
	mysqlpp::Query query = conn->query();
	query << "UPDATE "<< mapArgs["-sqldatabase"] << ".`worker` SET `online` = "<< mysqlpp::quote << workerCount << " WHERE `id` = 1"; // 634 is a dummy in the database
	query.execute();
}

/*
 * Checks for new blocks that should be propagated
 */
void xptServer_checkForNewBlocks(xptServer_t* xptServer, std::vector<xptServerClient_t*> *arrClients)
{
	uint32_t numberOfCoinTypes = 0;
	uint32_t blockHeightPerCoinType[32] = {0};
	xptServer->xptCallback_getBlockHeight(xptServer, &numberOfCoinTypes, blockHeightPerCoinType);
	for(uint32_t i=0; i<numberOfCoinTypes; i++)
	{
		if( blockHeightPerCoinType[i] > xptServer->coinTypeBlockHeight[i] )
		{
			delete xptServer->currentBlock;
			xptServer->currentBlock = NULL;
			std::auto_ptr<CBlockTemplate> pblocktemplate(CreateNewBlock(*new CReserveKey(xptServer->pWallet)));
			if (!pblocktemplate.get())
				return;

			xptServer->currentBlock = &pblocktemplate->block;
			pblocktemplate.release();

			fprintf(stderr, "New block arrived.\n");
			xptServer->coinTypeBlockHeight[i] = blockHeightPerCoinType[i];
			xptServer_sendNewBlockToAll(xptServer, arrClients, i);
		}
	}
}

/*
 * Create and bind a socket
 */
static int create_and_bind (char *port)
{
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s, sfd;
  int on = 1;

  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
  hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
  hints.ai_flags = AI_PASSIVE;     /* All interfaces */

  s = getaddrinfo (NULL, port, &hints, &result);
  if (s != 0)
    {
      fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
      return -1;
    }

  for (rp = result; rp != NULL; rp = rp->ai_next)
    {
      sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (sfd == -1)
        continue;

      int rc = setsockopt(sfd, SOL_SOCKET,  SO_REUSEADDR,
      					(char *)&on, sizeof(on));
      if (rc < 0)
      {
    	  continue;
      }

      rc = setsockopt(sfd, IPPROTO_TCP,  TCP_NODELAY,
            					(char *)&on, sizeof(on));
      if (rc < 0)
		{
    	  continue;
		}

      s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
      if (s == 0)
        {
          /* We managed to bind successfully! */
          break;
        }

      close (sfd);
    }

  if (rp == NULL)
    {
      fprintf (stderr, "Could not bind\n");
      return -1;
    }

  freeaddrinfo (result);

  return sfd;
}

/*
 * make a socket non blocking
 */
static int make_socket_non_blocking (int sfd)
{
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1)
    {
      perror ("fcntl");
      return -1;
    }

  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1)
    {
      perror ("fcntl");
      return -1;
    }

  return 0;
}


unsigned int GetRandomNonce()
{
	timespec ts;
	clock_gettime(1, &ts);
	srand(ts.tv_nsec);

	// generate a random nonce
	unsigned int nExtraNonce = 0;
	const unsigned int arrayExtraNonceModulo[4] = {100000000, 1000000, 10000, 1000};
	const unsigned int nExtraNonceModulo = arrayExtraNonceModulo[rand() % 4];


	nExtraNonce = ts.tv_nsec % nExtraNonceModulo;

	return nExtraNonce;
}

// generate work for the workers
bool _xptServer_t::xptCallback_generateWork(xptServer_t* xptServer, xptServerClient_t* xptServerClient, uint32_t numOfWorkEntries,
		uint32_t coinTypeIndex, xptBlockWorkInfo_t* xptBlockWorkInfo,
		xptWorkData_t* xptWorkData) {

	if(xptServer == NULL || xptServerClient == NULL || xptBlockWorkInfo == NULL || xptWorkData == NULL)
		return false;

	boost::mutex::scoped_lock lock(xptServerClient->mutex); // lock first the mutex

	// clear blocks
	xptServerClient->arrCurrentBlocks.clear();

	// gernerate the current block
	unsigned int nExtraNonce = GetRandomNonce();

	// setup a new block

	// Each thread has its own key and counter
	if( xptServerClient->currentKey == NULL )
	{
		xptServerClient->currentKey = new CReserveKey(xptServer->pWallet);
	}

	xptServerClient->arrCurrentBlocks.push_back(*xptServer->currentBlock);
	CBlock *pCurrentBlock = &xptServerClient->arrCurrentBlocks.back();
	pCurrentBlock->vtx = xptServer->currentBlock->vtx;

	// Create coinbase tx
	CPubKey pubkey;
	if (!xptServerClient->currentKey->GetReservedKey(pubkey))
		return false;

	// Add our coinbase tx as first transaction
	pCurrentBlock->vtx[0].vout[0].scriptPubKey.clear();
	pCurrentBlock->vtx[0].vout[0].scriptPubKey << pubkey << OP_CHECKSIG;

	SetExtraNonce(pCurrentBlock, pindexBest, nExtraNonce, true);
	if (pCurrentBlock->nNonce >= 0xffff0000)
	{
		// should not happen 0xffff0000 == 4294901760
		pCurrentBlock->nNonce = 0;
	}

	// Check that the hash meets the minimum
	unsigned int dWhileCounter = 0;
	while (pCurrentBlock->GetHeaderHash() < hashBlockHeaderLimit)
	{
		// just for security
		dWhileCounter++;
		if(dWhileCounter >= 4294901760)
			break;

		IncrementExtraNonce(pCurrentBlock, pindexBest, pCurrentBlock->nNonce, true);
		if (pCurrentBlock->nNonce >= 0xffff0000)
		{
			fprintf(stderr, "Generate Work failed because we run out of nonces");
			nExtraNonce = 0;
			return false;
			continue;
		}
	}

	// miniing protocol 2 -> we are only allowed to send blocks where the blockheader is a prime
	/*
	 * // Primecoin: try to find hash that is probable prime
       do
       {
           uint256 hashBlockHeader = pblock->GetHeaderHash();
            if (hashBlockHeader < hashBlockHeaderLimit)
               continue; // must meet minimum requirement
            if (ProbablePrimalityTestWithTrialDivision(CBigNum(hashBlockHeader), 1000))
               break;
        } while (++(pblock->nNonce) < 0xffff0000);
	 * */

	xptServer->coinTypeBlockHeight[0] = pindexBest->nHeight + 1;

	// setup work for the client

	xptBlockWorkInfo->nBits = pCurrentBlock->GetBlockHeader().nBits;
	xptBlockWorkInfo->version = 2;
	xptBlockWorkInfo->nTime = GetTime();
	xptBlockWorkInfo->height = xptServer->coinTypeBlockHeight[0];
	xptBlockWorkInfo->nBitsShare = POOL_DIFFICULT;

	memcpy(xptBlockWorkInfo->prevBlock, &pCurrentBlock->hashPrevBlock,
			sizeof(pCurrentBlock->hashPrevBlock));

	memcpy(&*xptWorkData[0].merkleRoot, &pCurrentBlock->hashMerkleRoot,
					sizeof(pCurrentBlock->hashMerkleRoot));

	// now set work data for each additional payload
	for (uint32_t iPayload = 1; iPayload < numOfWorkEntries; iPayload++)
	{
		xptServerClient->arrCurrentBlocks.push_back(*xptServer->currentBlock);
		CBlock *pAdditionalNewBlock = &xptServerClient->arrCurrentBlocks.back();
		pAdditionalNewBlock->vtx = xptServer->currentBlock->vtx;

		// Create coinbase tx
		CPubKey pubkey;
		if (!xptServerClient->currentKey->GetReservedKey(pubkey))
			return false;
		pAdditionalNewBlock->vtx[0].vout[0].scriptPubKey.clear();
		pAdditionalNewBlock->vtx[0].vout[0].scriptPubKey << pubkey << OP_CHECKSIG;

		nExtraNonce = GetRandomNonce();

		SetExtraNonce(pAdditionalNewBlock, pindexBest, nExtraNonce, true);
		if (pAdditionalNewBlock->nNonce >= 0xffff0000)
		{
			// should not happen 0xffff0000 == 4294901760
			pAdditionalNewBlock->nNonce = 0;
		}

		// Check that the hash meets the minimum
		unsigned int dWhileCounterAdditional = 0;
		while (pAdditionalNewBlock->GetHeaderHash() < hashBlockHeaderLimit)
		{
			// just for security
			dWhileCounterAdditional++;
			if(dWhileCounterAdditional >= 4294901760)
				break;

			IncrementExtraNonce(pAdditionalNewBlock, pindexBest, pAdditionalNewBlock->nNonce, true);
			if (pAdditionalNewBlock->nNonce >= 0xffff0000)
			{
				fprintf(stderr, "Generate Work failed because we run out of nonces");
				nExtraNonce = 0;
				return false;
			}
		}

		// save work
		memcpy(&*xptWorkData[iPayload].merkleRoot, &pAdditionalNewBlock->hashMerkleRoot,
				sizeof(pAdditionalNewBlock->hashMerkleRoot));
	}

	return true;
}

// check current blockheit
void _xptServer_t::xptCallback_getBlockHeight(xptServer_t* xptServer, uint32_t* coinTypeNum,
		uint32_t* blockHeightPerCoinType) {
	*coinTypeNum = (unsigned int) 1;
	blockHeightPerCoinType[0] = pindexBest->nHeight + 1;
}

// check the submited share
bool _xptServer_t::xptCallback_checkWork(xptServer_t* xptServer,
		xptServerClient_t* xptServerClient, std::auto_ptr<xptSubmittedShare_t> submittedShare,
		double *shareValue, std::string &errorText) {

	boost::mutex::scoped_lock(xptServerClient->mutex); // lock first the mutex

	if( xptServer == NULL || xptServerClient == NULL )
	{
		fprintf(stderr, "<xptCallback_checkWork> Server or Client went null.\n");
		return false;
	}

	// check if the difficult is okay
	if (submittedShare->nBits < POOL_DIFFICULT) {
		errorText = "Submitted share was to low in difficult.";
		return false;
	}

	// setup blockheader for test
	std::auto_ptr<CBlockHeader> pBlockHeaderForTest( new CBlockHeader() );
	// convert prevBlock hash
	std::vector<uint8_t> vectorPrevBlockHash;
	uint256 hashPrevBlock;
	for (int i = 0; i < 32; i++) {
		vectorPrevBlockHash.push_back(submittedShare->prevBlockHash[i]);
	}
	pBlockHeaderForTest->hashPrevBlock = uint256(vectorPrevBlockHash);

	// convert merkle hash
	std::vector<uint8_t> vectorMerkle;
	for (int i = 0; i < 32; i++) {
		vectorMerkle.push_back(submittedShare->merkleRoot[i]);
	}

	pBlockHeaderForTest->hashMerkleRoot = uint256(vectorMerkle);

	// get the block that was send by the worker
	CBlock *pCurrentBlock = NULL;
	for(unsigned int iBlock = 0; iBlock < xptServerClient->arrCurrentBlocks.size(); iBlock++ )
	{
		CBlock *pPossibleBlock = &xptServerClient->arrCurrentBlocks.at(iBlock);
		if( pPossibleBlock->hashMerkleRoot == pBlockHeaderForTest->hashMerkleRoot )
		{
			// found it
			pCurrentBlock = pPossibleBlock;
			break;
		}
	}

	// found the given workload ?
	if( pCurrentBlock == NULL )
	{
		errorText = "No matching workload found. Share went stale.";
		return false;
	}

	// check if the submitted block is stale
	if (hashBestChain != pCurrentBlock->hashPrevBlock) {
		errorText = "Submitted share went stale.";
		return false;
	}

	// convert submitted data to bignum
	std::vector<unsigned char> bnSerializeData;
	bnSerializeData.reserve(submittedShare->chainMultiplierSize);
	for (int i = 0; i < submittedShare->chainMultiplierSize; i++) {
		bnSerializeData.push_back(submittedShare->chainMultiplier[i]);
	}

	CBigNum calculatedByClient = CBigNum(bnSerializeData);

	// setup blockheader
	pBlockHeaderForTest->nVersion = submittedShare->version;
	pBlockHeaderForTest->nTime = submittedShare->nTime;
	pBlockHeaderForTest->nBits = submittedShare->nBits;
	pBlockHeaderForTest->nNonce = submittedShare->nonce;
	pBlockHeaderForTest->bnPrimeChainMultiplier = calculatedByClient;

	// set the blockheader
	pCurrentBlock->SetBlockHeader(*pBlockHeaderForTest.get());
	if (pCurrentBlock->GetHeaderHash() < hashBlockHeaderLimit)
	{
		errorText = "Submitted share is invalid.";
		return false;
	}

	// check proof of work
	// we first check if the share is valid by checking if it would create a new block
	// if the difficult was as low as pool difficult
	unsigned int chainType = 0;
	unsigned int chainLength = 0;
	unsigned int shareTestBits = POOL_DIFFICULT;
	if (!CheckPrimeProofOfWork(pCurrentBlock->GetHeaderHash(),
			shareTestBits,
			pCurrentBlock->bnPrimeChainMultiplier, chainType,
			chainLength,
			true ))
	{
		errorText = "Submitted share is invalid.";
		return false;
	}

	// check if the client tries to double submit a share
	/*if( CheckDoubleSubmit(xptServer, xptServer->coinTypeBlockHeight[0], pBlockHeaderForTest->bnPrimeChainMultiplier.ToString(), xptServerClient->workerId) == true )
	{
		errorText = "Double submit detected.";
		return false;
	}*/

	// determine sharevalue
	sint32 shareDifficultyMajor = (sint32)(chainLength>>24);
	getShareValue( shareDifficultyMajor, shareValue );

	// check if we have a new block
	uint32_t chainTypeNewBlock = 0;
	uint32_t chainLengthNewBlock = 0;
	bool bFoundNewBlock = true;
	if (!CheckPrimeProofOfWork(pCurrentBlock->GetHeaderHash(),
				pCurrentBlock->nBits,
				pCurrentBlock->bnPrimeChainMultiplier, chainTypeNewBlock,
				chainLengthNewBlock))
	{
		// valid share but difficult to low so we dont get a new block :(
		bFoundNewBlock = false;
	}

	if( bFoundNewBlock == true )
	{
		// increase sharevalue as boni for finding a block
		getShareValue(99, shareValue);
	}

	// save share to database
	// check pass and workername
	bool bSaveShare = SaveShare( xptServer, pBlockHeaderForTest->bnPrimeChainMultiplier.ToString(), shareDifficultyMajor, xptServer->coinTypeBlockHeight[0], *shareValue, xptServerClient->workerId, xptServerClient->userId);

	if(!bSaveShare)
		fprintf(stderr, "<IMPORTANT> Could not save share ! !:\n");

	// if we did not find a block we leave here; share is already saved to db
	if( bFoundNewBlock == false )
	{
		return true;
	}

	// we found a new block
	fprintf(stderr, "<IMPORTANT>FOUND BLOCK !:\n");

	// DEBUG
	{
		pCurrentBlock->print();
	}


	fprintf(stderr, "<IMPORTANT>Trying our luck with the current block:\n");
	{
		// Found a solution
		{
			if (pCurrentBlock->hashPrevBlock
					!= hashBestChain) {
				fprintf(stderr, "<IMPORTANT> generated block is stale");
				errorText = "PrimecoinMiner : generated block is stale";
				return false;
			}
			else
			{
				// Remove key from key pool
				if( xptServerClient->currentKey == NULL )
				{
					fprintf(stderr, "<IMPORTANT> Processblock failed - key null !!!!!!:\n");
					return false;
				}
				xptServerClient->currentKey->KeepKey();

				// Track how many getdata requests this block gets
				{
					LOCK(xptServer->pWallet->cs_wallet);
					xptServer->pWallet->mapRequestCount[pCurrentBlock->GetHash()] =	0;
				}

				// Process this block the same as if we had received it from another node
				CValidationState state;
				if (!ProcessBlock(state, NULL, pCurrentBlock )) {
					fprintf(stderr, "<IMPORTANT> Processblock failed !!!!!!:\n");
					errorText = "ProcessBlock, block not accepted";
					return false;
				}

				if(bFoundNewBlock)
				{
					// save block to db
					bool bSaveBlock = SaveBlock( xptServer, pCurrentBlock );
					if(!bSaveBlock)
					{
						fprintf(stderr, "<IMPORTANT> Could not save BLOCK !!!!!!:\n");
						return false;
					}
					else
					{
						return true;
					}
				}
			}
		}
	}

	return true;
}

#define MAXEVENTS 100
void xptServer_startProcessingEPoll(CWallet* pwallet)
{
	SetThreadPriority(THREAD_PRIORITY_NORMAL);
	RenameThread("primecoin-pool");

	// sleep so the wallet can do its thing
	sleep(60);

	int sfd, s;

	if( mapArgs["-poolport"] == "" )
	{
		fprintf(stderr, "Poolport not set !\n");
		return;
	}

	sfd = create_and_bind(const_cast<char *>(mapArgs["-poolport"].c_str()));
	if (sfd == -1)
	abort ();

	s = make_socket_non_blocking(sfd);
	if (s == -1)
	abort ();

	s = listen (sfd, SOMAXCONN);
	if (s == -1)
	{
	  perror ("listen");
	  abort ();
	}

	int nThreads = GetArg("-threads", 0);
	if (nThreads <= 0)
		nThreads = 1;

	std::vector<int> arrEpollListeners;
	arrEpollListeners.reserve( nThreads );
	for( int iThread = 0; iThread < nThreads; iThread++)
	{
		int efdServer = epoll_create1 (0);
		arrEpollListeners.push_back(efdServer);
		new boost::thread(StartListeningThread, pwallet, efdServer);
	}

	struct epoll_event event;
	struct epoll_event *events;

	int efd = epoll_create1 (0);
	if (efd == -1)
	{
	  perror ("epoll_create");
	  abort ();
	}

	event.data.fd = sfd;
	event.events = EPOLLIN | EPOLLET;
	s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event);
	if (s == -1)
	{
	  perror ("epoll_ctl");
	  abort ();
	}

	/* Buffer where events are returned */
	events = (epoll_event*)calloc (MAXEVENTS, sizeof event);

	/* The event loop */
	int iThreadUsed = 0;
	while (1)
	{
	  int n, i;

	  n = epoll_wait (efd, events, MAXEVENTS, -1);

	  for (i = 0; i < n; i++)
	  {
		  if ((events[i].events & EPOLLERR) ||
				  (events[i].events & EPOLLHUP) ||
				  (!(events[i].events & EPOLLIN)))
		  {
			  /* An error has occured on this fd, or the socket is not
				 ready for reading (why were we notified then?) */

			  fprintf(stderr, "epoll error: Server acceptsocket died.\n");
			  epoll_ctl(efd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
			  continue;
		  }
		  else if ( events[i].data.fd == sfd )
		  {
			  /* We have a notification on the listening socket, which
				means one or more incoming connections. */
			  while (1)
			  {
				  struct sockaddr in_addr;
				  socklen_t in_len;
				  int infd;
				  char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
				  in_len = sizeof in_addr;
				  infd = accept (sfd, &in_addr, &in_len);

				  if (infd == -1)
				  {
					  if ((errno == EAGAIN) ||
						  (errno == EWOULDBLOCK))
					  {
						  /* We have processed all incoming
							 connections. */
						  break;
					  }
					  else
					  {
						  perror ("accept");
						  break;
					  }
				  }

				  s = getnameinfo (&in_addr, in_len,
								   hbuf, sizeof hbuf,
								   sbuf, sizeof sbuf,
								   NI_NUMERICHOST | NI_NUMERICSERV);
				  if (s == 0)
				  {
					  fprintf(stderr, "+");
					  /*fprintf(stderr, "Accepted connection on descriptor %d "
							 "(host=%s, port=%s)\n", infd, hbuf, sbuf);*/
				  }

				  /* Make the incoming socket non-blocking and add it to the
					 list of fds to monitor. */
				  s = make_socket_non_blocking (infd);
				  if (s == -1)
				  {
					fprintf(stderr, "<DEBUG> Abort 1");
					abort ();
				  }

				  xptServerClient_t* client = new xptServerClient_t();
				  client->clientSocket = infd;
				  event.data.ptr = client;
				  event.events = EPOLLIN | EPOLLET;

				  if( iThreadUsed >= nThreads )
					  iThreadUsed = 0;

				  s = epoll_ctl (arrEpollListeners.at(iThreadUsed), EPOLL_CTL_ADD, infd, &event);
				  iThreadUsed++;
				  if (s == -1)
				  {
					  perror ("epoll_ctl");
					  abort ();
				  }
			  }
			  continue;
		  }
	  }

	}

	free (events);
}

void StartListeningThread( CWallet* pwallet, int efd)
{
	fprintf(stderr, "xptServer instance started.\n");
	//setup xpt server
	xptServer_t *server = new xptServer_t();
	server->pWallet = pwallet;

	std::auto_ptr<CBlockTemplate> pblocktemplate(CreateNewBlock(*new CReserveKey(pwallet)));
	if (!pblocktemplate.get())
		return;

	server->currentBlock = &pblocktemplate->block;
	pblocktemplate.release();

	// container to store monitored clients
	std::vector<xptServerClient_t*> arrClientsMonitored;
	arrClientsMonitored.reserve(10000);

	struct epoll_event event;
	struct epoll_event *events;

	event.data.ptr = NULL;
	event.events = EPOLLIN | EPOLLET;

	/* Buffer where events are returned */
	events = (epoll_event*)calloc (MAXEVENTS, sizeof event);

	/* The event loop */
	while (1)
	{
	  int n, i;

	  n = epoll_wait (efd, events, MAXEVENTS, 100);

	  for (i = 0; i < n; i++)
	  {
		  if ((events[i].events & EPOLLERR) ||
				  (events[i].events & EPOLLHUP) ||
				  (!(events[i].events & EPOLLIN)))
		  {
			  /* An error has occured on this fd, or the socket is not
				 ready for reading (why were we notified then?) */

			  xptServerClient_t* client = NULL;
			  if(events[i].data.ptr != NULL)
			  {
				  client = (xptServerClient_t*)events[i].data.ptr;
			  }

			  if(client != NULL)
			  {
				  client->disconnected = true;
				  epoll_ctl(efd, EPOLL_CTL_DEL, client->clientSocket, &events[i]);

				  // drop out of the monitored list
				  for(unsigned int i = 0; i < arrClientsMonitored.size(); i++ )
				  {
					  if(arrClientsMonitored.at(i) == client)
					  {
						  arrClientsMonitored.erase(arrClientsMonitored.begin() + i);
					  }
				  }

				  delete client;
				  client = NULL;
				  fprintf(stderr, "-");
			  }
			  else
			  {
				  fprintf(stderr, "epoll error: xptClient is Null\n");
			  }

			  continue;
		  }
		  else if ( events[i].data.ptr != NULL )
		  {
			  /* We have data on the fd waiting to be read. Read and
				 display it. We must read whatever data is available
				 completely, as we are running in edge-triggered mode
				 and won't get a notification again for the same
				 data. */
			  int done = 0;

			  xptServerClient_t* client = (xptServerClient_t*)events[i].data.ptr;

			  if(client != NULL)
			  {
				  if( client->bMonitored == false )
				  {
					  arrClientsMonitored.push_back( client );
					  client->bMonitored = true;
				  }

				  while (1)
				  {
					  sint32 retRead = 0;
					  if( xptServer_receiveData(server, client, retRead) == false )
					  {
						  done = 1;
						  break;
					  }
					  else
					  {
						  if (retRead == -1)
						  {
							  /*  If errno == EAGAIN, that means we have read all
								  data. So go back to the main loop. */
							  if (errno != EAGAIN)
							  {
								perror ("read");
								done = 1;
							  }
							  break;
						  }
						  else if (retRead == 0)
						  {
							  /* End of file. The remote has closed the
								 connection. */
							  done = 1;
							  break;
						  }
					  }
					}
			  }
			  else
			  {
				  fprintf(stderr, "Client went null - should not happen !");
				  done = 1;
			  }

			  if (done)
			  {
				  /* Closing the descriptor will make epoll remove it
				  	from the set of descriptors which are monitored. */
				  if(client != NULL)
				  {
					  client->disconnected = true;
					  epoll_ctl(efd, EPOLL_CTL_DEL, client->clientSocket, &events[i]);

					  // drop out of the monitored list
					  for(unsigned int i = 0; i < arrClientsMonitored.size(); i++ )
					  {
						  if(arrClientsMonitored.at(i) == client)
						  {
							  arrClientsMonitored.erase(arrClientsMonitored.begin() + i);
						  }
					  }
					  delete client;
					  client = NULL;
				  }
				  else
				  {
					  fprintf(stderr, "epoll error: xptClient is Null after recieving.\n");
				  }
			  }
		  }
	   }

	  if( !arrClientsMonitored.empty() )
		  xptServer_checkForNewBlocks(server, &arrClientsMonitored);
	}

	free (events);
	delete server;
	arrClientsMonitored.clear();
}

/*
 * checks for confirmed blocks and pays them
 */
void xptServer_payWallet(mysqlpp::Connection *conn, CWallet* pwallet)
{
	try
	{
		// get all wallets with more then 3 xpm
		mysqlpp::Query query = conn->query();
		query << "SELECT * FROM `wallet` WHERE `amount` >= 3 AND `payoutadress` != '';";
		if (mysqlpp::StoreQueryResult res = query.store()) {
			mysqlpp::StoreQueryResult::const_iterator it;
			for (it = res.begin(); it != res.end(); ++it) {
				mysqlpp::Row wallet = *it;

				// payout
				bool bSuccess = xptServer_payOut(conn, wallet["user_id"], pwallet );
				if( bSuccess == false )
				{
					fprintf(stderr, "<IMPORTANT> payout for user %s failed.", wallet["user_id"].c_str());
				}
			}
		}

		// now check the instant payouts
		query << "SELECT * FROM `instant_payout_queue`;";
		if (mysqlpp::StoreQueryResult res = query.store()) {
			mysqlpp::StoreQueryResult::const_iterator it;
			for (it = res.begin(); it != res.end(); ++it) {
				mysqlpp::Row instantPayout = *it;

				// now get the wallet
				mysqlpp::Query queryWallet = conn->query();
				queryWallet << "SELECT * FROM `wallet` WHERE `user_id` = "
								"" << mysqlpp::quote << instantPayout["user"] << ""
								" AND `amount` >= 0.02 AND `payoutadress` != '';";

				/*std::string strQuery = queryWallet.str();
				fprintf(stderr, "%s", queryWallet.c_str());*/

				if (mysqlpp::StoreQueryResult resWallet = queryWallet.store())
				{
					mysqlpp::StoreQueryResult::const_iterator itWallet;
					for (itWallet = resWallet.begin(); itWallet != resWallet.end(); ++itWallet)
					{
						mysqlpp::Row wallet = *itWallet;

						// payout
						bool bSuccess = xptServer_payOut(conn, wallet["user_id"], pwallet );
						if( bSuccess == false )
						{
							fprintf(stderr, "<IMPORTANT> payout for user %s failed.", wallet["user_id"].c_str());
						}

						// remove from queue
						mysqlpp::Query queryDropInstant = conn->query();
						queryDropInstant << "DELETE FROM `instant_payout_queue` WHERE `user` = "
											"" << mysqlpp::quote << wallet["user_id"] << ";";
						queryDropInstant.execute();
					}
				}
			}
		}
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		fprintf(stderr, "<IMPORTANT> %s", er.what());
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		fprintf(stderr, "<IMPORTANT> %s",er.what());
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		fprintf(stderr, "<IMPORTANT> %s",er.what());
	}
}

/*
 * pay xpm
 */
bool xptServer_payOut(mysqlpp::Connection *conn, const uint32_t &user_id, CWallet* pwallet )
{
	try
	{
		// get the wallet
		mysqlpp::Query query = conn->query();
		query << "select * from "<< mapArgs["-sqldatabase"] << ".`wallet` where `user_id` = " << mysqlpp::quote << user_id << ";";
		if (mysqlpp::StoreQueryResult res = query.store()) {
			mysqlpp::StoreQueryResult::const_iterator it;

			if( res.size() != 1 )
			{
				fprintf(stderr, "<IMPORTANT> Payout failed because we found multiple wallets for one user.");
				return false;
			}

			for (it = res.begin(); it != res.end(); ++it)
			{
				mysqlpp::Row wallet = *it;

				// setup payment variables
				double walletBalance = wallet["amount"];
				std::string destination = wallet["payoutadress"].c_str();

				if( destination.empty() )
				{
					fprintf(stderr, "<INFO> Payout failed because user has no payout adress.");
					return false;
				}
				CBitcoinAddress address(destination);
				if (!address.IsValid())
				{
					fprintf(stderr, "<INFO> Payout failed because user has Invalid payout adress.");
					return false;
				}

				// minimum is 0.02 XPM ( because of 0.01 fee
				if( walletBalance < 0.02 )
				{
					return false;
				}

				if( pwallet == NULL )
				{
					fprintf(stderr, "<IMPORTANT> Payout failed because wallet is null.");
					return false;
				}
				else
				{
					LOCK(cs_main);
					if(pwallet->IsLocked() )
					{
						pwallet->Unlock("CHANGEME");
					}

					// Amount
					int64 nAmount = AmountFromValue(walletBalance);
					if (nAmount < MIN_TXOUT_AMOUNT)
					{
						fprintf(stderr, "<IMPORTANT> Send amount too small.");
					}

					// wallet has to less money ?
					if( nAmount > pwallet->GetBalance())
					{
						// better run :(
						fprintf(stderr, "<IMPORTANT> Amount to send is more then we have !!!");
						return false;
					}

					// setup transaction
					CWalletTx wtx;
					wtx.mapValue["comment"] = "candypool.net - thank you :)";

					// setup payout adress
					CBitcoinAddress address(destination);
					if(!address.IsValid())
					{
						fprintf(stderr, "<IMPORTANT> XPM Adress invalid <%s>", destination.c_str());
						return false;
					}

					//determine fee
					CReserveKey reservekey(pwallet);
					int64 nFeeRequired;
					std::string strErrorTransaction;
					CScript scriptPubKey;
					scriptPubKey.SetDestination(address.Get());
					pwallet->CreateTransaction(scriptPubKey, nAmount, wtx, reservekey, nFeeRequired, strErrorTransaction);
					nAmount -= nFeeRequired;

					std::string strError = pwallet->SendMoneyToDestination(address.Get(), nAmount, wtx);
					if (strError != "")
					{
						fprintf(stderr, "<IMPORTANT> Payment error: %s", strError.c_str() );
						return false;
					}

					fprintf(stderr, "<INFO> Send %lld to %d", nAmount, user_id );

					// mark in journal
					query << "INSERT INTO "<< mapArgs["-sqldatabase"] << ".`journal` (`id` ,`block` ,`share` ,`user` ,`value` ,`time`) VALUES (NULL ,"
							" NULL,"
							" NULL,"
							"" << mysqlpp::quote << wallet["user_id"] << ","
							"" << mysqlpp::quote << (walletBalance * -1) << ","
							"CURRENT_TIMESTAMP);";
					query.execute();

					// update wallet
					query << "UPDATE "<< mapArgs["-sqldatabase"] << ".`wallet` SET `amount` = "
							""<< mysqlpp::quote << 0 << ""
							" WHERE `wallet`.`user_id` = "
							" " << mysqlpp::quote << wallet["user_id"] << ";";
					query.execute();

					pwallet->Lock();
					return true;
				}
			}
		}
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		fprintf(stderr, "<IMPORTANT> %s", er.what());
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		fprintf(stderr, "<IMPORTANT> %s",er.what());
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		fprintf(stderr, "<IMPORTANT> %s",er.what());
	}

	return false;
}

/*
 * give the money to the people
 */
void xptServer_payConfirmedBlocks(mysqlpp::Connection *conn, CWallet* pwallet)
{
	// check that there is enough time to pay everything
	// ( avoid restart times )
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	//	restart are at following hours at 00 minutes: 0 1,6,11,16,21
	// only allow payouts if in the window between (XX:05 and XX:45)
	int minute = timeinfo->tm_min;
	if( minute < 5 || minute > 45 )
		return;

	// collect all unpayed blocks
	try
	{
		if(pwallet->IsLocked() )
		{
			pwallet->Unlock("CHANGEME");
		}

		if( pwallet->GetBalance() < 0. )
		{
			fprintf(stderr, "Trying to payout block but there is no money ?");
			return;
		}

		// setup payment variables
		std::string destination = "CHANGEME"; // primecoin adress to send the autopayment to
		std::string destinationFee = "CHANGEME"; // primecoin adress to send the fee to

		CBitcoinAddress address(destination);
		if (!address.IsValid())
		{
			fprintf(stderr, "<INFO> Payout failed because user has Invalid payout adress.");
			return;
		}

		if( pwallet == NULL )
		{
			fprintf(stderr, "<IMPORTANT> Payout failed because wallet is null.");
			return;
		}
		else
		{
			// Amount
			int64 nAmount = pwallet->GetBalance();
			int64 fee = nAmount * 0.02;
			nAmount -= fee;
			if(ValueFromAmount(nAmount).get_real() < 9 )
				return;

			LOCK(cs_main);

			if (nAmount < MIN_TXOUT_AMOUNT)
			{
				fprintf(stderr, "<IMPORTANT> Send amount too small.");
			}

			// setup transaction
			CWalletTx wtx;

			// setup payout adress
			CBitcoinAddress address(destination);
			CBitcoinAddress addressFee(destinationFee);
			if(!address.IsValid())
			{
				fprintf(stderr, "<IMPORTANT> XPM Adress invalid <%s>", destination.c_str());
				return;
			}

			if(!addressFee.IsValid())
			{
				fprintf(stderr, "<IMPORTANT> XPM FEE Adress invalid <%s>", destinationFee.c_str());
				return;
			}

			//setup payment
			CReserveKey reservekey(pwallet);
			int64 nFeeRequired;
			std::string strErrorTransaction;
			CScript scriptPubKey;
			scriptPubKey.SetDestination(address.Get());
			pwallet->CreateTransaction(scriptPubKey, nAmount, wtx, reservekey, nFeeRequired, strErrorTransaction);
			nAmount -= nFeeRequired;

			std::string strError = pwallet->SendMoneyToDestination(address.Get(), nAmount, wtx);
			if (strError != "")
			{
				fprintf(stderr, "<IMPORTANT> Payment error: %s", strError.c_str() );
				return;
			}

			fprintf(stderr, "<INFO> Send %lld XPM", nAmount);

			//setup fee payment
			CReserveKey reservekeyFee(pwallet);
			nFeeRequired = 0;
			CScript scriptPubKeyFee;
			scriptPubKeyFee.SetDestination(addressFee.Get());
			pwallet->CreateTransaction(scriptPubKeyFee, fee, wtx, reservekeyFee, nFeeRequired, strErrorTransaction);
			fee -= nFeeRequired;

			strError = pwallet->SendMoneyToDestination(addressFee.Get(), fee, wtx);
			if (strError != "")
			{
				fprintf(stderr, "<IMPORTANT> Payment error: %s", strError.c_str() );
				return;
			}

			fprintf(stderr, "<INFO> Send %lld XPM", fee);
		}
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		fprintf(stderr, "<IMPORTANT> %s", er.what());
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		fprintf(stderr, "<IMPORTANT> %s",er.what());
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		fprintf(stderr, "<IMPORTANT> %s",er.what());
	}
}

/*
 * checks block in the db for confirmation
 */
void xptServer_checkBlockConfirmations(mysqlpp::Connection *conn, CWallet* pwallet)
{
	// collect all unpayed blocks
	while( true )
	{
		if(pwallet->IsLocked() )
		{
			pwallet->Unlock("CHANGEME");
		}

		fprintf(stderr, "<INFO> Current Balance %f:\n", ValueFromAmount(pwallet->GetBalance()).get_real());

		try
		{
			mysqlpp::Query query = conn->query();
			query << "select `hash`, `id` from "<< mapArgs["-sqldatabase"] << ".`blocks` where `confirmations` < 3500 AND `payed` = 0";
			if (mysqlpp::StoreQueryResult res = query.store()) {
				mysqlpp::StoreQueryResult::const_iterator it;
				for (it = res.begin(); it != res.end(); ++it) {
					mysqlpp::Row row = *it;

					// get the hash
					std::string strBlockHash = row["hash"].c_str();
					if(strBlockHash.length() > 0)
					{
						uint256 blockHash = 0;
						blockHash.SetHex(strBlockHash);
						std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(blockHash);
						if (mi == mapBlockIndex.end())
						{
							// block not found
							fprintf(stderr, "CheckConfirmation: Block not found <%s>", strBlockHash.c_str());
						}
						else
						{
							CBlockIndex* pindex = (*mi).second;
							if (pindex->IsInMainChain())
							{
								CBlock block;
								block.ReadFromDisk(pindex);
								CMerkleTx txGen(block.vtx[0]);
								txGen.SetMerkleBranch(&block);
								int iConfirmations = txGen.GetDepthInMainChain();

								// update the block
								query << "UPDATE "<< mapArgs["-sqldatabase"] << ".`blocks` SET `confirmations` = " << mysqlpp::quote << iConfirmations << ""
										",`blocks`.`orphan` = 0"
										" WHERE `blocks`.`id` = " << mysqlpp::quote << row["id"];
								query.execute();
							}
							else
							{
								// orphaned -> but if we are lucky it comes back :/
								query << "UPDATE "<< mapArgs["-sqldatabase"] << ".`blocks` SET `blocks`.`orphan` = 1 WHERE `blocks`.`id` = " << mysqlpp::quote << row["id"];
								query.execute();
							}
						}
					}
				}
			}
		}
		catch (const mysqlpp::BadQuery& er) {
			// Handle any query errors
			fprintf(stderr, "%s", er.what());
		}
		catch (const mysqlpp::BadConversion& er) {
			// Handle bad conversions
			fprintf(stderr, "%s",er.what());
		}
		catch (const mysqlpp::Exception& er) {
			// Catch-all for any other MySQL++ exceptions
			fprintf(stderr, "%s",er.what());
		}

		// sleep for 5 minutes

		// a private pool doesn't need to payout to users
		// the owner already has the pool wallet.
		// if a public pool is desired uncomment the following lines

		xptServer_payConfirmedBlocks(conn, pwallet);
		//xptServer_payWallet(conn, pwallet);


		sleep(300);
	}
}

/*
 * check if the share is a double submit
 */
bool CheckDoubleSubmit(xptServer_t* xptServer, const uint32_t &blockHeight, const std::string &strPrimeChain, const uint32_t &workerId)
{
	try
	{
		mysqlpp::Connection *conn = xptServer->conn();
		mysqlpp::Query query = conn->query();


		query << "select `id` from "<< mapArgs["-sqldatabase"] << ".`shares` where `share` = " << mysqlpp::quote << strPrimeChain << ""
				" AND `blockheight` = "<< mysqlpp::quote << blockHeight << ""
				" AND `worker_id` = " << mysqlpp::quote << workerId;

		/*std::string strQuery = query.str();
		fprintf(stderr, "%s", strQuery.c_str());*/

		mysqlpp::StoreQueryResult res = query.store();

		if( res.num_rows() > 0 )
		{
			return true;
		}
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		fprintf(stderr, "%s", er.what());
		return true;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		fprintf(stderr, "%s",er.what());
		return true;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		fprintf(stderr, "%s",er.what());
		return true;
	}

	return false;
}
/*
 * save share to db
 */
bool SaveShare( xptServer_t* xptServer,
				const std::string &strPrimeChain,
				const uint32_t &chainLength,
				const uint32_t &blockHeight,
				const double &shareValue,
				const uint32_t &workerId,
				const uint32_t &userId )
{

	if( xptServer == NULL )
	{
		fprintf(stderr, "<SaveShare> XPTServer was NULL");
		abort();
	}

	try
	{
		mysqlpp::Connection *conn = xptServer->conn();
		mysqlpp::Query query = conn->query();

		query << "INSERT INTO "<< mapArgs["-sqldatabase"] << ".`shares` (`id`, `share`, `share_length`, `share_value`, `worker_id`, `time`, `user_id`, `cointype`, `blockheight`) "
				"VALUES (NULL,"
				""<< mysqlpp::quote << strPrimeChain << ""
				", "<< mysqlpp::quote << chainLength << ""
				", "<< mysqlpp::quote << shareValue << ""
				", "<< mysqlpp::quote << workerId << ""
				", CURRENT_TIMESTAMP"
				", "<< mysqlpp::quote << userId << ""
				", '1'"
				","<< mysqlpp::quote << blockHeight << ");";

		query.execute();
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		fprintf(stderr, "%s", er.what());
		return false;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		fprintf(stderr, "%s",er.what());
		return false;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		fprintf(stderr, "%s",er.what());
		return false;
	}

	return true;
}

/*
 * save block to db
 */
bool SaveBlock( xptServer_t* xptServer, CBlock *pBlock )
{
	if( pBlock == NULL )
	{
		fprintf(stderr, "<SaveBlock> Block was NULL");
		return false;
	}

	if( xptServer == NULL )
	{
		fprintf(stderr, "<SaveBlock> XPTServer was NULL");
		return false;
	}

	try
	{
		mysqlpp::Connection *conn = xptServer->conn();
		mysqlpp::Query query = conn->query();
		double value = ValueFromAmount(pBlock->vtx[0].vout[0].nValue).get_real();
		query << "INSERT INTO "<< mapArgs["-sqldatabase"] << ".`blocks` (`id`, `hash`, `height`, `time`, `value`, `confirmations`, `payed`, `orphan`, `difficult`) VALUES (NULL,"
				""<< mysqlpp::quote << pBlock->GetHash().ToString() <<","
				""<< mysqlpp::quote <<  xptServer->coinTypeBlockHeight[0] <<","
				" CURRENT_TIMESTAMP,"
				""<< mysqlpp::quote << value <<","
				" '0',"
				" '0',"
				" '0',"
				" " << mysqlpp::quote << GetPrimeDifficulty(pBlock->GetBlockHeader().nBits) << ");";

		query.execute();
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		fprintf(stderr, "%s", er.what());
		return false;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		fprintf(stderr, "%s",er.what());
		return false;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		fprintf(stderr, "%s",er.what());
		return false;
	}

	return true;
}
