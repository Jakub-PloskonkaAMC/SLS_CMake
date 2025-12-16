################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/Memory/flash_handler.c \
../Src/Memory/memory.c \
../Src/Memory/shared_memory.c 

OBJS += \
./Src/Memory/flash_handler.o \
./Src/Memory/memory.o \
./Src/Memory/shared_memory.o 

C_DEPS += \
./Src/Memory/flash_handler.d \
./Src/Memory/memory.d \
./Src/Memory/shared_memory.d 


# Each subdirectory must supply rules for building sources it contributes
Src/Memory/%.o Src/Memory/%.su Src/Memory/%.cyclo: ../Src/Memory/%.c Src/Memory/subdir.mk
	arm-none-eabi-gcc -c -u _printf_float "$<" -mcpu=cortex-m4 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' -DARM_MATH_CM4 '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F413xx -c -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Drivers/CMSIS/DSP/Include" -I../Src/App/ModbusMath -I../Drivers/CMSIS/DSP/Include -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Src/App/ModbusMath" -I../Src/App -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Src/Modbus" -I../Src/OTC -I../Src/Memory -I../Inc -I../Src/Drivers -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Lib/GCC -I../../Src/App/ModbusMath -I../../Drivers/CMSIS/DSP/Include -Og -ffunction-sections -Wall -fmessage-length=0 -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-Memory

clean-Src-2f-Memory:
	-$(RM) ./Src/Memory/flash_handler.cyclo ./Src/Memory/flash_handler.d ./Src/Memory/flash_handler.o ./Src/Memory/flash_handler.su ./Src/Memory/memory.cyclo ./Src/Memory/memory.d ./Src/Memory/memory.o ./Src/Memory/memory.su ./Src/Memory/shared_memory.cyclo ./Src/Memory/shared_memory.d ./Src/Memory/shared_memory.o ./Src/Memory/shared_memory.su

.PHONY: clean-Src-2f-Memory

