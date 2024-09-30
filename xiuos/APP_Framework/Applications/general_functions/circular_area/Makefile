include $(KERNEL_ROOT)/.config
ifeq ($(CONFIG_ADD_NUTTX_FEATURES),y)
    include $(APPDIR)/Make.defs
    CSRCS += circular_area_app.c
    include $(APPDIR)/Application.mk
endif

ifeq ($(CONFIG_ADD_XIZI_FEATURES),y)
    SRC_FILES := circular_area_app.c
    include $(KERNEL_ROOT)/compiler.mk
endif