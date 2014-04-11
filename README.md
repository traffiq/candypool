This is a opensource primecoin pool based on the xpt protocol of ypool.
You can use any miner that supports the xpt protocol.

This code is based on a private pool build.
So there is no user managment enabled and the PPLNS System is not running too.
If you need that just browse the code and comment the stuff back in.

Features:
- autopayment to a given address in code ( xptServer.cpp )
- unique work for every worker
- can handle high load very well ( tested with more then 100.000 miners )
- mysql backend

How to install:

A) Database
1. Setup a mysql database and import the pool.sql file.
Make sure you are creating a innodb database.
Create a user for the poolserver to access this database.

B) Poolserver
1. Installing the required dependencies ( Ubuntu )

sudo apt-get install -y build-essential m4 libssl-dev libdb++-dev libboost-all-dev libminiupnpc-dev libmysql++-dev libmysqlclient-dev

Now you need to setup gmp with the following commands:
cd
rm -rf gmp-5.1.2.tar.bz2 gmp-5.1.2
wget http://mirrors.kernel.org/gnu/gmp/gmp-5.1.2.tar.bz2
tar xjvf gmp-5.1.2.tar.bz2
cd gmp-5.1.2
./configure --enable-cxx
make
sudo make install

Important:
Because encrypting the wallet is important you are supposed to change the wallet key in the source code.
The wallet key is hardcoded because it is more secure than to store it in a config file.
Open the file "main.cpp" and "xptServer.cpp" and search for "CHANGEME":

"pwallet->EncryptWallet("CHANGEME");"
Change "CHANGEME" to the wallet passphrase of your choice.
Do the same with:
pwallet->Unlock("CHANGEME");

As you now have all dependencies you can compile the poolserver itself:
Go into the sourcecode folder and type
make
strip primecoind

Copy the file primecoind into the home directory of the user you plan to run it with.
For example:
cp primecoind /home/pool/

If everything compiles without errors you need to create a configfile for the server.
The configfile must be in the ".primecoin" directory of the user running the poolserver.

Create a new system user called pool. After that enter the following commands:

su pool
cd
mkdir -p .primecoin
echo 'server=1
gen=1
server=1
rpcallowip=127.0.0.1
rpcuser=primecoinrpc
rpcpassword=SOME_SECURE_PASSWORD
threads=4
poolport=10034
sqlhost=localhost
sqluser=pool
sqldatabase=pool
sqlpassword=CHANGEMEBYHAND' > .primecoin/primecoin.conf
sed -i -e "s/SOME_SECURE_PASSWORD/`< /dev/urandom tr -cd '[:alnum:]' | head -c32`/" .primecoin/primecoin.conf

Make sure to replace the sql credentials in the .primecoin.conf file with the correct ones. 

Starting the poolserver:
cd 
chmod 770 primecoind
./primecoind

Important: Wait till the server has catched up with the blockchain until you connect your miner.
Now try to connect with your miner to your_ip:poolport

What do the custom parameters mean ?
- sql* = sql connection info
- threads = how much threads shall be spawned to handle the workers

Donations are welcome:
XPM AbCbdrpuxMqcKqNSTqs22RDvodh29HdR2X
BTC 1JRTVsHvU1nQe75mfroCyL9ZPxeKDWnGx2