################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../leveldb/util/arena.o \
../leveldb/util/bloom.o \
../leveldb/util/cache.o \
../leveldb/util/coding.o \
../leveldb/util/comparator.o \
../leveldb/util/crc32c.o \
../leveldb/util/env.o \
../leveldb/util/env_posix.o \
../leveldb/util/env_win.o \
../leveldb/util/filter_policy.o \
../leveldb/util/hash.o \
../leveldb/util/histogram.o \
../leveldb/util/logging.o \
../leveldb/util/options.o \
../leveldb/util/status.o 

CC_SRCS += \
../leveldb/util/arena.cc \
../leveldb/util/arena_test.cc \
../leveldb/util/bloom.cc \
../leveldb/util/bloom_test.cc \
../leveldb/util/cache.cc \
../leveldb/util/cache_test.cc \
../leveldb/util/coding.cc \
../leveldb/util/coding_test.cc \
../leveldb/util/comparator.cc \
../leveldb/util/crc32c.cc \
../leveldb/util/crc32c_test.cc \
../leveldb/util/env.cc \
../leveldb/util/env_posix.cc \
../leveldb/util/env_test.cc \
../leveldb/util/env_win.cc \
../leveldb/util/filter_policy.cc \
../leveldb/util/hash.cc \
../leveldb/util/histogram.cc \
../leveldb/util/logging.cc \
../leveldb/util/options.cc \
../leveldb/util/status.cc \
../leveldb/util/testharness.cc \
../leveldb/util/testutil.cc 

OBJS += \
./leveldb/util/arena.o \
./leveldb/util/arena_test.o \
./leveldb/util/bloom.o \
./leveldb/util/bloom_test.o \
./leveldb/util/cache.o \
./leveldb/util/cache_test.o \
./leveldb/util/coding.o \
./leveldb/util/coding_test.o \
./leveldb/util/comparator.o \
./leveldb/util/crc32c.o \
./leveldb/util/crc32c_test.o \
./leveldb/util/env.o \
./leveldb/util/env_posix.o \
./leveldb/util/env_test.o \
./leveldb/util/env_win.o \
./leveldb/util/filter_policy.o \
./leveldb/util/hash.o \
./leveldb/util/histogram.o \
./leveldb/util/logging.o \
./leveldb/util/options.o \
./leveldb/util/status.o \
./leveldb/util/testharness.o \
./leveldb/util/testutil.o 

CC_DEPS += \
./leveldb/util/arena.d \
./leveldb/util/arena_test.d \
./leveldb/util/bloom.d \
./leveldb/util/bloom_test.d \
./leveldb/util/cache.d \
./leveldb/util/cache_test.d \
./leveldb/util/coding.d \
./leveldb/util/coding_test.d \
./leveldb/util/comparator.d \
./leveldb/util/crc32c.d \
./leveldb/util/crc32c_test.d \
./leveldb/util/env.d \
./leveldb/util/env_posix.d \
./leveldb/util/env_test.d \
./leveldb/util/env_win.d \
./leveldb/util/filter_policy.d \
./leveldb/util/hash.d \
./leveldb/util/histogram.d \
./leveldb/util/logging.d \
./leveldb/util/options.d \
./leveldb/util/status.d \
./leveldb/util/testharness.d \
./leveldb/util/testutil.d 


# Each subdirectory must supply rules for building sources it contributes
leveldb/util/%.o: ../leveldb/util/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


