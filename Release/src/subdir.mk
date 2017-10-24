################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/command_decoder.c \
../src/main_zadajnik.c \
../src/stm32f4xx_hal_msp.c \
../src/stm32f4xx_it.c \
../src/string.c \
../src/uart.c 

OBJS += \
./src/command_decoder.o \
./src/main_zadajnik.o \
./src/stm32f4xx_hal_msp.o \
./src/stm32f4xx_it.o \
./src/string.o \
./src/uart.o 

C_DEPS += \
./src/command_decoder.d \
./src/main_zadajnik.d \
./src/stm32f4xx_hal_msp.d \
./src/stm32f4xx_it.d \
./src/string.d \
./src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../inc" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I../inc/lcd -I../inc/bsp -I"../inc/extsensor" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


