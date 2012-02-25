################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Stack/netif.c \
../Stack/translate.c 

OBJS += \
./Stack/netif.o \
./Stack/translate.o 

C_DEPS += \
./Stack/netif.d \
./Stack/translate.d 


# Each subdirectory must supply rules for building sources it contributes
Stack/%.o: ../Stack/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/opt/opdk/leon-wsn/includes -I/opt/opdk/ecos-rep-1.0.9/leon3_install/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


