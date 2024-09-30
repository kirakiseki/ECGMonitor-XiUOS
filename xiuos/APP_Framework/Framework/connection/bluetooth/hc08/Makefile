include $(KERNEL_ROOT)/.config
ifeq ($(CONFIG_ADD_NUTTX_FEATURES),y)
    include $(APPDIR)/Make.defs
    CSRCS += hc08.c
    include $(APPDIR)/Application.mk

endif

ifeq ($(CONFIG_ADD_XIZI_FEATURES),y)
    SRC_FILES := hc08.c
    include $(KERNEL_ROOT)/compiler.mk

endif
