include $(KERNEL_ROOT)/.config

all: COMPILER

CUR_DIR := $(shell pwd)
OPLK_ROOT := $(CUR_DIR)/openPOWERLINK
LIBOPLKMN := $(OPLK_ROOT)/stack/lib/xiuos/arm/liboplkmn.a
LIBOPLKCN := $(OPLK_ROOT)/stack/lib/xiuos/arm/liboplkcn.a
LIBOPLK_BUILD_DIR := $(OPLK_ROOT)/stack/build/xiuos
OPLK_DEMO_MN_CONSOLE := $(OPLK_ROOT)/bin/xiuos/arm/demo_mn_console/libdemo_mn_console.a
OPLK_DEMO_CN_CONSOLE := $(OPLK_ROOT)/bin/xiuos/arm/demo_cn_console/libdemo_cn_console.a
OPLK_DEMO_MN_CONSOLE_BUILD_DIR := $(OPLK_ROOT)/apps/demo_mn_console/build/xiuos
OPLK_DEMO_CN_CONSOLE_BUILD_DIR := $(OPLK_ROOT)/apps/demo_cn_console/build/xiuos
OPLK_TOOLCHAIN := $(OPLK_ROOT)/cmake/toolchain-xiuos-arm-none-eabi-gnu.cmake

define oplk_build
	cd $(1) && \
		find . ! -name '.' ! -name '.gitignore' -exec rm -rf {} + && \
		cmake -DCMAKE_TOOLCHAIN_FILE=$(OPLK_TOOLCHAIN) ../.. && \
		cmake --build . --target install
endef

.PHONY: $(LIBOPLKMN) $(LIBOPLKCN)
$(LIBOPLKMN) $(LIBOPLKCN)&:
	$(call oplk_build,$(LIBOPLK_BUILD_DIR))
$(LIBOPLKMN):
	$(call oplk_build,$(LIBOPLK_BUILD_DIR))
$(LIBOPLKCN):
	$(call oplk_build,$(LIBOPLK_BUILD_DIR))

.PHONY: $(OPLK_DEMO_MN_CONSOLE)
$(OPLK_DEMO_MN_CONSOLE): ${LIBOPLKMN}
	$(call oplk_build,$(OPLK_DEMO_MN_CONSOLE_BUILD_DIR))

.PHONY: $(OPLK_DEMO_CN_CONSOLE)
$(OPLK_DEMO_CN_CONSOLE): ${LIBOPLKCN}
	$(call oplk_build,$(OPLK_DEMO_CN_CONSOLE_BUILD_DIR))

LIBOPLKMN := $(LIBOPLKMN:$(CUR_DIR)/%=%)
LIBOPLKCN := $(LIBOPLKCN:$(CUR_DIR)/%=%)
OPLK_DEMO_MN_CONSOLE := $(OPLK_DEMO_MN_CONSOLE:$(CUR_DIR)/%=%)
OPLK_DEMO_CN_CONSOLE := $(OPLK_DEMO_CN_CONSOLE:$(CUR_DIR)/%=%)

SRC_FILES := adapter_powerlink.c
ifeq ($(CONFIG_POWERLINK_MN)_$(CONFIG_POWERLINK_CN),y_)
	SRC_FILES += $(OPLK_DEMO_MN_CONSOLE) $(LIBOPLKMN)
else ifeq ($(CONFIG_POWERLINK_MN)_$(CONFIG_POWERLINK_CN),_y)
	SRC_FILES += $(OPLK_DEMO_CN_CONSOLE) $(LIBOPLKCN)
else ifeq ($(CONFIG_POWERLINK_MN)_$(CONFIG_POWERLINK_CN),y_y)
	$(error CONFIG_POWERLINK_MN and CONFIG_POWERLINK_CN cannot be enabled simultaneously due to name conflict)
endif

include $(KERNEL_ROOT)/compiler.mk
