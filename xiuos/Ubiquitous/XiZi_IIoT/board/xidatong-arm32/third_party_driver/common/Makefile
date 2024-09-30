SRC_FILES := system_MIMXRT1052.c fsl_cache.c fsl_clock.c fsl_common.c pin_mux.c clock_config.c

ifeq ($(CONFIG_BSP_USING_SDIO),y)
  SRC_FILES += fsl_usdhc.c
endif

ifeq ($(CONFIG_TOOL_USING_OTA),y)
  SRC_FILES += fsl_romapi.c flash.c mcuboot.c common.c ymodem.c
endif

include $(KERNEL_ROOT)/compiler.mk
