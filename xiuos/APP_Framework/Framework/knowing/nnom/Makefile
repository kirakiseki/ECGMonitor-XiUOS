SRC_DIR := 
ifeq ($(CONFIG_USING_NNOM),y)
	SRC_FILES := $(wildcard src/backends/*.c) $(wildcard src/core/*.c) $(wildcard src/layers/*.c)
endif

include $(KERNEL_ROOT)/compiler.mk
