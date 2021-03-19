################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Sindicato.c \
../src/blocks.c \
../src/consola_sindicato.c \
../src/datos_sindicato.c \
../src/utils_Sindicato.c 

OBJS += \
./src/Sindicato.o \
./src/blocks.o \
./src/consola_sindicato.o \
./src/datos_sindicato.o \
./src/utils_Sindicato.o 

C_DEPS += \
./src/Sindicato.d \
./src/blocks.d \
./src/consola_sindicato.d \
./src/datos_sindicato.d \
./src/utils_Sindicato.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2020-2c-breakfastClub/Utils_Shared" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


