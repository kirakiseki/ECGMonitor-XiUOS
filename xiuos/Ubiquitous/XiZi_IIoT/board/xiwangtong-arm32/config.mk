export CROSS_COMPILE ?=/usr/bin/arm-none-eabi-

export CFLAGS := -mcpu=cortex-m7 -mthumb  -ffunction-sections -fdata-sections -Dgcc -O0 -gdwarf-2 -g -fgnu89-inline -Wa,-mimplicit-it=thumb
export AFLAGS := -c -mcpu=cortex-m7 -mthumb -ffunction-sections -fdata-sections -x assembler-with-cpp -Wa,-mimplicit-it=thumb  -gdwarf-2

### if use USB function, use special lds file because USB uses ITCM

ifeq ($(CONFIG_LIB_MUSLLIB), y)
export LFLAGS += -nostdlib -nostdinc # -fno-builtin -nodefaultlibs
export LIBCC := -lgcc
export LINK_MUSLLIB := $(KERNEL_ROOT)/lib/musllib/libmusl.a
endif

ifeq ($(CONFIG_RESOURCES_LWIP), y)
export LINK_LWIP := $(KERNEL_ROOT)/resources/ethernet/LwIP/liblwip.a
endif

ifeq ($(CONFIG_MCUBOOT_BOOTLOADER),y)
export LFLAGS += -mcpu=cortex-m7 -mthumb -ffunction-sections -fdata-sections -Wl,--gc-sections,-Map=XiZi-xiwangtong-arm32-boot.map,-cref,-u,Reset_Handler -T $(BSP_ROOT)/link-bootloader.lds
else ifeq ($(CONFIG_MCUBOOT_APPLICATION),y)
export LFLAGS += -mcpu=cortex-m7 -mthumb -ffunction-sections -fdata-sections -Wl,--gc-sections,-Map=XiZi-xiwangtong-arm32-app.map,-cref,-u,Reset_Handler -T $(BSP_ROOT)/link-application.lds
else ifeq ($(CONFIG_BSP_USING_USB),y)
export LFLAGS += -mcpu=cortex-m7 -mthumb -ffunction-sections -fdata-sections -Wl,--gc-sections,-Map=XiZi-xiwangtong-arm32.map,-cref,-u,Reset_Handler -T $(BSP_ROOT)/link-usb.lds
else
export LFLAGS += -mcpu=cortex-m7 -mthumb -ffunction-sections -fdata-sections -Wl,--gc-sections,-Map=XiZi-xiwangtong-arm32.map,-cref,-u,Reset_Handler -T $(BSP_ROOT)/link.lds
endif

export CXXFLAGS := -mcpu=cortex-m7 -mthumb -ffunction-sections -fdata-sections -Dgcc -O0 -gdwarf-2 -g

export APPLFLAGS := -mcpu=cortex-m7 -mthumb -ffunction-sections -fdata-sections -Wl,--gc-sections,-Map=XiZi-app.map,-cref,-u, -T $(BSP_ROOT)/link_userspace.lds


export DEFINES := -DHAVE_CCONFIG_H  -DCPU_MIMXRT1052CVL5B -DSKIP_SYSCLK_INIT -DEVK_MCIMXRM -DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 -DXIP_EXTERNAL_FLASH=1 -D__STARTUP_INITIALIZE_NONCACHEDATA -D__STARTUP_CLEAR_BSS

export ARCH = arm
export MCU = cortex-m7
