################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/FatFs/diskio.c \
../src/FatFs/ff.c \
../src/FatFs/ff_gen_drv.c \
../src/FatFs/ffunicode.c \
../src/FatFs/sd_diskio.c \
../src/FatFs/syscall.c 

OBJS += \
./src/FatFs/diskio.o \
./src/FatFs/ff.o \
./src/FatFs/ff_gen_drv.o \
./src/FatFs/ffunicode.o \
./src/FatFs/sd_diskio.o \
./src/FatFs/syscall.o 

C_DEPS += \
./src/FatFs/diskio.d \
./src/FatFs/ff.d \
./src/FatFs/ff_gen_drv.d \
./src/FatFs/ffunicode.d \
./src/FatFs/sd_diskio.d \
./src/FatFs/syscall.d 


# Each subdirectory must supply rules for building sources it contributes
src/FatFs/%.o: ../src/FatFs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall -Wextra  -g -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=12000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -g -O0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


