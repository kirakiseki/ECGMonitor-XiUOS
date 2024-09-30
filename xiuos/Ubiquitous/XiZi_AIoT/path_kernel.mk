
export KERNELPATHS:= -I$(BSP_ROOT)

ifeq ($(CONFIG_LIB_MUSLLIB), y)
KERNELPATHS += -I$(KERNEL_ROOT)/services/lib/musllib/src/include \
	-I$(KERNEL_ROOT)/services/lib/musllib/include \
	-I$(KERNEL_ROOT)/services/lib/musllib/src/internal #
# chose arch for musl
ifeq ($(ARCH), arm)
KERNELPATHS += -I$(KERNEL_ROOT)/services/lib/musllib/arch/arm
endif
ifeq ($(ARCH), risc-v)
KERNELPATHS += -I$(KERNEL_ROOT)/services/lib/musllib/arch/riscv64
endif

endif # end of musl include path

ifeq ($(CONFIG_LIB_NEWLIB),y)
KERNELPATHS += -I$(KERNEL_ROOT)/services/lib/newlib/include #
endif

ifeq ($(BSP_ROOT),$(KERNEL_ROOT)/services/boards/imx6q-sabrelite)
KERNELPATHS += \
	-I$(KERNEL_ROOT)/hardkernel/arch/arm/armv7-a/cortex-a9 \
	-I$(KERNEL_ROOT)/hardkernel/abstraction \
	-I$(KERNEL_ROOT)/include \
	-I$(BSP_ROOT)/third_party_driver/include \
	-I$(BSP_ROOT)/include

ifeq ($(CONFIG_RESOURCES_LWIP),y)
KERNELPATHS += \
	-I$(KERNEL_ROOT)/resources/ethernet/LwIP \
	-I$(KERNEL_ROOT)/resources/ethernet/LwIP/include \
	-I$(KERNEL_ROOT)/resources/ethernet/LwIP/include/compat \
	-I$(KERNEL_ROOT)/resources/ethernet/LwIP/include/lwip \
	-I$(KERNEL_ROOT)/resources/ethernet/LwIP/include/netif \
	-I$(KERNEL_ROOT)/resources/ethernet/LwIP/include/lwip/apps \
	-I$(KERNEL_ROOT)/resources/ethernet/LwIP/include/lwip/priv \
	-I$(KERNEL_ROOT)/resources/ethernet/LwIP/include/lwip/prot \
	-I$(KERNEL_ROOT)/resources/ethernet/LwIP/arch 
endif
endif

KERNELPATHS += -I$(KERNEL_ROOT)/../../APP_Framework/Applications/general_functions/list #

ifeq ($(ARCH), arm)
KERNELPATHS +=-I$(KERNEL_ROOT)/arch/arm/shared \
	-I$(KERNEL_ROOT)/lib/comlibc/common #
endif

KERNELPATHS += -I$(KERNEL_ROOT)/kernel/include #

