SRC_DIR :=

ifeq ($(CONFIG_USING_CMSIS_5_NN_DEMOAPP),y)
SRC_DIR += cmsisnn-cifar10
endif

ifeq ($(CONFIG_USING_CMSIS_5_NN_DEMOAPP_VEG_CLASSIFY),y)
SRC_DIR += cmsisnn_vegetable_classify
endif

include $(KERNEL_ROOT)/compiler.mk