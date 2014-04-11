################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test/Checkpoints_tests.cpp \
../test/DoS_tests.cpp \
../test/accounting_tests.cpp \
../test/alert_tests.cpp \
../test/allocator_tests.cpp \
../test/base32_tests.cpp \
../test/base58_tests.cpp \
../test/base64_tests.cpp \
../test/bignum_tests.cpp \
../test/bloom_tests.cpp \
../test/canonical_tests.cpp \
../test/checkblock_tests.cpp \
../test/compress_tests.cpp \
../test/getarg_tests.cpp \
../test/key_tests.cpp \
../test/miner_tests.cpp \
../test/mruset_tests.cpp \
../test/multisig_tests.cpp \
../test/netbase_tests.cpp \
../test/pmt_tests.cpp \
../test/rpc_tests.cpp \
../test/script_P2SH_tests.cpp \
../test/script_tests.cpp \
../test/serialize_tests.cpp \
../test/sigopcount_tests.cpp \
../test/test_bitcoin.cpp \
../test/transaction_tests.cpp \
../test/uint160_tests.cpp \
../test/uint256_tests.cpp \
../test/util_tests.cpp \
../test/wallet_tests.cpp 

OBJS += \
./test/Checkpoints_tests.o \
./test/DoS_tests.o \
./test/accounting_tests.o \
./test/alert_tests.o \
./test/allocator_tests.o \
./test/base32_tests.o \
./test/base58_tests.o \
./test/base64_tests.o \
./test/bignum_tests.o \
./test/bloom_tests.o \
./test/canonical_tests.o \
./test/checkblock_tests.o \
./test/compress_tests.o \
./test/getarg_tests.o \
./test/key_tests.o \
./test/miner_tests.o \
./test/mruset_tests.o \
./test/multisig_tests.o \
./test/netbase_tests.o \
./test/pmt_tests.o \
./test/rpc_tests.o \
./test/script_P2SH_tests.o \
./test/script_tests.o \
./test/serialize_tests.o \
./test/sigopcount_tests.o \
./test/test_bitcoin.o \
./test/transaction_tests.o \
./test/uint160_tests.o \
./test/uint256_tests.o \
./test/util_tests.o \
./test/wallet_tests.o 

CPP_DEPS += \
./test/Checkpoints_tests.d \
./test/DoS_tests.d \
./test/accounting_tests.d \
./test/alert_tests.d \
./test/allocator_tests.d \
./test/base32_tests.d \
./test/base58_tests.d \
./test/base64_tests.d \
./test/bignum_tests.d \
./test/bloom_tests.d \
./test/canonical_tests.d \
./test/checkblock_tests.d \
./test/compress_tests.d \
./test/getarg_tests.d \
./test/key_tests.d \
./test/miner_tests.d \
./test/mruset_tests.d \
./test/multisig_tests.d \
./test/netbase_tests.d \
./test/pmt_tests.d \
./test/rpc_tests.d \
./test/script_P2SH_tests.d \
./test/script_tests.d \
./test/serialize_tests.d \
./test/sigopcount_tests.d \
./test/test_bitcoin.d \
./test/transaction_tests.d \
./test/uint160_tests.d \
./test/uint256_tests.d \
./test/util_tests.d \
./test/wallet_tests.d 


# Each subdirectory must supply rules for building sources it contributes
test/%.o: ../test/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


