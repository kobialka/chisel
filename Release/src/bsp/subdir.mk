################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bsp/lis3dsh.c \
../src/bsp/stm32f4_discovery.c \
../src/bsp/stm32f4_discovery_accelerometer.c 

OBJS += \
./src/bsp/lis3dsh.o \
./src/bsp/stm32f4_discovery.o \
./src/bsp/stm32f4_discovery_accelerometer.o 

C_DEPS += \
./src/bsp/lis3dsh.d \
./src/bsp/stm32f4_discovery.d \
./src/bsp/stm32f4_discovery_accelerometer.d 


# Each subdirectory must supply rules for building sources it contributes
src/bsp/%.o: ../src/bsp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../inc" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


