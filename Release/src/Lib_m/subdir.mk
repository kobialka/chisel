################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Lib_m/lib_FONT_Table_m.c \
../src/Lib_m/lib_S1D15705_m.c \
../src/Lib_m/lib_STRINGS_m.c 

OBJS += \
./src/Lib_m/lib_FONT_Table_m.o \
./src/Lib_m/lib_S1D15705_m.o \
./src/Lib_m/lib_STRINGS_m.o 

C_DEPS += \
./src/Lib_m/lib_FONT_Table_m.d \
./src/Lib_m/lib_S1D15705_m.d \
./src/Lib_m/lib_STRINGS_m.d 


# Each subdirectory must supply rules for building sources it contributes
src/Lib_m/%.o: ../src/Lib_m/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


