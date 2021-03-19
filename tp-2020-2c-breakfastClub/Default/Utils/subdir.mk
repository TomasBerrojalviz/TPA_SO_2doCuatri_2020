################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utils/utils_en_comun.c 

OBJS += \
./Utils/utils_en_comun.o 

C_DEPS += \
./Utils/utils_en_comun.d 


# Each subdirectory must supply rules for building sources it contributes
Utils/%.o: ../Utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


