include $(KERNEL_ROOT)/.config
ifeq ($(CONFIG_ADD_NUTTX_FEATURES),y)
    include $(APPDIR)/Make.defs
    CSRCS += adapter_4g.c
    include $(APPDIR)/Application.mk

endif

ifeq ($(CONFIG_ADD_XIZI_FEATURES),y)
    SRC_FILES := adapter_4g.c

    ifeq ($(CONFIG_ADAPTER_EC200T),y)
        SRC_DIR += ec200t
    endif

    include $(KERNEL_ROOT)/compiler.mk
endif