SRC_FILES := hc32_ll.c hc32_ll_clk.c hc32_ll_icg.c hc32_ll_utility.c hc32_ll_fcg.c hc32_ll_interrupts.c hc32_ll_pwc.c hc32_ll_aos.c hc32_ll_sram.c hc32_ll_efm.c hc32f4a0_ll_interrupts_share.c hc32_ll_gpio.c hc32_ll_rmu.c hc32_ll_tmr0.c hc32_ll_dma.c

ifeq ($(CONFIG_BSP_USING_UART),y)
  SRC_FILES += hc32_ll_usart.c
endif

ifeq ($(CONFIG_BSP_USING_ADC),y)
  SRC_FILES += hc32_ll_adc.c
endif

ifeq ($(CONFIG_BSP_USING_DAC),y)
  SRC_FILES += hc32_ll_dac.c
endif

ifeq ($(CONFIG_BSP_USING_SDIO),y)
  SRC_FILES += hc32_ll_sdioc.c
endif

ifeq ($(CONFIG_BSP_USING_SPI),y)
  SRC_FILES += hc32_ll_spi.c
endif

ifeq ($(CONFIG_BSP_USING_QSPI_FLASH),y)
  SRC_FILES += hc32_ll_qspi.c
endif

ifeq ($(CONFIG_BSP_USING_I2C),y)
  SRC_FILES += hc32_ll_i2c.c
endif

ifeq ($(CONFIG_BSP_USING_LWIP),y)
  SRC_FILES += hc32_ll_eth.c
endif

ifeq ($(CONFIG_BSP_USING_USB),y)
  SRC_FILES += hc32_ll_usb.c
endif

ifeq ($(CONFIG_BSP_USING_RTC),y)
  SRC_FILES += hc32_ll_rtc.c
endif

ifeq ($(CONFIG_BSP_USING_WDT),y)
  SRC_FILES += hc32_ll_wdt.c
endif

ifeq ($(CONFIG_BSP_USING_CAN),y)
  SRC_FILES += hc32_ll_can.c
endif

include $(KERNEL_ROOT)/compiler.mk
