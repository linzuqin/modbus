cmake_minimum_required(VERSION 3.27)

# Roots
include("../roots.cmake")

set(CONTEXT Project+Target_1)
set(TARGET ${CONTEXT})
set(OUT_DIR "${SOLUTION_ROOT}/out/Project/Target_1")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LD_SCRIPT "${SOLUTION_ROOT}/Project_Target_1.sct")
set(LD_SCRIPT_PP ${LD_SCRIPT})

# Processor Options
set(CPU Cortex-M3)
set(FPU NO_FPU)
set(BYTE_ORDER Little-endian)

# Toolchain config map
include("toolchain.cmake")

# Setup project
project(${CONTEXT} LANGUAGES C ASM)

# Compilation database
add_custom_target(database
  COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" "${OUT_DIR}"
)
set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})

# Setup context
add_executable(${CONTEXT})
set_target_properties(${CONTEXT} PROPERTIES PREFIX "" SUFFIX ".axf" OUTPUT_NAME "Project")
set_target_properties(${CONTEXT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR})
add_library(${CONTEXT}_GLOBAL INTERFACE)

# Includes
target_include_directories(${CONTEXT} PUBLIC
  $<$<COMPILE_LANGUAGE:C,CXX>:
    ${SOLUTION_ROOT}/System
    ${SOLUTION_ROOT}/Component/fal/inc
    ${SOLUTION_ROOT}/Component/FlashDB/inc
    ${SOLUTION_ROOT}/Component/SFUD/sfud/inc
    ${SOLUTION_ROOT}/Component/XMODEM
    ${SOLUTION_ROOT}/Component/ymodem
    ${SOLUTION_ROOT}/Component/modbus/modbus/ascii
    ${SOLUTION_ROOT}/Component/modbus/modbus/functions
    ${SOLUTION_ROOT}/Component/modbus/modbus/include
    ${SOLUTION_ROOT}/Component/modbus/modbus/rtu
    ${SOLUTION_ROOT}/Component/modbus/modbus
    ${SOLUTION_ROOT}/Component/modbus/modbus/tcp
    ${SOLUTION_ROOT}/Component/PID/inc
  >
  ${SOLUTION_ROOT}/RTE/_Target_1
  ${SOLUTION_ROOT}/RTE/RTOS
  ${CMSIS_PACK_ROOT}/Keil/STM32F1xx_DFP/2.4.1/Device/Include
  ${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/components/finsh
  ${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/include
  ${SOLUTION_ROOT}/Start
  ${SOLUTION_ROOT}/Library
  ${SOLUTION_ROOT}/Hardware
  ${SOLUTION_ROOT}/User
  ${SOLUTION_ROOT}/RTT
  ${SOLUTION_ROOT}/Component/fal/samples/porting
  ${SOLUTION_ROOT}/Component/modbus/slave
  ${SOLUTION_ROOT}/Component/modbus/master
  ${SOLUTION_ROOT}/Component/modbus/master/inc
  ${SOLUTION_ROOT}/Component/AT
  ${SOLUTION_ROOT}/Component/cJSON-v1.7.17
)

# Defines
target_compile_definitions(${CONTEXT} PUBLIC
  $<$<COMPILE_LANGUAGE:C,CXX>:
    USE_STDPERIPH_DRIVER
    STM32F10X_HD
    __MICROLIB
    STM32F10X_HD
    _RTE_
  >
)

# Compile options
target_compile_options(${CONTEXT} PUBLIC
  $<$<COMPILE_LANGUAGE:ASM>:
    "SHELL:${ASM_CPU}"
    "SHELL:${ASM_FLAGS}"
    "SHELL:${ASM_BYTE_ORDER}"
    "SHELL:-masm=auto"
  >
  $<$<COMPILE_LANGUAGE:C>:
    "SHELL:${CC_CPU}"
    "SHELL:${CC_FLAGS}"
    "SHELL:${CC_BYTE_ORDER}"
    "SHELL:-std=c99"
    "SHELL:-fno-rtti"
    "SHELL:-funsigned-char"
    "SHELL:-fshort-enums"
    "SHELL:-fshort-wchar"
    "SHELL:-ffunction-sections"
    "SHELL:-Wno-packed"
    "SHELL:-Wno-missing-variable-declarations"
    "SHELL:-Wno-missing-prototypes"
    "SHELL:-Wno-missing-noreturn"
    "SHELL:-Wno-sign-conversion"
    "SHELL:-Wno-nonportable-include-path"
    "SHELL:-Wno-reserved-id-macro"
    "SHELL:-Wno-unused-macros"
    "SHELL:-Wno-documentation-unknown-command"
    "SHELL:-Wno-documentation"
    "SHELL:-Wno-license-management"
    "SHELL:-Wno-parentheses-equality"
  >
)

# Compile Options Abstractions
add_library(${CONTEXT}_ABSTRACTIONS INTERFACE)
cbuild_set_options_flags(CC "debug" "on" "" "" CC_OPTIONS_FLAGS_${CONTEXT})
cbuild_set_options_flags(ASM "debug" "on" "" "" ASM_OPTIONS_FLAGS_${CONTEXT})
target_compile_options(${CONTEXT}_ABSTRACTIONS INTERFACE
  $<$<COMPILE_LANGUAGE:C>:
    "SHELL:${CC_OPTIONS_FLAGS_${CONTEXT}}"
  >
  $<$<COMPILE_LANGUAGE:ASM>:
    "SHELL:${ASM_OPTIONS_FLAGS_${CONTEXT}}"
  >
)

# Add groups and components
include("groups.cmake")
include("components.cmake")

target_link_libraries(${CONTEXT} PUBLIC
  Group_Start
  Group_Library
  Group_Hardware
  Group_User
  Group_RTT
  Group_FlashDB
  Group_freemodbus_modbus
  Group_freemodbus_slave
  Group_freemodbus_master
  Group_PID
  Group_AT
  RealThread_RTOS_RT-Thread_kernel_3_1_5
  RealThread_RTOS_RT-Thread_shell_3_1_5
)

# Linker options
target_link_options(${CONTEXT} PUBLIC
  "SHELL:${LD_CPU}"
  "SHELL:${_LS}\"${LD_SCRIPT_PP}\""
  "SHELL:--library_type=microlib"
  "SHELL:--map"
  "SHELL:--load_addr_map_info"
  "SHELL:--xref"
  "SHELL:--callgraph"
  "SHELL:--symbols"
  "SHELL:--info sizes"
  "SHELL:--info totals"
  "SHELL:--info unused"
  "SHELL:--info veneers"
  "SHELL:--strict"
  "SHELL:--summary_stderr"
  "SHELL:--info summarysizes"
)
set_target_properties(${CONTEXT} PROPERTIES LINK_DEPENDS ${LD_SCRIPT})
