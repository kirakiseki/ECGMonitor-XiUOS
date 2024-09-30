export CFLAGS := -mcmodel=medany -march=rv64imafdc -mabi=lp64d -fno-common -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -O0 -ggdb -fgnu89-inline  
# $(warning, "DEBUG, here add cflags.")
# export CFLAGS += -nostdlib -nostdinc -fno-builtin
export AFLAGS := -c -mcmodel=medany -march=rv64imafdc -mabi=lp64d -x assembler-with-cpp -ggdb
export LFLAGS := -mcmodel=medany -march=rv64imafdc -mabi=lp64d -nostartfiles -Wl,--gc-sections,-Map=XiZi-kd233.map,-cref,-u,_start -T $(BSP_ROOT)/link.lds

export APPLFLAGS := -mcmodel=medany -march=rv64imafdc -mabi=lp64d -nostartfiles -Wl,--gc-sections,-Map=XiZi-app.map,-cref,-u, -T $(BSP_ROOT)/link_userspace.lds

ifeq ($(CONFIG_LIB_MUSLLIB), y)
export LFLAGS += -nostdlib -nostdinc -fno-builtin -nodefaultlibs
export LIBCC := -lgcc
export LINK_MUSLLIB := $(KERNEL_ROOT)/lib/musllib/libmusl.a
endif

ifeq ($(CONFIG_RESOURCES_LWIP), y)
export LINK_LWIP := $(KERNEL_ROOT)/resources/ethernet/LwIP/liblwip.a
endif

export CXXFLAGS := -mcmodel=medany -march=rv64imafdc -mabi=lp64d -fno-common -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -O0 -ggdb 

export CROSS_COMPILE ?=/opt/gnu-mcu-eclipse/riscv-none-gcc/8.2.0-2.1-20190425-1021/bin/riscv-none-embed-


export DEFINES := -DHAVE_CCONFIG_H -DHAVE_SIGINFO

export ARCH = risc-v
export MCU =  k210


