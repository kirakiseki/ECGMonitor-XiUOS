include $(KERNEL_ROOT)/.config
ifeq ($(CONFIG_ADD_NUTTX_FEATURES),y)
    include $(APPDIR)/Make.defs
    include $(APPDIR)/Application.mk
endif

ifeq ($(CONFIG_ADD_XIZI_FEATURES),y)
    SRC_DIR := Applications Framework lib
    include $(KERNEL_ROOT)/compiler.mk
endif

