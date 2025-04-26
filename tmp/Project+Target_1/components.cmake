# components.cmake

# component RealThread::RTOS&RT-Thread:kernel@3.1.5
add_library(RealThread_RTOS_RT-Thread_kernel_3_1_5 OBJECT
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/libcpu/arm/cortex-m3/context_rvds.S"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/libcpu/arm/cortex-m3/cpuport.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/src/clock.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/src/components.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/src/idle.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/src/ipc.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/src/irq.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/src/kservice.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/src/mem.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/src/mempool.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/src/object.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/src/scheduler.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/src/thread.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/src/timer.c"
  "${SOLUTION_ROOT}/RTE/RTOS/board.c"
)
target_include_directories(RealThread_RTOS_RT-Thread_kernel_3_1_5 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  ${SOLUTION_ROOT}/RTE/RTOS
  ${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/include
)
target_compile_definitions(RealThread_RTOS_RT-Thread_kernel_3_1_5 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
target_compile_options(RealThread_RTOS_RT-Thread_kernel_3_1_5 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(RealThread_RTOS_RT-Thread_kernel_3_1_5 PUBLIC
  ${CONTEXT}_ABSTRACTIONS
)
set_source_files_properties("${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/libcpu/arm/cortex-m3/context_rvds.S" PROPERTIES
  COMPILE_DEFINITIONS "__MICROLIB;STM32F10X_HD;_RTE_"
)

# component RealThread::RTOS&RT-Thread:shell@3.1.5
add_library(RealThread_RTOS_RT-Thread_shell_3_1_5 OBJECT
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/components/finsh/cmd.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/components/finsh/msh.c"
  "${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/components/finsh/shell.c"
  "${SOLUTION_ROOT}/RTE/RTOS/finsh_port.c"
)
target_include_directories(RealThread_RTOS_RT-Thread_shell_3_1_5 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  ${SOLUTION_ROOT}/RTE/RTOS
  ${CMSIS_PACK_ROOT}/RealThread/RT-Thread/3.1.5/components/finsh
)
target_compile_definitions(RealThread_RTOS_RT-Thread_shell_3_1_5 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
target_compile_options(RealThread_RTOS_RT-Thread_shell_3_1_5 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(RealThread_RTOS_RT-Thread_shell_3_1_5 PUBLIC
  ${CONTEXT}_ABSTRACTIONS
)
