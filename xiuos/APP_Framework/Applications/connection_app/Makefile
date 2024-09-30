include $(KERNEL_ROOT)/.config

ifeq ($(CONFIG_ADD_NUTTX_FEATURES),y)
    include $(APPDIR)/Make.defs
    include $(APPDIR)/Application.mk
endif

ifeq ($(CONFIG_ADD_XIZI_FEATURES),y)

	ifeq ($(CONFIG_CONNECTION_ADAPTER_4G),y)
		SRC_DIR += 4g_app
	endif

	ifeq ($(CONFIG_RESOURCES_LWIP),y)
		SRC_DIR += socket_demo
	endif

    include $(KERNEL_ROOT)/compiler.mk
endif

