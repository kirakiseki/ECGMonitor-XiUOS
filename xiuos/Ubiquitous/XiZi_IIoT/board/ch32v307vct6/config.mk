
export CFLAGS :=  -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -g
export AFLAGS :=  -march=rv32imac -mabi=ilp32 -x assembler-with-cpp -ggdb
export LFLAGS :=  -march=rv32imac -mabi=ilp32 -nostartfiles -Wl,--gc-sections,-Map=XiZi-ch32v307vct6.map,-cref,-u,_start -T $(BSP_ROOT)/link.ld

# export CFLAGS :=  -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -g -std=gnu99
# export AFLAGS :=  -march=rv32imac -mabi=ilp32 -x assembler-with-cpp -ggdb
# export LFLAGS :=  -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -g -T $(BSP_ROOT)/link.ld -nostartfiles -Xlinker --gc-sections -Wl,-Map,"XiZi-ch32v307vct6.map" --specs=nano.specs --specs=nosys.specs

export APPLFLAGS :=  -nostartfiles -Wl,--gc-sections,-Map=XiZi-app.map,-cref,-u, -T $(BSP_ROOT)/link_userspace.lds

export CXXFLAGS :=  -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common # -std=gnu99

export CROSS_COMPILE ?=/opt/riscv-embedded-gcc/bin/riscv-none-embed-

export DEFINES := -DHAVE_CCONFIG_H -DHAVE_SIGINFO

export ARCH = risc-v
export MCU =  GH32V307



