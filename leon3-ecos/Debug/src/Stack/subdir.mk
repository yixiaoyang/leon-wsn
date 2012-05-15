################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Stack/netif.c \
../src/Stack/translate.c 

OBJS += \
./src/Stack/netif.o \
./src/Stack/translate.o 

C_DEPS += \
./src/Stack/netif.d \
./src/Stack/translate.d 


# Each subdirectory must supply rules for building sources it contributes
src/Stack/%.o: ../src/Stack/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	sparc-elf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


