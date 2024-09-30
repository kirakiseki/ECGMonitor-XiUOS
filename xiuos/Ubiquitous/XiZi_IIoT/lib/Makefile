SRC_DIR :=    
MUSL_DIR :=



ifeq ($(CONFIG_LIB_NEWLIB),y)
SRC_DIR += newlib
endif

ifeq ($(CONFIG_LIB_CPLUSPLUS),y)
SRC_DIR += libcpp
endif

ifeq ($(CONFIG_LIB_MUSLLIB), y)
# SRC_DIR += musllib
# MUSL_DIR += musllib
endif

include $(KERNEL_ROOT)/compiler.mk

