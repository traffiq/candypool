################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../leveldb/helpers/memenv/memenv.o 

CC_SRCS += \
../leveldb/helpers/memenv/memenv.cc \
../leveldb/helpers/memenv/memenv_test.cc 

OBJS += \
./leveldb/helpers/memenv/memenv.o \
./leveldb/helpers/memenv/memenv_test.o 

CC_DEPS += \
./leveldb/helpers/memenv/memenv.d \
./leveldb/helpers/memenv/memenv_test.d 


# Each subdirectory must supply rules for building sources it contributes
leveldb/helpers/memenv/%.o: ../leveldb/helpers/memenv/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


