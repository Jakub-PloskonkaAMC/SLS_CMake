# Lista plików źródłowych dla biblioteki src_lib
set(SRC_FILES
    main.c
    debug.c
    stm32f4xx_hal_msp.c
    stm32f4xx_it.c
    syscalls.c
    sysmem.c
    system_stm32f4xx.c
    App/ModbusMath/ModbusMath_MaxMin.c
    App/ModbusMath/ModbusMath.c
    App/acquisition_hf.c
    App/acquisition_lf.c
    App/comm_app.c
    App/comm_link.c
    App/crc16.c
    App/estimates_calc.c
     ../Inc
    ../Drivers/STM32F4xx_HAL_Driver/Inc
    ../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ../Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ../Drivers/CMSIS/Include

)

# Statyczna biblioteka z plikow zrodlowych podanych w zmiennej SRC_FILES
add_library(src_lib ${SRC_FILES})

# Dodanie katalogu z plikami naglowkowymi do sciezek kompilatora
target_include_directories(src_lib PUBLIC
    ../Inc
    ../Drivers/STM32F4xx_HAL_Driver/Inc
    ../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ../Drivers/CMSIS/Include
    ../Drivers/CMSIS/Device/ST/STM32F4xx/Include
)