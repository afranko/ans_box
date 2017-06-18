################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/edge_comm/GSM_MQTT.c \
../src/edge_comm/edge_comm.c \
../src/edge_comm/parson.c 

OBJS += \
./src/edge_comm/GSM_MQTT.o \
./src/edge_comm/edge_comm.o \
./src/edge_comm/parson.o 

C_DEPS += \
./src/edge_comm/GSM_MQTT.d \
./src/edge_comm/edge_comm.d \
./src/edge_comm/parson.d 


# Each subdirectory must supply rules for building sources it contributes
src/edge_comm/%.o: ../src/edge_comm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=12000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


