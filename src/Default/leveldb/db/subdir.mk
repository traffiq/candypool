################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../leveldb/db/builder.o \
../leveldb/db/c.o \
../leveldb/db/db_impl.o \
../leveldb/db/db_iter.o \
../leveldb/db/dbformat.o \
../leveldb/db/filename.o \
../leveldb/db/log_reader.o \
../leveldb/db/log_writer.o \
../leveldb/db/memtable.o \
../leveldb/db/repair.o \
../leveldb/db/table_cache.o \
../leveldb/db/version_edit.o \
../leveldb/db/version_set.o \
../leveldb/db/write_batch.o 

C_SRCS += \
../leveldb/db/c_test.c 

CC_SRCS += \
../leveldb/db/builder.cc \
../leveldb/db/c.cc \
../leveldb/db/corruption_test.cc \
../leveldb/db/db_bench.cc \
../leveldb/db/db_impl.cc \
../leveldb/db/db_iter.cc \
../leveldb/db/db_test.cc \
../leveldb/db/dbformat.cc \
../leveldb/db/dbformat_test.cc \
../leveldb/db/filename.cc \
../leveldb/db/filename_test.cc \
../leveldb/db/leveldb_main.cc \
../leveldb/db/log_reader.cc \
../leveldb/db/log_test.cc \
../leveldb/db/log_writer.cc \
../leveldb/db/memtable.cc \
../leveldb/db/repair.cc \
../leveldb/db/skiplist_test.cc \
../leveldb/db/table_cache.cc \
../leveldb/db/version_edit.cc \
../leveldb/db/version_edit_test.cc \
../leveldb/db/version_set.cc \
../leveldb/db/version_set_test.cc \
../leveldb/db/write_batch.cc \
../leveldb/db/write_batch_test.cc 

OBJS += \
./leveldb/db/builder.o \
./leveldb/db/c.o \
./leveldb/db/c_test.o \
./leveldb/db/corruption_test.o \
./leveldb/db/db_bench.o \
./leveldb/db/db_impl.o \
./leveldb/db/db_iter.o \
./leveldb/db/db_test.o \
./leveldb/db/dbformat.o \
./leveldb/db/dbformat_test.o \
./leveldb/db/filename.o \
./leveldb/db/filename_test.o \
./leveldb/db/leveldb_main.o \
./leveldb/db/log_reader.o \
./leveldb/db/log_test.o \
./leveldb/db/log_writer.o \
./leveldb/db/memtable.o \
./leveldb/db/repair.o \
./leveldb/db/skiplist_test.o \
./leveldb/db/table_cache.o \
./leveldb/db/version_edit.o \
./leveldb/db/version_edit_test.o \
./leveldb/db/version_set.o \
./leveldb/db/version_set_test.o \
./leveldb/db/write_batch.o \
./leveldb/db/write_batch_test.o 

C_DEPS += \
./leveldb/db/c_test.d 

CC_DEPS += \
./leveldb/db/builder.d \
./leveldb/db/c.d \
./leveldb/db/corruption_test.d \
./leveldb/db/db_bench.d \
./leveldb/db/db_impl.d \
./leveldb/db/db_iter.d \
./leveldb/db/db_test.d \
./leveldb/db/dbformat.d \
./leveldb/db/dbformat_test.d \
./leveldb/db/filename.d \
./leveldb/db/filename_test.d \
./leveldb/db/leveldb_main.d \
./leveldb/db/log_reader.d \
./leveldb/db/log_test.d \
./leveldb/db/log_writer.d \
./leveldb/db/memtable.d \
./leveldb/db/repair.d \
./leveldb/db/skiplist_test.d \
./leveldb/db/table_cache.d \
./leveldb/db/version_edit.d \
./leveldb/db/version_edit_test.d \
./leveldb/db/version_set.d \
./leveldb/db/version_set_test.d \
./leveldb/db/write_batch.d \
./leveldb/db/write_batch_test.d 


# Each subdirectory must supply rules for building sources it contributes
leveldb/db/%.o: ../leveldb/db/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

leveldb/db/%.o: ../leveldb/db/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/cf/primecoin-0.1.2-hp11/src" -O2 -std=gnu99 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


