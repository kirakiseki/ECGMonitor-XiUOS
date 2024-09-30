export CROSS_COMPILE ?=/usr/bin/arm-none-eabi-
export DEVICE = -march=armv7-a -mtune=cortex-a9 -mfpu=vfpv3-d16 -ftree-vectorize -ffast-math -mfloat-abi=softfp
export CFLAGS := $(DEVICE) -Wall -O0 -g -gdwarf-2
export AFLAGS := -c $(DEVICE) -x assembler-with-cpp -D__ASSEMBLY__ -gdwarf-2
export LFLAGS := $(DEVICE) -Wl,--gc-sections,-Map=XiZi-imx6q-sabrelite.map,-cref,-u,ExceptionVectors -T $(BSP_ROOT)/link.lds
export CXXFLAGS := 

ifeq ($(CONFIG_LIB_MUSLLIB), y)
export LFLAGS += -nostdlib -nostdinc -fno-builtin -nodefaultlibs
export LIBCC := -lgcc
export LINK_MUSLLIB := $(KERNEL_ROOT)/lib/musllib/libmusl.a
endif

# ifeq ($(CONFIG_RESOURCES_LWIP), y)
# export LINK_LWIP := $(KERNEL_ROOT)/resources/ethernet/LwIP/liblwip.a
# endif

export DEFINES := -DHAVE_CCONFIG_H -DCHIP_MX6DQ

export USING_NEWLIB =1
export USING_VFS = 1
export ARCH = arm
export ARCH_ARMV = armv7-a
