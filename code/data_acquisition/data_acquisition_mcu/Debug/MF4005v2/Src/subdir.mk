################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MF4005v2/Src/MF4005v2.c 

OBJS += \
./MF4005v2/Src/MF4005v2.o 

C_DEPS += \
./MF4005v2/Src/MF4005v2.d 


# Each subdirectory must supply rules for building sources it contributes
MF4005v2/Src/%.o MF4005v2/Src/%.su MF4005v2/Src/%.cyclo: ../MF4005v2/Src/%.c MF4005v2/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32F103xB -c -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/DataCollection/Core/Inc" -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/DataCollection/Peripherals/Inc" -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/DataCollection/CMSIS/Include" -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/DataCollection/MF4005v2/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-MF4005v2-2f-Src

clean-MF4005v2-2f-Src:
	-$(RM) ./MF4005v2/Src/MF4005v2.cyclo ./MF4005v2/Src/MF4005v2.d ./MF4005v2/Src/MF4005v2.o ./MF4005v2/Src/MF4005v2.su

.PHONY: clean-MF4005v2-2f-Src

