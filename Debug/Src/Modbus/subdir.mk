################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/Modbus/ModbusRTU.c \
../Src/Modbus/ModbusRTU_FunctionCallbacks.c \
../Src/Modbus/ModbusRTU_crc.c 

OBJS += \
./Src/Modbus/ModbusRTU.o \
./Src/Modbus/ModbusRTU_FunctionCallbacks.o \
./Src/Modbus/ModbusRTU_crc.o 

C_DEPS += \
./Src/Modbus/ModbusRTU.d \
./Src/Modbus/ModbusRTU_FunctionCallbacks.d \
./Src/Modbus/ModbusRTU_crc.d 


# Each subdirectory must supply rules for building sources it contributes
Src/Modbus/%.o Src/Modbus/%.su Src/Modbus/%.cyclo: ../Src/Modbus/%.c Src/Modbus/subdir.mk
	arm-none-eabi-gcc -c -u _printf_float "$<" -mcpu=cortex-m4 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' -DARM_MATH_CM4 '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F413xx -c -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Drivers/CMSIS/DSP/Include" -I../Src/App/ModbusMath -I../Drivers/CMSIS/DSP/Include -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Src/App/ModbusMath" -I../Src/App -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Src/Modbus" -I../Src/OTC -I../Src/Memory -I../Inc -I../Src/Drivers -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Lib/GCC -I../../Src/App/ModbusMath -I../../Drivers/CMSIS/DSP/Include -Og -ffunction-sections -Wall -fmessage-length=0 -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-Modbus

clean-Src-2f-Modbus:
	-$(RM) ./Src/Modbus/ModbusRTU.cyclo ./Src/Modbus/ModbusRTU.d ./Src/Modbus/ModbusRTU.o ./Src/Modbus/ModbusRTU.su ./Src/Modbus/ModbusRTU_FunctionCallbacks.cyclo ./Src/Modbus/ModbusRTU_FunctionCallbacks.d ./Src/Modbus/ModbusRTU_FunctionCallbacks.o ./Src/Modbus/ModbusRTU_FunctionCallbacks.su ./Src/Modbus/ModbusRTU_crc.cyclo ./Src/Modbus/ModbusRTU_crc.d ./Src/Modbus/ModbusRTU_crc.o ./Src/Modbus/ModbusRTU_crc.su

.PHONY: clean-Src-2f-Modbus

