################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../qt/test/test_main.cpp \
../qt/test/uritests.cpp 

OBJS += \
./qt/test/test_main.o \
./qt/test/uritests.o 

CPP_DEPS += \
./qt/test/test_main.d \
./qt/test/uritests.d 


# Each subdirectory must supply rules for building sources it contributes
qt/test/%.o: ../qt/test/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


