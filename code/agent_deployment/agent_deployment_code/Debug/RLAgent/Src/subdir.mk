################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../RLAgent/Src/agent.c 

OBJS += \
./RLAgent/Src/agent.o 

C_DEPS += \
./RLAgent/Src/agent.d 


# Each subdirectory must supply rules for building sources it contributes
RLAgent/Src/%.o RLAgent/Src/%.su RLAgent/Src/%.cyclo: ../RLAgent/Src/%.c RLAgent/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32F103xB -c -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/AgentDeployment/Core/Inc" -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/AgentDeployment/RLAgent/Inc" -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/AgentDeployment/Peripherals/Inc" -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/AgentDeployment/CMSIS/Include" -I"C:/Users/Newton/Desktop/STM32F103C8_PROJECTS/AgentDeployment/MF4005v2/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-RLAgent-2f-Src

clean-RLAgent-2f-Src:
	-$(RM) ./RLAgent/Src/agent.cyclo ./RLAgent/Src/agent.d ./RLAgent/Src/agent.o ./RLAgent/Src/agent.su

.PHONY: clean-RLAgent-2f-Src

