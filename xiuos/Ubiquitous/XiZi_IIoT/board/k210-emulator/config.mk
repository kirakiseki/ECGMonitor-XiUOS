export CFLAGS := -mcmodel=medany -march=rv64imac -mabi=lp64 -fno-common -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -O0 -Wa,-g -ggdb -fgnu89-inline -Werror
export AFLAGS := -c -mcmodel=medany -march=rv64imac -mabi=lp64  -Wa,-g -ggdb
export LFLAGS := -mcmodel=medany -march=rv64imac -mabi=lp64 -nostartfiles -Wl,--gc-sections,-Map=XiZi-kd233.map,-cref,-u,_start -T $(BSP_ROOT)/link.lds

export APPLFLAGS := -mcmodel=medany -march=rv64imac -mabi=lp64 -nostartfiles -Wl,--gc-sections,-Map=XiZi-app.map,-cref,-u, -T $(BSP_ROOT)/link_userspace.lds


export CXXFLAGS := -mcmodel=medany -march=rv64imac -mabi=lp64 -fno-common -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -O0 -Wa,-g -ggdb -Werror

export CROSS_COMPILE ?=/opt/gnu-mcu-eclipse/riscv-none-gcc/8.2.0-2.1-20190425-1021/bin/riscv-none-embed-

export DEFINES := -DHAVE_CCONFIG_H -DHAVE_SIGINFO

export ARCH = risc-v
export MCU =  k210


