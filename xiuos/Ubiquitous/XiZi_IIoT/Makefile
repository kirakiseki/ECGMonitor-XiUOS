MAKEFLAGS += --no-print-directory


.PHONY:all clean distclean show_info  menuconfig
.PHONY:COMPILE_APP COMPILE_KERNEL


riscv_support := kd233 maix-go hifive1-rev-B gapuino gd32vf103-rvstar rv32m1-vega aiit-riscv64-board xidatong-riscv64 edu-riscv64
arm_support += stm32f407-st-discovery stm32f407zgt6 stm32f103-nano nuvoton-m2354 ok1052-c imxrt1176-sbc aiit-arm32-board xidatong-arm32 xiwangtong-arm32 edu-arm32
emulator_support += hifive1-emulator k210-emulator cortex-m0-emulator cortex-m3-emulator cortex-m4-emulator
support := $(riscv_support) $(arm_support) $(emulator_support) 
SRC_DIR :=

export BOARD ?=kd233
# This is the environment variable for kconfig-mconf
export KCONFIG_CONFIG ?= .config

ifeq ($(filter $(BOARD),$(support)),)
$(warning "You should choose board like this:make BOARD=kd233")
$(warning "This is what we support:")
$(warning "RISCV EVB:    $(riscv_support)")
$(warning "ARM EVB:      $(arm_support)")
$(warning "EMULATORS:    $(emulator_support)")
# $(warning "$(support)")
$(error "break" )
endif

export TARGET 
export COMPILE_TYPE
export KERNEL_ROOT ?=$(strip $(shell pwd))

MAKEFILES  =$(KERNEL_ROOT)/.config
-include $(KERNEL_ROOT)/.config

export BSP_ROOT ?= $(KERNEL_ROOT)/board/$(BOARD)
export UBIQUITOUS_ROOT ?= ..
include board/$(BOARD)/config.mk
export BSP_BUILD_DIR := board/$(BOARD)
export HOSTTOOLS_DIR ?= $(KERNEL_ROOT)/tool/hosttools
export CONFIG2H_EXE ?= $(HOSTTOOLS_DIR)/xsconfig.sh

export GEN_KSELECTOR_EXE ?= $(HOSTTOOLS_DIR)/generate_kselector.py
export FEATURE2YAML_EXE ?= $(HOSTTOOLS_DIR)/kernel_selector.sh
export KSELECTOR_EXE ?= $(HOSTTOOLS_DIR)/kernel_selector.py
export CPPPATHS
export SRC_APP_DIR := ../../APP_Framework
export SRC_KERNEL_DIR := arch board lib fs kernel resources tool
export SRC_DIR:= $(SRC_APP_DIR) $(SRC_KERNEL_DIR)
export LIBCC 
export MUSL_DIR := $(KERNEL_ROOT)/lib/musllib
export LWIP_DIR := $(KERNEL_ROOT)/resources/ethernet

PART:=

all:
ifeq ($(CONFIG_COMPILER_APP)_$(CONFIG_COMPILER_KERNEL),y_)
PART += COMPILE_APP

else ifeq ($(CONFIG_COMPILER_APP)_$(CONFIG_COMPILER_KERNEL),_y)
PART += COMPILE_KERNEL

else ifeq ($(CONFIG_COMPILER_APP)_$(CONFIG_COMPILER_KERNEL),y_y)
PART := COMPILE_APP COMPILE_KERNEL

else
PART := 

ifeq ($(CONFIG_LIB_MUSLLIB), y)
PART += COMPILE_MUSL
endif

ifeq ($(CONFIG_RESOURCES_LWIP), y)
PART += COMPILE_LWIP
endif

ifeq ($(CONFIG_MCUBOOT_BOOTLOADER), y)
PART += COMPILE_BOOTLOADER
else ifeq ($(CONFIG_MCUBOOT_APPLICATION), y)
PART += COMPILE_APPLICATION
else
PART += COMPILE_ALL
endif

endif


all: $(PART)


COMPILE_ALL:
	@for dir in $(SRC_DIR);do    \
               $(MAKE) -C $$dir;          \
       done
	@cp link.mk build/Makefile
	@$(MAKE) -C build TARGET=XiZi-$(BOARD).elf LINK_FLAGS=LFLAGS
	@rm build/Makefile build/make.obj

COMPILE_BOOTLOADER:
	@for dir in $(SRC_DIR);do    \
               $(MAKE) -C $$dir;          \
       done
	@cp link.mk build/Makefile
	@$(MAKE) -C build COMPILE_TYPE="-boot" TARGET=XiZi-$(BOARD)-boot.elf LINK_FLAGS=LFLAGS
	@rm build/Makefile build/make.obj

COMPILE_APPLICATION:
	@for dir in $(SRC_DIR);do    \
               $(MAKE) -C $$dir;          \
       done
	@cp link.mk build/Makefile
	@$(MAKE) -C build COMPILE_TYPE="-app" TARGET=XiZi-$(BOARD)-app.elf LINK_FLAGS=LFLAGS
	@rm build/Makefile build/make.obj

COMPILE_MUSL:
	@for dir in $(MUSL_DIR);do    \
               $(MAKE) -C $$dir COMPILE_TYPE=$@ CONFIG_RESOURCES_LWIP=n;          \
       done
	@cp link_libc.mk build/Makefile
	@$(MAKE) -C build TARGET=libmusl.a LINK_FLAGS=LFLAGS
	@cp build/libmusl.a $(KERNEL_ROOT)/lib/musllib/libmusl.a
	@rm build/Makefile build/make.obj

COMPILE_LWIP: 
	@for dir in $(LWIP_DIR);do    \
               $(MAKE) -C $$dir COMPILE_TYPE=$@;          \
       done
	@cp link_lwip.mk build/Makefile
	@$(MAKE) -C build TARGET=liblwip.a LINK_FLAGS=LFLAGS
	@cp build/liblwip.a $(KERNEL_ROOT)/resources/ethernet/LwIP/liblwip.a
	@rm build/Makefile build/make.obj

COMPILE_KERNEL:
	@for dir in $(SRC_KERNEL_DIR);do    \
               $(MAKE) -C $$dir;          \
       done
	@cp link.mk build/Makefile
	@$(MAKE) -C build COMPILE_TYPE="_kernel" TARGET=XiZi-$(BOARD)_kernel.elf LINK_FLAGS=LFLAGS
	@rm build/Makefile build/make.obj

COMPILE_APP:
	@echo $(SRC_APP_DIR)
	@for dir in $(SRC_APP_DIR);do    \
               $(MAKE) -C $$dir;          \
       done
	@cp link.mk build/Makefile
	@$(MAKE) -C build COMPILE_TYPE="_app" TARGET=XiZi-$(BOARD)_app.elf LINK_FLAGS=APPLFLAGS  USE_APP_INCLUDEPATH=y
	@rm build/Makefile build/make.obj

show_info:
	@echo "CONFIG_COMPILER_APP is :" $(CONFIG_COMPILER_APP)
	@echo "CONFIG_COMPILER_KERNEL is :" $(CONFIG_COMPILER_KERNEL)
	@echo "KERNELPATHS is :" $(KERNELPATHS)
	@echo "TARGET is :" $(TARGET)
	@echo "VPATH is :" $(VPATH)
	@echo "BSP_ROOT is :" $(BSP_ROOT)
	@echo "KERNEL_ROOT is :" $(KERNEL_ROOT)
	@echo "CPPPATHS is :" $(CPPPATHS)
	@echo "SRC_DIR is :" $(SRC_DIR)
	@echo "BUILD_DIR is :" $(BUILD_DIR)
	@echo "RTT_ROOT_DIR is :" $(RTT_ROOT_DIR)
	@echo "BSP_BUILD_DIR is :" $(BSP_BUILD_DIR)
	@echo "OBJS is :" $(OBJS)
	@for f in $(CPPPATHS); do \
		echo $$f;                \
	done



menuconfig: 
	@if [  -f "$(BSP_ROOT)/.config" ]; then \
  		cp $(BSP_ROOT)/.config $(KERNEL_ROOT)/.config; \
		else if [  -f "$(BSP_ROOT)/.defconfig" ]; then \
		cp $(BSP_ROOT)/.defconfig $(KERNEL_ROOT)/.config ;\
	fi ;fi
	@kconfig-mconf $(BSP_ROOT)/Kconfig
	@$(CONFIG2H_EXE) .config
	@cp $(KERNEL_ROOT)/.config $(BSP_ROOT)/.config

kernel_selector:
	$(eval KCONFIG_CONFIG := .selector)
	@if [ -f "$(BSP_ROOT)/.selector" ]; then \
		cp $(BSP_ROOT)/.selector $(KERNEL_ROOT)/.selector; \
		else if [ -f "$(BSP_ROOT)/.defselector" ]; then \
		cp $(BSP_ROOT)/.defselector $(KERNEL_ROOT)/.selector ;\
	fi ;fi
	@$(GEN_KSELECTOR_EXE)
	@cp "$(UBIQUITOUS_ROOT)/Kselector_features_meta" $(UBIQUITOUS_ROOT)/Kselector_features
	@cp "$(UBIQUITOUS_ROOT)/Kselector_params_meta" $(UBIQUITOUS_ROOT)/Kselector_params
	@kconfig-mconf $(BSP_ROOT)/Kselector
	@$(FEATURE2YAML_EXE) .selector
	@if [ -f "$(BSP_ROOT)/requirement.yaml" ]; then \
		cp $(BSP_ROOT)/requirement.yaml $(KERNEL_ROOT)/requirement.yaml; \
	fi
	@$(KSELECTOR_EXE)
	@cp $(KERNEL_ROOT)/.selector $(BSP_ROOT)/.selector

clean:
	@echo Clean target and build_dir
	@rm -rf build
	@rm -rf temp.txt

distclean:
	@echo Clean all configuration
	@make clean
	@rm -f .config*
	@rm -f $(KERNEL_ROOT)/lib/musllib/libmusl.a
	@rm -f $(KERNEL_ROOT)/resources/ethernet/LwIP/liblwip.a
	@rm -f $(KERNEL_ROOT)/board/*/.config
