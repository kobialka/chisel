################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main_acc_start_v1.c \
../src/stm32f4xx_hal_msp.c \
../src/stm32f4xx_it.c 

OBJS += \
./src/main_acc_start_v1.o \
./src/stm32f4xx_hal_msp.o \
./src/stm32f4xx_it.o 

C_DEPS += \
./src/main_acc_start_v1.d \
./src/stm32f4xx_hal_msp.d \
./src/stm32f4xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../inc" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


