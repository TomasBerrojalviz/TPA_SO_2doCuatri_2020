################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Cliente/src/Cliente.c 

OBJS += \
./Cliente/src/Cliente.o 

C_DEPS += \
./Cliente/src/Cliente.d 


# Each subdirectory must supply rules for building sources it contributes
Cliente/src/%.o: ../Cliente/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


