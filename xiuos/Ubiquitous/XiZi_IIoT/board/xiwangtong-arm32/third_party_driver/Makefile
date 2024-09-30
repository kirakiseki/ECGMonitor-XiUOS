SRC_DIR := common gpio

ifeq ($(CONFIG_BSP_USING_LWIP),y)
  SRC_DIR += ethernet
endif

ifeq ($(CONFIG_BSP_USING_SEMC),y)
  SRC_DIR += semc
endif

ifeq ($(CONFIG_BSP_USING_I2C),y)
  SRC_DIR += i2c
endif

ifeq ($(CONFIG_BSP_USING_LPUART),y)
  SRC_DIR += uart
endif

ifeq ($(CONFIG_BSP_USING_CH438),y)
  SRC_DIR += ch438
endif

ifeq ($(CONFIG_BSP_USING_SDIO),y)
  SRC_DIR += sdio
endif

ifeq ($(CONFIG_BSP_USING_USB),y)
  SRC_DIR += usb
endif

ifeq ($(CONFIG_BSP_USING_LCD),y)
  SRC_DIR += lcd
endif

ifeq ($(CONFIG_BSP_USING_TOUCH),y)
  SRC_DIR += touch
endif

ifeq ($(CONFIG_BSP_USING_WDT),y)
  SRC_DIR += wdt
endif

include $(KERNEL_ROOT)/compiler.mk
