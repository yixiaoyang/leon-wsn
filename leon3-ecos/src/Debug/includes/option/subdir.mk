################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../includes/option/cc932.c \
../includes/option/cc936.c \
../includes/option/cc949.c \
../includes/option/cc950.c \
../includes/option/ccsbcs.c \
../includes/option/syscall.c 

OBJS += \
./includes/option/cc932.o \
./includes/option/cc936.o \
./includes/option/cc949.o \
./includes/option/cc950.o \
./includes/option/ccsbcs.o \
./includes/option/syscall.o 

C_DEPS += \
./includes/option/cc932.d \
./includes/option/cc936.d \
./includes/option/cc949.d \
./includes/option/cc950.d \
./includes/option/ccsbcs.d \
./includes/option/syscall.d 


# Each subdirectory must supply rules for building sources it contributes
includes/option/%.o: ../includes/option/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/opt/opdk/leon-wsn/includes -I/opt/opdk/ecos-rep-1.0.9/leon3_install/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


