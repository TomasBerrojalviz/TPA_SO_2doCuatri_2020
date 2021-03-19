################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Restaurante.c \
../src/datos_restaurante.c \
../src/utils_Restaurante.c 

OBJS += \
./src/Restaurante.o \
./src/datos_restaurante.o \
./src/utils_Restaurante.o 

C_DEPS += \
./src/Restaurante.d \
./src/datos_restaurante.d \
./src/utils_Restaurante.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


