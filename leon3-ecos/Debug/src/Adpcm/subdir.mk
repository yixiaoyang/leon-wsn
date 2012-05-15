################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Adpcm/adpcm.c 

OBJS += \
./src/Adpcm/adpcm.o 

C_DEPS += \
./src/Adpcm/adpcm.d 


# Each subdirectory must supply rules for building sources it contributes
src/Adpcm/%.o: ../src/Adpcm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	sparc-elf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


