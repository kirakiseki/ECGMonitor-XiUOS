SRC_DIR := 

ifeq ($(CONFIG_LIB_USING_LORA_RADIO),y)
	SRC_DIR += lora_radio_driver
endif

ifeq ($(CONFIG_LIB_USING_LORAWAN_ED_STACK),y)
	SRC_DIR += lorawan_devicenode
endif

ifeq ($(CONFIG_LIB_USING_LORAWAN_GATEWAY_SC),y)
	SRC_DIR += lorawan_gateway_single_channel
endif

include $(KERNEL_ROOT)/compiler.mk
