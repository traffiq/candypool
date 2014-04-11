################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../leveldb/port/port_posix.o 

CC_SRCS += \
../leveldb/port/port_posix.cc \
../leveldb/port/port_win.cc 

OBJS += \
./leveldb/port/port_posix.o \
./leveldb/port/port_win.o 

CC_DEPS += \
./leveldb/port/port_posix.d \
./leveldb/port/port_win.d 


# Each subdirectory must supply rules for building sources it contributes
leveldb/port/%.o: ../leveldb/port/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


