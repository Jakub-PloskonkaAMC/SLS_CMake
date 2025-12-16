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
	arm-none-eabi-gcc -c -u _printf_float "$<" -mcpu=cortex-m4 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' -DARM_MATH_CM4 '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F413xx -c -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Drivers/CMSIS/DSP/Include" -I../Src/App/ModbusMath -I../Drivers/CMSIS/DSP/Include -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Src/App/ModbusMath" -I../Src/App -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Src/Modbus" -I../Src/OTC -I../Src/Memory -I../Inc -I../Src/Drivers -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Lib/GCC -I../../Src/App/ModbusMath -I../../Drivers/CMSIS/DSP/Include -Og -ffunction-sections -Wall -fmessage-length=0 -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-App-2f-ModbusMath

clean-Src-2f-App-2f-ModbusMath:
	-$(RM) ./Src/App/ModbusMath/ModbusMath.cyclo ./Src/App/ModbusMath/ModbusMath.d ./Src/App/ModbusMath/ModbusMath.o ./Src/App/ModbusMath/ModbusMath.su ./Src/App/ModbusMath/ModbusMath_MaxMin.cyclo ./Src/App/ModbusMath/ModbusMath_MaxMin.d ./Src/App/ModbusMath/ModbusMath_MaxMin.o ./Src/App/ModbusMath/ModbusMath_MaxMin.su

.PHONY: clean-Src-2f-App-2f-ModbusMath

