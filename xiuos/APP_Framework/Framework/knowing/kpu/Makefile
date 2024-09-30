SRC_DIR := 

ifeq ($(CONFIG_USING_KPU_PROCESSING),y)
	SRC_DIR += k210_yolov2_detect_procedure yolov2 yolov2_json
endif

include $(KERNEL_ROOT)/compiler.mk
