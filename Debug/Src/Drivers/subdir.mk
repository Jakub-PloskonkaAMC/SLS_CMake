################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/Drivers/MacAddress.c \
../Src/Drivers/ad7768.c \
../Src/Drivers/adxl.c \
../Src/Drivers/board.c \
../Src/Drivers/gpio_hf.c \
../Src/Drivers/gpio_lf.c \
../Src/Drivers/rs485.c \
../Src/Drivers/spi.c \
../Src/Drivers/temp.c \
../Src/Drivers/timers.c 

OBJS += \
./Src/Drivers/MacAddress.o \
./Src/Drivers/ad7768.o \
./Src/Drivers/adxl.o \
./Src/Drivers/board.o \
./Src/Drivers/gpio_hf.o \
./Src/Drivers/gpio_lf.o \
./Src/Drivers/rs485.o \
./Src/Drivers/spi.o \
./Src/Drivers/temp.o \
./Src/Drivers/timers.o 

C_DEPS += \
./Src/Drivers/MacAddress.d \
./Src/Drivers/ad7768.d \
./Src/Drivers/adxl.d \
./Src/Drivers/board.d \
./Src/Drivers/gpio_hf.d \
./Src/Drivers/gpio_lf.d \
./Src/Drivers/rs485.d \
./Src/Drivers/spi.d \
./Src/Drivers/temp.d \
./Src/Drivers/timers.d 


# Each subdirectory must supply rules for building sources it contributes
Src/Drivers/%.o Src/Drivers/%.su Src/Drivers/%.cyclo: ../Src/Drivers/%.c Src/Drivers/subdir.mk
	arm-none-eabi-gcc -c -u _printf_float "$<" -mcpu=cortex-m4 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' -DARM_MATH_CM4 '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F413xx -c -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Drivers/CMSIS/DSP/Include" -I../Src/App/ModbusMath -I../Drivers/CMSIS/DSP/Include -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Src/App/ModbusMath" -I../Src/App -I"C:/App/ProjectsAMC/T0808-SLS/SLS/Src/Modbus" -I../Src/OTC -I../Src/Memory -I../Inc -I../Src/Drivers -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Lib/GCC -I../../Src/App/ModbusMath -I../../Drivers/CMSIS/DSP/Include -Og -ffunction-sections -Wall -fmessage-length=0 -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-Drivers

clean-Src-2f-Drivers:
	-$(RM) ./Src/Drivers/MacAddress.cyclo ./Src/Drivers/MacAddress.d ./Src/Drivers/MacAddress.o ./Src/Drivers/MacAddress.su ./Src/Drivers/ad7768.cyclo ./Src/Drivers/ad7768.d ./Src/Drivers/ad7768.o ./Src/Drivers/ad7768.su ./Src/Drivers/adxl.cyclo ./Src/Drivers/adxl.d ./Src/Drivers/adxl.o ./Src/Drivers/adxl.su ./Src/Drivers/board.cyclo ./Src/Drivers/board.d ./Src/Drivers/board.o ./Src/Drivers/board.su ./Src/Drivers/gpio_hf.cyclo ./Src/Drivers/gpio_hf.d ./Src/Drivers/gpio_hf.o ./Src/Drivers/gpio_hf.su ./Src/Drivers/gpio_lf.cyclo ./Src/Drivers/gpio_lf.d ./Src/Drivers/gpio_lf.o ./Src/Drivers/gpio_lf.su ./Src/Drivers/rs485.cyclo ./Src/Drivers/rs485.d ./Src/Drivers/rs485.o ./Src/Drivers/rs485.su ./Src/Drivers/spi.cyclo ./Src/Drivers/spi.d ./Src/Drivers/spi.o ./Src/Drivers/spi.su ./Src/Drivers/temp.cyclo ./Src/Drivers/temp.d ./Src/Drivers/temp.o ./Src/Drivers/temp.su ./Src/Drivers/timers.cyclo ./Src/Drivers/timers.d ./Src/Drivers/timers.o ./Src/Drivers/timers.su

.PHONY: clean-Src-2f-Drivers

