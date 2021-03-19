################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Restaurante/src/Restaurante.c 

OBJS += \
./Restaurante/src/Restaurante.o 

C_DEPS += \
./Restaurante/src/Restaurante.d 


# Each subdirectory must supply rules for building sources it contributes
Restaurante/src/%.o: ../Restaurante/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


