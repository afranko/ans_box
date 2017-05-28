################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/FatFs/drivers/sd_diskio.c \
../src/FatFs/drivers/sdram_diskio.c \
../src/FatFs/drivers/sram_diskio.c \
../src/FatFs/drivers/usbh_diskio.c 

OBJS += \
./src/FatFs/drivers/sd_diskio.o \
./src/FatFs/drivers/sdram_diskio.o \
./src/FatFs/drivers/sram_diskio.o \
./src/FatFs/drivers/usbh_diskio.o 

C_DEPS += \
./src/FatFs/drivers/sd_diskio.d \
./src/FatFs/drivers/sdram_diskio.d \
./src/FatFs/drivers/sram_diskio.d \
./src/FatFs/drivers/usbh_diskio.d 


# Each subdirectory must supply rules for building sources it contributes
src/FatFs/drivers/%.o: ../src/FatFs/drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


