SRC_DIR :=

ifeq ($(CONFIG_APP_MNIST),y)
SRC_DIR += mnist
endif

ifeq ($(CONFIG_USING_NNOM_DEMOAPP),y)
SRC_DIR += nnom_demo
endif

ifeq ($(CONFIG_USING_CMSIS_5_DEMOAPP),y)
SRC_DIR += cmsis_5_demo
endif

include $(KERNEL_ROOT)/compiler.mk