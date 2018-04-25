################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bsp_driver_sd.c \
../src/circbuff.c \
../src/comm.c \
../src/fatfs.c \
../src/init.c \
../src/main.c \
../src/measurement.c \
../src/message.c \
../src/parson.c \
../src/statemachine.c \
../src/stm32f4xx_hal_msp.c \
../src/stm32f4xx_it.c \
../src/storage.c 

OBJS += \
./src/bsp_driver_sd.o \
./src/circbuff.o \
./src/comm.o \
./src/fatfs.o \
./src/init.o \
./src/main.o \
./src/measurement.o \
./src/message.o \
./src/parson.o \
./src/statemachine.o \
./src/stm32f4xx_hal_msp.o \
./src/stm32f4xx_it.o \
./src/storage.o 

C_DEPS += \
./src/bsp_driver_sd.d \
./src/circbuff.d \
./src/comm.d \
./src/fatfs.d \
./src/init.d \
./src/main.d \
./src/measurement.d \
./src/message.d \
./src/parson.d \
./src/statemachine.d \
./src/stm32f4xx_hal_msp.d \
./src/stm32f4xx_it.d \
./src/storage.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fsingle-precision-constant -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=12000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -g -O0 -Wmissing-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/stm32f4xx_hal_msp.o: ../src/stm32f4xx_hal_msp.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fsingle-precision-constant -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=12000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -g -O0 -Wmissing-prototypes -Wbad-function-cast -Wno-missing-prototypes -Wno-missing-declarations -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/stm32f4xx_hal_msp.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


