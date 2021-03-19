################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sindicato/src/Sindicato.c 

OBJS += \
./Sindicato/src/Sindicato.o 

C_DEPS += \
./Sindicato/src/Sindicato.d 


# Each subdirectory must supply rules for building sources it contributes
Sindicato/src/%.o: ../Sindicato/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


