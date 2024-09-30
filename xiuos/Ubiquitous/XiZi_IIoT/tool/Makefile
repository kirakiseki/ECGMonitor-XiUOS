SRC_DIR := 

ifeq ($(CONFIG_TOOL_SHELL),y)
  SRC_DIR += shell
endif

ifeq ($(CONFIG_TOOL_USING_OTA),y)
  SRC_DIR += bootloader
endif

include $(KERNEL_ROOT)/compiler.mk
