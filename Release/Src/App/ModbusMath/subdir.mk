################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/App/ModbusMath/ModbusMath.c \
../Src/App/ModbusMath/ModbusMath_MaxMin.c 

OBJS += \
./Src/App/ModbusMath/ModbusMath.o \
./Src/App/ModbusMath/ModbusMath_MaxMin.o 

C_DEPS += \
./Src/App/ModbusMath/ModbusMath.d \
./Src/App/ModbusMath/ModbusMath_MaxMin.d 


# Each subdirectory must supply rules for building sources it contributes
Src/App/ModbusMath/%.o Src/App/ModbusMath/%.su Src/App/ModbusMath/%.cyclo: ../Src/App/ModbusMath/%.c Src/App/ModbusMath/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F413xx -c -I../Src/Modbus -I../Src/App -I../Src/OTC -I../Src/Memory -I../Src/Drivers -I../Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Src/App/ModbusMath -I../Drivers/CMSIS/DSP/Include -Ofast -ffunction-sections -Wall -fmessage-length=0 -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-App-2f-ModbusMath

clean-Src-2f-App-2f-ModbusMath:
	-$(RM) ./Src/App/ModbusMath/ModbusMath.cyclo ./Src/App/ModbusMath/ModbusMath.d ./Src/App/ModbusMath/ModbusMath.o ./Src/App/ModbusMath/ModbusMath.su ./Src/App/ModbusMath/ModbusMath_MaxMin.cyclo ./Src/App/ModbusMath/ModbusMath_MaxMin.d ./Src/App/ModbusMath/ModbusMath_MaxMin.o ./Src/App/ModbusMath/ModbusMath_MaxMin.su

.PHONY: clean-Src-2f-App-2f-ModbusMath

