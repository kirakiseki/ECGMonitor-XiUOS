include $(KERNEL_ROOT)/.config

ifeq ($(CONFIG_ADD_NUTTX_FEATURES),y)
    include $(APPDIR)/Make.defs
    CSRCS += framework_init.c
    include $(APPDIR)/Application.mk
endif

ifeq ($(CONFIG_ADD_XIZI_FEATURES),y)
    SRC_FILES := framework_init.c
    SRC_DIR :=  transform_layer 

    ifeq ($(CONFIG_SUPPORT_SENSOR_FRAMEWORK),y)
        SRC_DIR += sensor
    endif

    ifeq ($(CONFIG_SUPPORT_CONNECTION_FRAMEWORK),y)
        SRC_DIR += connection
    endif

    ifeq ($(CONFIG_SUPPORT_KNOWING_FRAMEWORK),y)
        SRC_DIR += knowing
    endif

    ifeq ($(CONFIG_SUPPORT_CONTROL_FRAMEWORK),y)
        SRC_DIR += control
    endif

    ifeq ($(CONFIG_CRYPTO),y)
        SRC_DIR += security
    endif

    ifeq ($(CONFIG_MBEDTLS), y)
        SRC_DIR += security
    endif

    include $(KERNEL_ROOT)/compiler.mk

endif

