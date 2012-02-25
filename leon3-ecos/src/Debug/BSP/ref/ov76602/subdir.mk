################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/ref/ov76602/ov7660.c \
../BSP/ref/ov76602/sccb.c 

OBJS += \
./BSP/ref/ov76602/ov7660.o \
./BSP/ref/ov76602/sccb.o 

C_DEPS += \
./BSP/ref/ov76602/ov7660.d \
./BSP/ref/ov76602/sccb.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/ref/ov76602/%.o: ../BSP/ref/ov76602/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Solaris C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


