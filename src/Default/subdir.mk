################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../addrman.cpp \
../alert.cpp \
../bitcoinrpc.cpp \
../bloom.cpp \
../bmp.cpp \
../checkpoints.cpp \
../checkpointsync.cpp \
../crypter.cpp \
../customBuffer.cpp \
../db.cpp \
../fMath.cpp \
../fastSorter.cpp \
../fastString.cpp \
../fastString_eprintf.cpp \
../fileMgr.cpp \
../hash.cpp \
../hashTable_uint32.cpp \
../init.cpp \
../key.cpp \
../keystore.cpp \
../leveldb.cpp \
../main.cpp \
../msgQueue.cpp \
../net.cpp \
../netbase.cpp \
../noui.cpp \
../packetBuffer.cpp \
../perlinNoise.cpp \
../prime.cpp \
../protocol.cpp \
../rpcblockchain.cpp \
../rpcdump.cpp \
../rpcmining.cpp \
../rpcnet.cpp \
../rpcrawtransaction.cpp \
../rpcwallet.cpp \
../sData.cpp \
../script.cpp \
../simpleHTTP.cpp \
../simpleList.cpp \
../streamWrapper.cpp \
../sync.cpp \
../tgaLib.cpp \
../txdb.cpp \
../util.cpp \
../version.cpp \
../wallet.cpp \
../walletdb.cpp \
../xptPacketbuffer.cpp \
../xptServer.cpp \
../xptServerPacketHandler.cpp 

OBJS += \
./addrman.o \
./alert.o \
./bitcoinrpc.o \
./bloom.o \
./bmp.o \
./checkpoints.o \
./checkpointsync.o \
./crypter.o \
./customBuffer.o \
./db.o \
./fMath.o \
./fastSorter.o \
./fastString.o \
./fastString_eprintf.o \
./fileMgr.o \
./hash.o \
./hashTable_uint32.o \
./init.o \
./key.o \
./keystore.o \
./leveldb.o \
./main.o \
./msgQueue.o \
./net.o \
./netbase.o \
./noui.o \
./packetBuffer.o \
./perlinNoise.o \
./prime.o \
./protocol.o \
./rpcblockchain.o \
./rpcdump.o \
./rpcmining.o \
./rpcnet.o \
./rpcrawtransaction.o \
./rpcwallet.o \
./sData.o \
./script.o \
./simpleHTTP.o \
./simpleList.o \
./streamWrapper.o \
./sync.o \
./tgaLib.o \
./txdb.o \
./util.o \
./version.o \
./wallet.o \
./walletdb.o \
./xptPacketbuffer.o \
./xptServer.o \
./xptServerPacketHandler.o 

CPP_DEPS += \
./addrman.d \
./alert.d \
./bitcoinrpc.d \
./bloom.d \
./bmp.d \
./checkpoints.d \
./checkpointsync.d \
./crypter.d \
./customBuffer.d \
./db.d \
./fMath.d \
./fastSorter.d \
./fastString.d \
./fastString_eprintf.d \
./fileMgr.d \
./hash.d \
./hashTable_uint32.d \
./init.d \
./key.d \
./keystore.d \
./leveldb.d \
./main.d \
./msgQueue.d \
./net.d \
./netbase.d \
./noui.d \
./packetBuffer.d \
./perlinNoise.d \
./prime.d \
./protocol.d \
./rpcblockchain.d \
./rpcdump.d \
./rpcmining.d \
./rpcnet.d \
./rpcrawtransaction.d \
./rpcwallet.d \
./sData.d \
./script.d \
./simpleHTTP.d \
./simpleList.d \
./streamWrapper.d \
./sync.d \
./tgaLib.d \
./txdb.d \
./util.d \
./version.d \
./wallet.d \
./walletdb.d \
./xptPacketbuffer.d \
./xptServer.d \
./xptServerPacketHandler.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


