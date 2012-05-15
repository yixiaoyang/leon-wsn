################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BSP/FAT32.c \
../src/BSP/VS1003x.c \
../src/BSP/data.c \
../src/BSP/ff.c \
../src/BSP/led.c \
../src/BSP/ov7660.c \
../src/BSP/rf.c \
../src/BSP/sccb.c \
../src/BSP/sd.c \
../src/BSP/uz2400d.c 

OBJS += \
./src/BSP/FAT32.o \
./src/BSP/VS1003x.o \
./src/BSP/data.o \
./src/BSP/ff.o \
./src/BSP/led.o \
./src/BSP/ov7660.o \
./src/BSP/rf.o \
./src/BSP/sccb.o \
./src/BSP/sd.o \
./src/BSP/uz2400d.o 

C_DEPS += \
./src/BSP/FAT32.d \
./src/BSP/VS1003x.d \
./src/BSP/data.d \
./src/BSP/ff.d \
./src/BSP/led.d \
./src/BSP/ov7660.d \
./src/BSP/rf.d \
./src/BSP/sccb.d \
./src/BSP/sd.d \
./src/BSP/uz2400d.d 


# Each subdirectory must supply rules for building sources it contributes
src/BSP/%.o: ../src/BSP/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	sparc-elf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


