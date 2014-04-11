################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../leveldb/table/block.o \
../leveldb/table/block_builder.o \
../leveldb/table/filter_block.o \
../leveldb/table/format.o \
../leveldb/table/iterator.o \
../leveldb/table/merger.o \
../leveldb/table/table.o \
../leveldb/table/table_builder.o \
../leveldb/table/two_level_iterator.o 

CC_SRCS += \
../leveldb/table/block.cc \
../leveldb/table/block_builder.cc \
../leveldb/table/filter_block.cc \
../leveldb/table/filter_block_test.cc \
../leveldb/table/format.cc \
../leveldb/table/iterator.cc \
../leveldb/table/merger.cc \
../leveldb/table/table.cc \
../leveldb/table/table_builder.cc \
../leveldb/table/table_test.cc \
../leveldb/table/two_level_iterator.cc 

OBJS += \
./leveldb/table/block.o \
./leveldb/table/block_builder.o \
./leveldb/table/filter_block.o \
./leveldb/table/filter_block_test.o \
./leveldb/table/format.o \
./leveldb/table/iterator.o \
./leveldb/table/merger.o \
./leveldb/table/table.o \
./leveldb/table/table_builder.o \
./leveldb/table/table_test.o \
./leveldb/table/two_level_iterator.o 

CC_DEPS += \
./leveldb/table/block.d \
./leveldb/table/block_builder.d \
./leveldb/table/filter_block.d \
./leveldb/table/filter_block_test.d \
./leveldb/table/format.d \
./leveldb/table/iterator.d \
./leveldb/table/merger.d \
./leveldb/table/table.d \
./leveldb/table/table_builder.d \
./leveldb/table/table_test.d \
./leveldb/table/two_level_iterator.d 


# Each subdirectory must supply rules for building sources it contributes
leveldb/table/%.o: ../leveldb/table/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


