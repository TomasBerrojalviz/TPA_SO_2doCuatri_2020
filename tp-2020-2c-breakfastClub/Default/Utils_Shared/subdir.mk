################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utils_Shared/utils_en_comun.c 

OBJS += \
./Utils_Shared/utils_en_comun.o 

C_DEPS += \
./Utils_Shared/utils_en_comun.d 


# Each subdirectory must supply rules for building sources it contributes
Utils_Shared/%.o: ../Utils_Shared/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2020-2c-breakfastClub/Utils_Shared" -O2 -g -Wall -c -fmessage-length=0 -fPIC -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


