################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../leveldb/doc/bench/db_bench_sqlite3.cc \
../leveldb/doc/bench/db_bench_tree_db.cc 

OBJS += \
./leveldb/doc/bench/db_bench_sqlite3.o \
./leveldb/doc/bench/db_bench_tree_db.o 

CC_DEPS += \
./leveldb/doc/bench/db_bench_sqlite3.d \
./leveldb/doc/bench/db_bench_tree_db.d 


# Each subdirectory must supply rules for building sources it contributes
leveldb/doc/bench/%.o: ../leveldb/doc/bench/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


