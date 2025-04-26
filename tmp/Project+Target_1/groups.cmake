# groups.cmake

# group Start
add_library(Group_Start OBJECT
  "${SOLUTION_ROOT}/Start/startup_stm32f10x_hd.s"
  "${SOLUTION_ROOT}/Start/system_stm32f10x.c"
)
target_include_directories(Group_Start PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  ${SOLUTION_ROOT}/Start
)
target_compile_definitions(Group_Start PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Start_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Start_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_Start PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_Start PUBLIC
  Group_Start_ABSTRACTIONS
)
set(COMPILE_DEFINITIONS
  __MICROLIB
  STM32F10X_HD
  _RTE_
)
cbuild_set_defines(AS_ARM COMPILE_DEFINITIONS)
set_source_files_properties("${SOLUTION_ROOT}/Start/startup_stm32f10x_hd.s" PROPERTIES
  COMPILE_FLAGS "${COMPILE_DEFINITIONS}"
)
set_source_files_properties("${SOLUTION_ROOT}/Start/startup_stm32f10x_hd.s" PROPERTIES
  COMPILE_OPTIONS "-masm=auto;-x assembler"
)

# group Library
add_library(Group_Library OBJECT
  "${SOLUTION_ROOT}/Library/misc.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_adc.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_bkp.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_can.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_cec.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_crc.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_dac.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_dbgmcu.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_dma.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_exti.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_flash.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_fsmc.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_gpio.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_i2c.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_iwdg.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_pwr.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_rcc.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_rtc.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_sdio.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_spi.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_tim.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_usart.c"
  "${SOLUTION_ROOT}/Library/stm32f10x_wwdg.c"
)
target_include_directories(Group_Library PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  ${SOLUTION_ROOT}/Library
)
target_compile_definitions(Group_Library PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Library_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Library_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_Library PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_Library PUBLIC
  Group_Library_ABSTRACTIONS
)

# group Hardware
add_library(Group_Hardware OBJECT
  "${SOLUTION_ROOT}/Hardware/MyRTC.c"
  "${SOLUTION_ROOT}/Hardware/W25Q64Flash.c"
  "${SOLUTION_ROOT}/Hardware/TIM.c"
  "${SOLUTION_ROOT}/Hardware/iwdog.c"
  "${SOLUTION_ROOT}/Hardware/uart.c"
  "${SOLUTION_ROOT}/Hardware/spi.c"
  "${SOLUTION_ROOT}/Hardware/GPIO.c"
)
target_include_directories(Group_Hardware PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  ${SOLUTION_ROOT}/Hardware
)
target_compile_definitions(Group_Hardware PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Hardware_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Hardware_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_Hardware PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_Hardware PUBLIC
  Group_Hardware_ABSTRACTIONS
)

# group User
add_library(Group_User OBJECT
  "${SOLUTION_ROOT}/User/main.c"
  "${SOLUTION_ROOT}/User/stm32f10x_it.c"
  "${SOLUTION_ROOT}/User/user_DB.c"
)
target_include_directories(Group_User PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  ${SOLUTION_ROOT}/User
)
target_compile_definitions(Group_User PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_User_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_User_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_User PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_User PUBLIC
  Group_User_ABSTRACTIONS
)

# group RTT
add_library(Group_RTT OBJECT
  "${SOLUTION_ROOT}/RTT/SEGGER_RTT.c"
  "${SOLUTION_ROOT}/RTT/SEGGER_RTT_printf.c"
)
target_include_directories(Group_RTT PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  ${SOLUTION_ROOT}/RTT
)
target_compile_definitions(Group_RTT PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_RTT_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_RTT_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_RTT PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_RTT PUBLIC
  Group_RTT_ABSTRACTIONS
)

# group FlashDB
add_library(Group_FlashDB OBJECT
  "${SOLUTION_ROOT}/Component/fal/src/fal.c"
  "${SOLUTION_ROOT}/Component/fal/src/fal_flash.c"
  "${SOLUTION_ROOT}/Component/fal/src/fal_partition.c"
  "${SOLUTION_ROOT}/Component/fal/src/fal_rtt.c"
  "${SOLUTION_ROOT}/Component/fal/samples/porting/fal_flash_sfud_port.c"
  "${SOLUTION_ROOT}/Component/FlashDB/src/fdb.c"
  "${SOLUTION_ROOT}/Component/FlashDB/src/fdb_file.c"
  "${SOLUTION_ROOT}/Component/FlashDB/src/fdb_kvdb.c"
  "${SOLUTION_ROOT}/Component/FlashDB/src/fdb_tsdb.c"
  "${SOLUTION_ROOT}/Component/FlashDB/src/fdb_utils.c"
  "${SOLUTION_ROOT}/Component/SFUD/sfud/src/sfud.c"
  "${SOLUTION_ROOT}/Component/SFUD/sfud/src/sfud_sfdp.c"
  "${SOLUTION_ROOT}/Component/SFUD/sfud/port/sfud_port.c"
  "${SOLUTION_ROOT}/Component/FlashDB/port/fal/samples/porting/fal_flash_stm32f1_port.c"
)
target_include_directories(Group_FlashDB PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  ${SOLUTION_ROOT}/Component/fal/samples/porting
)
target_compile_definitions(Group_FlashDB PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_FlashDB_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_FlashDB_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_FlashDB PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_FlashDB PUBLIC
  Group_FlashDB_ABSTRACTIONS
)

# group freemodbus/modbus
add_library(Group_freemodbus_modbus OBJECT
  "${SOLUTION_ROOT}/Component/modbus/modbus/rtu/mbcrc.c"
  "${SOLUTION_ROOT}/Component/modbus/modbus/functions/mbfuncother.c"
  "${SOLUTION_ROOT}/Component/modbus/modbus/functions/mbutils.c"
)
target_include_directories(Group_freemodbus_modbus PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_freemodbus_modbus PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_freemodbus_modbus_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_freemodbus_modbus_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_freemodbus_modbus PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_freemodbus_modbus PUBLIC
  Group_freemodbus_modbus_ABSTRACTIONS
)

# group freemodbus/slave
add_library(Group_freemodbus_slave OBJECT
  "${SOLUTION_ROOT}/Component/modbus/slave/mb.c"
  "${SOLUTION_ROOT}/Component/modbus/slave/mbfunccoils.c"
  "${SOLUTION_ROOT}/Component/modbus/slave/mbfuncdisc.c"
  "${SOLUTION_ROOT}/Component/modbus/slave/mbfuncholding.c"
  "${SOLUTION_ROOT}/Component/modbus/slave/mbfuncinput.c"
  "${SOLUTION_ROOT}/Component/modbus/slave/mbrtu.c"
  "${SOLUTION_ROOT}/Component/modbus/slave/portevent.c"
  "${SOLUTION_ROOT}/Component/modbus/slave/portserial.c"
  "${SOLUTION_ROOT}/Component/modbus/modbus/ascii/mbascii.c"
  "${SOLUTION_ROOT}/Component/modbus/slave/porttimer.c"
  "${SOLUTION_ROOT}/Component/modbus/slave/slave_mb_app.c"
)
target_include_directories(Group_freemodbus_slave PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  ${SOLUTION_ROOT}/Component/modbus/slave
)
target_compile_definitions(Group_freemodbus_slave PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_freemodbus_slave_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_freemodbus_slave_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_freemodbus_slave PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_freemodbus_slave PUBLIC
  Group_freemodbus_slave_ABSTRACTIONS
)

# group freemodbus/master
add_library(Group_freemodbus_master OBJECT
  "${SOLUTION_ROOT}/Component/modbus/master/master_mb_app.c"
  "${SOLUTION_ROOT}/Component/modbus/master/src/agile_modbus.c"
  "${SOLUTION_ROOT}/Component/modbus/master/src/agile_modbus_rtu.c"
  "${SOLUTION_ROOT}/Component/modbus/master/src/agile_modbus_tcp.c"
)
target_include_directories(Group_freemodbus_master PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  ${SOLUTION_ROOT}/Component/modbus/master
  ${SOLUTION_ROOT}/Component/modbus/master/inc
)
target_compile_definitions(Group_freemodbus_master PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_freemodbus_master_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_freemodbus_master_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_freemodbus_master PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_freemodbus_master PUBLIC
  Group_freemodbus_master_ABSTRACTIONS
)

# group PID
add_library(Group_PID OBJECT
  "${SOLUTION_ROOT}/Component/PID/src/qpid.c"
)
target_include_directories(Group_PID PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_PID PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_PID_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_PID_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_PID PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_PID PUBLIC
  Group_PID_ABSTRACTIONS
)

# group AT
add_library(Group_AT OBJECT
  "${SOLUTION_ROOT}/Component/AT/AT_Device.c"
  "${SOLUTION_ROOT}/Component/cJSON-v1.7.17/cJSON.c"
  "${SOLUTION_ROOT}/Component/cJSON-v1.7.17/cJSON_Utils.c"
  "${SOLUTION_ROOT}/Component/cJSON-v1.7.17/user_cjson.c"
  "${SOLUTION_ROOT}/Component/AT/AT_Cmd.c"
)
target_include_directories(Group_AT PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  ${SOLUTION_ROOT}/Component/AT
  ${SOLUTION_ROOT}/Component/cJSON-v1.7.17
)
target_compile_definitions(Group_AT PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_AT_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_AT_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_AT PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_AT PUBLIC
  Group_AT_ABSTRACTIONS
)
