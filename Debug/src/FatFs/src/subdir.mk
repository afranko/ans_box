################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/FatFs/src/diskio.c \
../src/FatFs/src/ff.c \
../src/FatFs/src/ff_gen_drv.c 

OBJS += \
./src/FatFs/src/diskio.o \
./src/FatFs/src/ff.o \
./src/FatFs/src/ff_gen_drv.o 

C_DEPS += \
./src/FatFs/src/diskio.d \
./src/FatFs/src/ff.d \
./src/FatFs/src/ff_gen_drv.d 


# Each subdirectory must supply rules for building sources it contributes
src/FatFs/src/%.o: ../src/FatFs/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


