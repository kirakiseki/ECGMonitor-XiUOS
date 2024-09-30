include $(KERNEL_ROOT)/.config
ifeq ($(CONFIG_ADD_NUTTX_FEATURES),y)
    include $(APPDIR)/Make.defs
    CSRCS += adapter_bluetooth.c
    include $(APPDIR)/Application.mk

endif

ifeq ($(CONFIG_ADD_XIZI_FEATURES),y)
    SRC_FILES := adapter_bluetooth.c

    ifeq ($(CONFIG_ADAPTER_HC08),y)
        SRC_DIR += hc08
    endif

    include $(KERNEL_ROOT)/compiler.mk
endif
