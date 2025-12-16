################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/App/acquisition_hf.c \
../Src/App/acquisition_lf.c \
../Src/App/comm_app.c \
../Src/App/comm_link.c \
../Src/App/crc16.c \
../Src/App/estimates_calc.c 

OBJS += \
./Src/App/acquisition_hf.o \
./Src/App/acquisition_lf.o \
./Src/App/comm_app.o \
./Src/App/comm_link.o \
./Src/App/crc16.o \
./Src/App/estimates_calc.o 

C_DEPS += \
./Src/App/acquisition_hf.d \
./Src/App/acquisition_lf.d \
./Src/App/comm_app.d \
./Src/App/comm_link.d \
./Src/App/crc16.d \
./Src/App/estimates_calc.d 


# Each subdirectory must supply rules for building sources it contributes
Src/App/%.o Src/App/%.su Src/App/%.cyclo: ../Src/App/%.c Src/App/subdir.mk
	arm-none-eabi-gcc -c -u _printf_float "$<" -mcpu=cortex-m4 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' -DARM_MATH_CM4 '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F413xx -c -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Drivers/CMSIS/DSP/Include" -I../Src/App/ModbusMath -I../Drivers/CMSIS/DSP/Include -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Src/App/ModbusMath" -I../Src/App -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Src/Modbus" -I../Src/OTC -I../Src/Memory -I../Inc -I../Src/Drivers -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Lib/GCC -I../../Src/App/ModbusMath -I../../Drivers/CMSIS/DSP/Include -Og -ffunction-sections -Wall -fmessage-length=0 -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-App

clean-Src-2f-App:
	-$(RM) ./Src/App/acquisition_hf.cyclo ./Src/App/acquisition_hf.d ./Src/App/acquisition_hf.o ./Src/App/acquisition_hf.su ./Src/App/acquisition_lf.cyclo ./Src/App/acquisition_lf.d ./Src/App/acquisition_lf.o ./Src/App/acquisition_lf.su ./Src/App/comm_app.cyclo ./Src/App/comm_app.d ./Src/App/comm_app.o ./Src/App/comm_app.su ./Src/App/comm_link.cyclo ./Src/App/comm_link.d ./Src/App/comm_link.o ./Src/App/comm_link.su ./Src/App/crc16.cyclo ./Src/App/crc16.d ./Src/App/crc16.o ./Src/App/crc16.su ./Src/App/estimates_calc.cyclo ./Src/App/estimates_calc.d ./Src/App/estimates_calc.o ./Src/App/estimates_calc.su

.PHONY: clean-Src-2f-App

