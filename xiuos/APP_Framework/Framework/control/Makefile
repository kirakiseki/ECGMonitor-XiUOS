SRC_DIR := shared

ifeq ($(CONFIG_CONTROL_IPC_PROTOCOL), y)
	SRC_DIR += ipc_protocol
endif

ifeq ($(CONFIG_CONTROL_PLC_PROTOCOL), y)
	SRC_DIR += plc_protocol
endif

include $(KERNEL_ROOT)/compiler.mk
