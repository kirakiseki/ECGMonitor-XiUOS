ifeq ($(CONFIG_BOARD_CORTEX_M4_EVB),y)
SRC_FILES := interrupt.c system_init.c boot.S interrupt_vector.S coreclock.c svc_entry.S

ifeq ($(CONFIG_TASK_ISOLATION),y)
SRC_FILES += svc_handle.c mpu.c
endif
endif

ifeq ($(CONFIG_BOARD_STM32F407_EVB),y)
SRC_FILES := interrupt.c system_init.c boot.S interrupt_vector.S coreclock.c svc_entry.S

ifeq ($(CONFIG_TASK_ISOLATION),y)
SRC_FILES += svc_handle.c mpu.c
endif
endif

ifeq ($(CONFIG_BOARD_EDU_ARM32_EVB),y)
SRC_DIR += hc32f4a0
endif

include $(KERNEL_ROOT)/compiler.mk
