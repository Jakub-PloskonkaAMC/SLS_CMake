################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/OTC/crc32.c \
../Src/OTC/otc_core.c \
../Src/OTC/otc_flash.c 

OBJS += \
./Src/OTC/crc32.o \
./Src/OTC/otc_core.o \
./Src/OTC/otc_flash.o 

C_DEPS += \
./Src/OTC/crc32.d \
./Src/OTC/otc_core.d \
./Src/OTC/otc_flash.d 


# Each subdirectory must supply rules for building sources it contributes
Src/OTC/%.o Src/OTC/%.su Src/OTC/%.cyclo: ../Src/OTC/%.c Src/OTC/subdir.mk
	arm-none-eabi-gcc -c -u _printf_float "$<" -mcpu=cortex-m4 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' -DARM_MATH_CM4 '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F413xx -c -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Drivers/CMSIS/DSP/Include" -I../Src/App/ModbusMath -I../Drivers/CMSIS/DSP/Include -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Src/App/ModbusMath" -I../Src/App -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Src/Modbus" -I../Src/OTC -I../Src/Memory -I../Inc -I../Src/Drivers -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Lib/GCC -I../../Src/App/ModbusMath -I../../Drivers/CMSIS/DSP/Include -Og -ffunction-sections -Wall -fmessage-length=0 -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-OTC

clean-Src-2f-OTC:
	-$(RM) ./Src/OTC/crc32.cyclo ./Src/OTC/crc32.d ./Src/OTC/crc32.o ./Src/OTC/crc32.su ./Src/OTC/otc_core.cyclo ./Src/OTC/otc_core.d ./Src/OTC/otc_core.o ./Src/OTC/otc_core.su ./Src/OTC/otc_flash.cyclo ./Src/OTC/otc_flash.d ./Src/OTC/otc_flash.o ./Src/OTC/otc_flash.su

.PHONY: clean-Src-2f-OTC

