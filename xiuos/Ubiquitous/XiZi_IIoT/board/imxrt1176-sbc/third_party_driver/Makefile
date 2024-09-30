SRC_DIR := common gpio cm7

ifeq ($(CONFIG_BSP_USING_LPUART),y)
  SRC_DIR += uart
endif

ifeq ($(CONFIG_BSP_USING_LWIP),y)
  SRC_DIR += ethernet
endif

include $(KERNEL_ROOT)/compiler.mk
