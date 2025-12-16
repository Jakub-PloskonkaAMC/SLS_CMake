################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s.c \
../Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_data.c \
../Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_initialize.c \
../Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_terminate.c \
../Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s.c \
../Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_data.c \
../Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_initialize.c \
../Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_terminate.c \
../Src/Estimates_Lib/MW_CMSIS_fft.c \
../Src/Estimates_Lib/minOrMax.c \
../Src/Estimates_Lib/rtGetInf.c \
../Src/Estimates_Lib/rtGetNaN.c \
../Src/Estimates_Lib/rt_nonfinite.c \
../Src/Estimates_Lib/sort.c \
../Src/Estimates_Lib/sortIdx.c 

OBJS += \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s.o \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_data.o \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_initialize.o \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_terminate.o \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s.o \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_data.o \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_initialize.o \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_terminate.o \
./Src/Estimates_Lib/MW_CMSIS_fft.o \
./Src/Estimates_Lib/minOrMax.o \
./Src/Estimates_Lib/rtGetInf.o \
./Src/Estimates_Lib/rtGetNaN.o \
./Src/Estimates_Lib/rt_nonfinite.o \
./Src/Estimates_Lib/sort.o \
./Src/Estimates_Lib/sortIdx.o 

C_DEPS += \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s.d \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_data.d \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_initialize.d \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_terminate.d \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s.d \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_data.d \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_initialize.d \
./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_terminate.d \
./Src/Estimates_Lib/MW_CMSIS_fft.d \
./Src/Estimates_Lib/minOrMax.d \
./Src/Estimates_Lib/rtGetInf.d \
./Src/Estimates_Lib/rtGetNaN.d \
./Src/Estimates_Lib/rt_nonfinite.d \
./Src/Estimates_Lib/sort.d \
./Src/Estimates_Lib/sortIdx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/Estimates_Lib/%.o Src/Estimates_Lib/%.su Src/Estimates_Lib/%.cyclo: ../Src/Estimates_Lib/%.c Src/Estimates_Lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F413xx -c -I../Src/Modbus -I../Src/App -I../Src/OTC -I../Src/Memory -I../Src/Drivers -I../Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Src/App/ModbusMath -I../Drivers/CMSIS/DSP/Include -Ofast -ffunction-sections -Wall -fmessage-length=0 -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-Estimates_Lib

clean-Src-2f-Estimates_Lib:
	-$(RM) ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s.cyclo ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s.d ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s.o ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s.su ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_data.cyclo ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_data.d ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_data.o ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_data.su ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_initialize.cyclo ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_initialize.d ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_initialize.o ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_initialize.su ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_terminate.cyclo ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_terminate.d ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_terminate.o ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s_terminate.su ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s.cyclo ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s.d ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s.o ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s.su ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_data.cyclo ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_data.d ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_data.o ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_data.su ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_initialize.cyclo ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_initialize.d ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_initialize.o ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_initialize.su ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_terminate.cyclo ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_terminate.d ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_terminate.o ./Src/Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s_terminate.su ./Src/Estimates_Lib/MW_CMSIS_fft.cyclo ./Src/Estimates_Lib/MW_CMSIS_fft.d ./Src/Estimates_Lib/MW_CMSIS_fft.o ./Src/Estimates_Lib/MW_CMSIS_fft.su ./Src/Estimates_Lib/minOrMax.cyclo ./Src/Estimates_Lib/minOrMax.d ./Src/Estimates_Lib/minOrMax.o ./Src/Estimates_Lib/minOrMax.su ./Src/Estimates_Lib/rtGetInf.cyclo ./Src/Estimates_Lib/rtGetInf.d ./Src/Estimates_Lib/rtGetInf.o ./Src/Estimates_Lib/rtGetInf.su ./Src/Estimates_Lib/rtGetNaN.cyclo ./Src/Estimates_Lib/rtGetNaN.d ./Src/Estimates_Lib/rtGetNaN.o ./Src/Estimates_Lib/rtGetNaN.su ./Src/Estimates_Lib/rt_nonfinite.cyclo ./Src/Estimates_Lib/rt_nonfinite.d ./Src/Estimates_Lib/rt_nonfinite.o ./Src/Estimates_Lib/rt_nonfinite.su ./Src/Estimates_Lib/sort.cyclo ./Src/Estimates_Lib/sort.d ./Src/Estimates_Lib/sort.o ./Src/Estimates_Lib/sort.su ./Src/Estimates_Lib/sortIdx.cyclo ./Src/Estimates_Lib/sortIdx.d ./Src/Estimates_Lib/sortIdx.o ./Src/Estimates_Lib/sortIdx.su

.PHONY: clean-Src-2f-Estimates_Lib

