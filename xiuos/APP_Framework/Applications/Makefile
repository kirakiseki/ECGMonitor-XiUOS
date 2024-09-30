include $(KERNEL_ROOT)/.config

ifeq ($(CONFIG_ADD_NUTTX_FEATURES),y)
    include $(APPDIR)/Make.defs
    CSRCS +=
    include $(APPDIR)/Application.mk
endif


ifeq ($(CONFIG_ADD_XIZI_FEATURES),y)
    SRC_DIR := general_functions app_test 

    SRC_FILES := main.c
    ifeq ($(CONFIG_LIB_LV),y)
        SRC_DIR += lv_app
        SRC_DIR += tinyttf_app
    endif

    ifeq ($(CONFIG_APPLICATION_OTA),y)
        SRC_DIR += ota
    endif

    ifeq ($(CONFIG_APPLICATION_SENSOR),y)
        SRC_DIR += sensor_app
    endif

    ifeq ($(CONFIG_APPLICATION_CONNECTION),y)
        SRC_DIR += connection_app
    endif

    ifeq ($(CONFIG_APPLICATION_KNOWING),y)
        SRC_DIR += knowing_app
    endif

    ifeq ($(CONFIG_APPLICATION_CONTROL),y)
        SRC_DIR += control_app
    endif

    include $(KERNEL_ROOT)/compiler.mk
endif