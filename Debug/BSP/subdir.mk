################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/FAT32.c \
../BSP/VS1003x.c \
../BSP/data.c \
../BSP/ff.c \
../BSP/led.c \
../BSP/ov7660.c \
../BSP/rf.c \
../BSP/sccb.c \
../BSP/sd.c \
../BSP/uz2400d.c 

OBJS += \
./BSP/FAT32.o \
./BSP/VS1003x.o \
./BSP/data.o \
./BSP/ff.o \
./BSP/led.o \
./BSP/ov7660.o \
./BSP/rf.o \
./BSP/sccb.o \
./BSP/sd.o \
./BSP/uz2400d.o 

C_DEPS += \
./BSP/FAT32.d \
./BSP/VS1003x.d \
./BSP/data.d \
./BSP/ff.d \
./BSP/led.d \
./BSP/ov7660.d \
./BSP/rf.d \
./BSP/sccb.d \
./BSP/sd.d \
./BSP/uz2400d.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/%.o: ../BSP/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/opt/opdk/leon-wsn/includes -I/opt/opdk/ecos-rep-1.0.9/leon3_install/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


