include $(KERNEL_ROOT)/.config
ifeq ($(CONFIG_ADD_NUTTX_FEATURES),y)
    include $(APPDIR)/Make.defs
    CSRCS += adapter_wifi.c
    include $(APPDIR)/Application.mk

endif

ifeq ($(CONFIG_ADD_XIZI_FEATURES),y)
SRC_FILES := adapter_wifi.c

ifeq ($(CONFIG_ADAPTER_HFA21_WIFI),y)
	SRC_DIR += hfa21_wifi
endif

ifeq ($(CONFIG_ADAPTER_ESP07S_WIFI),y)
	SRC_DIR += esp07s_wifi
endif

ifeq ($(CONFIG_ADAPTER_ESP8285_WIFI),y)
	SRC_DIR += esp8285_wifi
endif

include $(KERNEL_ROOT)/compiler.mk
endif
