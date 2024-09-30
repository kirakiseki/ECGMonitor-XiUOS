#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* XIUOS Rt-thread Configuration */

#define ROOT_DIR "../../../.."
#define BSP_DIR "."
#define RT_Thread_DIR "../.."
#define RTT_DIR "../../rt-thread"
#define BOARD_K210_EVB

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_USING_SMP
#define RT_CPUS_NR 2
#define RT_ALIGN_SIZE 8
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 100
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 4096
#define SYSTEM_THREAD_STACK_SIZE 4096
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 2048

/* kservice optimization */

#define RT_DEBUG
#define RT_DEBUG_COLOR
#define RT_DEBUG_INIT_CONFIG
#define RT_DEBUG_INIT 1

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE
#define RT_USING_SIGNALS

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_MEMHEAP
#define RT_USING_SLAB
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uarths"
#define RT_VER_NUM 0x40004
#define ARCH_CPU_64BIT
#define ARCH_RISCV
#define ARCH_RISCV_FPU
#define ARCH_RISCV_FPU_S
#define ARCH_RISCV64

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 8192
#define RT_MAIN_THREAD_PRIORITY 10

/* C++ features */

#define RT_USING_CPLUSPLUS

/* Command shell */

#define RT_USING_FINSH
#define RT_USING_MSH
#define FINSH_USING_MSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 16384
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_CMD_SIZE 80
#define MSH_USING_BUILT_IN_COMMANDS
#define FINSH_USING_DESCRIPTION
#define FINSH_ARG_MAX 10

/* Device virtual file system */

#define RT_USING_DFS
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 16
#define DFS_FILESYSTEM_TYPES_MAX 16
#define DFS_FD_MAX 64
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 437
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_LFN_3
#define RT_DFS_ELM_USE_LFN 3
#define RT_DFS_ELM_LFN_UNICODE_0
#define RT_DFS_ELM_LFN_UNICODE 0
#define RT_DFS_ELM_MAX_LFN 255
#define RT_DFS_ELM_DRIVES 2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 4096
#define RT_DFS_ELM_REENTRANT
#define RT_DFS_ELM_MUTEX_TIMEOUT 3000
#define RT_USING_DFS_DEVFS

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SYSTEM_WORKQUEUE
#define RT_SYSTEM_WORKQUEUE_STACKSIZE 2048
#define RT_SYSTEM_WORKQUEUE_PRIORITY 23
#define RT_USING_SERIAL
#define RT_USING_SERIAL_V1
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 64
#define RT_USING_PIN
#define RT_USING_SPI

/* Using USB */


/* POSIX layer and C standard library */

#define RT_USING_LIBC
#define RT_USING_PTHREADS
#define PTHREAD_NUM_MAX 8
#define RT_USING_POSIX
#define RT_LIBC_USING_TIME
#define RT_LIBC_DEFAULT_TIMEZONE 8

/* Network */

/* Socket abstraction layer */

#define RT_USING_SAL
#define SAL_INTERNET_CHECK
#define SAL_SOCKETS_NUM 16

/* Network interface device */

#define RT_USING_NETDEV
#define NETDEV_USING_IFCONFIG
#define NETDEV_USING_PING
#define NETDEV_USING_NETSTAT
#define NETDEV_USING_AUTO_DEFAULT
#define NETDEV_IPV4 1
#define NETDEV_IPV6 0

/* light weight TCP/IP stack */


/* AT commands */


/* VBUS(Virtual Software BUS) */


/* Utilities */


/* RT-Thread Utestcases */


/* Board Drivers Config */

/* On-chip Peripheral Drivers */

#define __STACKSIZE__ 4096
#define BSP_USING_UART_HS
#define BSP_USING_SPI1
#define BSP_SPI1_CLK_PIN 9
#define BSP_SPI1_D0_PIN 11
#define BSP_SPI1_D1_PIN 10
#define BSP_SPI1_USING_SS0
#define BSP_SPI1_SS0_PIN 12

/* Onboard Peripheral Drivers */


/* Kendryte SDK Config */

#define PKG_KENDRYTE_SDK_VERNUM 0x0055

/* MicroPython */


/* More Drivers */

#define PKG_USING_WIZNET
#define WIZ_USING_W5500

/* WIZnet device configure */

#define WIZ_SPI_DEVICE "spi10"
#define WIZ_RST_PIN 13
#define WIZ_IRQ_PIN 14
#define WIZ_USING_DHCP
#define WIZ_USING_PING
#define PKG_USING_WIZNET_LATEST_VERSION

/* APP_Framework */

/* Framework */

#define TRANSFORM_LAYER_ATTRIUBUTE
#define ADD_RTTHREAD_FEATURES

/* Security */


/* Applications */

/* config stack size and priority of main task */

#define MAIN_KTASK_STACK_SIZE 1024

/* ota app  */


/* test app */


/* connection app */


/* control app */

/* knowing app */


/* sensor app */

#define APPLICATION_SENSOR

/* lib */

#define APP_SELECT_NEWLIB

/* LVGL configuration */


#endif
