################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Application/application.c 

OBJS += \
./src/Application/application.o 

C_DEPS += \
./src/Application/application.d 


# Each subdirectory must supply rules for building sources it contributes
src/Application/%.o: ../src/Application/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	sparc-elf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


