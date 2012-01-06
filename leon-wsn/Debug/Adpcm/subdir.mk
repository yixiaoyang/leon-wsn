################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Adpcm/adpcm.c 

OBJS += \
./Adpcm/adpcm.o 

C_DEPS += \
./Adpcm/adpcm.d 


# Each subdirectory must supply rules for building sources it contributes
Adpcm/%.o: ../Adpcm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/opt/opdk/leon-wsn/includes -I/opt/opdk/ecos-rep-1.0.9/leon3_install/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


