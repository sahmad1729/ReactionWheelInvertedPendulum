################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Peripherals/Src/can.c \
../Peripherals/Src/gpio.c \
../Peripherals/Src/rcc.c \
../Peripherals/Src/timer.c \
../Peripherals/Src/usart.c 

OBJS += \
./Peripherals/Src/can.o \
./Peripherals/Src/gpio.o \
./Peripherals/Src/rcc.o \
./Peripherals/Src/timer.o \
./Peripherals/Src/usart.o 

C_DEPS += \
./Peripherals/Src/can.d \
./Peripherals/Src/gpio.d \
./Peripherals/Src/rcc.d \
./Peripherals/Src/timer.d \
./Peripherals/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Peripherals/Src/%.o Peripherals/Src/%.su Peripherals/Src/%.cyclo: ../Peripherals/Src/%.c Peripherals/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32F103xB -c -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/AgentDeployment/Core/Inc" -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/AgentDeployment/RLAgent/Inc" -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/AgentDeployment/Peripherals/Inc" -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/AgentDeployment/CMSIS/Include" -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/AgentDeployment/MF4005v2/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Peripherals-2f-Src

clean-Peripherals-2f-Src:
	-$(RM) ./Peripherals/Src/can.cyclo ./Peripherals/Src/can.d ./Peripherals/Src/can.o ./Peripherals/Src/can.su ./Peripherals/Src/gpio.cyclo ./Peripherals/Src/gpio.d ./Peripherals/Src/gpio.o ./Peripherals/Src/gpio.su ./Peripherals/Src/rcc.cyclo ./Peripherals/Src/rcc.d ./Peripherals/Src/rcc.o ./Peripherals/Src/rcc.su ./Peripherals/Src/timer.cyclo ./Peripherals/Src/timer.d ./Peripherals/Src/timer.o ./Peripherals/Src/timer.su ./Peripherals/Src/usart.cyclo ./Peripherals/Src/usart.d ./Peripherals/Src/usart.o ./Peripherals/Src/usart.su

.PHONY: clean-Peripherals-2f-Src

