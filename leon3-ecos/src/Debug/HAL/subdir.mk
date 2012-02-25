################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/apbuart.c \
../HAL/gpio.c \
../HAL/spi.c \
../HAL/tools.c \
../HAL/uart.c 

OBJS += \
./HAL/apbuart.o \
./HAL/gpio.o \
./HAL/spi.o \
./HAL/tools.o \
./HAL/uart.o 

C_DEPS += \
./HAL/apbuart.d \
./HAL/gpio.d \
./HAL/spi.d \
./HAL/tools.d \
./HAL/uart.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/%.o: ../HAL/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/opt/opdk/leon-wsn/includes -I/opt/opdk/ecos-rep-1.0.9/leon3_install/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


