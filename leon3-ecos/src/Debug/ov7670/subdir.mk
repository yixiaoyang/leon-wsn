################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ov7670/SCCB.c \
../ov7670/main.c \
../ov7670/ov7660.c \
../ov7670/ov7660config.c 

OBJS += \
./ov7670/SCCB.o \
./ov7670/main.o \
./ov7670/ov7660.o \
./ov7670/ov7660config.o 

C_DEPS += \
./ov7670/SCCB.d \
./ov7670/main.d \
./ov7670/ov7660.d \
./ov7670/ov7660config.d 


# Each subdirectory must supply rules for building sources it contributes
ov7670/%.o: ../ov7670/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Solaris C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


