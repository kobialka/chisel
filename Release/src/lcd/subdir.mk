################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lcd/lib_FONT_Table_m.c \
../src/lcd/lib_S1D15705_m.c 

OBJS += \
./src/lcd/lib_FONT_Table_m.o \
./src/lcd/lib_S1D15705_m.o 

C_DEPS += \
./src/lcd/lib_FONT_Table_m.d \
./src/lcd/lib_S1D15705_m.d 


# Each subdirectory must supply rules for building sources it contributes
src/lcd/%.o: ../src/lcd/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../inc" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I../inc/lcd -I../inc/bsp -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


