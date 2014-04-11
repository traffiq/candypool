################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../json/json_spirit_reader.cpp \
../json/json_spirit_value.cpp \
../json/json_spirit_writer.cpp 

OBJS += \
./json/json_spirit_reader.o \
./json/json_spirit_value.o \
./json/json_spirit_writer.o 

CPP_DEPS += \
./json/json_spirit_reader.d \
./json/json_spirit_value.d \
./json/json_spirit_writer.d 


# Each subdirectory must supply rules for building sources it contributes
json/%.o: ../json/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


