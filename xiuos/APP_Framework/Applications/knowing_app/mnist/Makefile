SRC_FILES := 
ifeq ($(CONFIG_APP_MNIST),y)
SRC_FILES := \
		main.cpp \
		mnistmain.c
	CPPPATHS += -I.
endif

include $(KERNEL_ROOT)/compiler.mk