################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/HAL/apbuart.c \
../src/HAL/cyg_spi.c \
../src/HAL/gpio.c \
../src/HAL/serial.c \
../src/HAL/spi.c \
../src/HAL/tools.c 

OBJS += \
./src/HAL/apbuart.o \
./src/HAL/cyg_spi.o \
./src/HAL/gpio.o \
./src/HAL/serial.o \
./src/HAL/spi.o \
./src/HAL/tools.o 

C_DEPS += \
./src/HAL/apbuart.d \
./src/HAL/cyg_spi.d \
./src/HAL/gpio.d \
./src/HAL/serial.d \
./src/HAL/spi.d \
./src/HAL/tools.d 


# Each subdirectory must supply rules for building sources it contributes
src/HAL/%.o: ../src/HAL/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	sparc-elf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


