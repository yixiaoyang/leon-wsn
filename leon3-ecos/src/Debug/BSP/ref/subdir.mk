################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/ref/FAT32.c \
../BSP/ref/sd.c 

OBJS += \
./BSP/ref/FAT32.o \
./BSP/ref/sd.o 

C_DEPS += \
./BSP/ref/FAT32.d \
./BSP/ref/sd.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/ref/%.o: ../BSP/ref/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Solaris C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


