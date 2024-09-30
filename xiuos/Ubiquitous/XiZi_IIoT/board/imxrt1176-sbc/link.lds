/*
** ###################################################################
**     Processors:          MIMXRT1052CVJ5B
**                          MIMXRT1052CVL5B
**                          MIMXRT1052DVJ6B
**                          MIMXRT1052DVL6B
**
**     Compiler:            GNU C Compiler
**     Reference manual:    IMXRT1050RM Rev.1, 03/2018
**     Version:             rev. 1.0, 2018-09-21
**     Build:               b180921
**
**     Abstract:
**         Linker file for the GNU C Compiler
**
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2018 NXP
**     All rights reserved.
**
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
** ###################################################################
*/


/**
* @file link.lds
* @brief ok1052-c board Linker script
* @version 1.0
* @author AIIT XUOS Lab
* @date 2021-05-28
*/

/*************************************************
File name: link.lds
Description: ok1052-c board Linker script
Others: take MIMXRT1052xxxxx_flexspi_nor.ld for references
History:
1. Date: 2021-05-28
Author: AIIT XUOS Lab
Modification:
1. add shell cmd table and g_service_table
*************************************************/

/* Entry Point */
ENTRY(Reset_Handler)

STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x2000;

/* Specify the memory areas- */
MEMORY
{
  /* define flash 32MB */
  m_boot_data           (RX)  : ORIGIN = 0x30000000, LENGTH = 0x00001000
  m_image_vertor_table  (RX)  : ORIGIN = 0x30001000, LENGTH = 0x00001000  
  m_interrupts          (RX)  : ORIGIN = 0x30002000, LENGTH = 0x00001000
  m_text                (RX)  : ORIGIN = 0x30002400, LENGTH = 0x01FFDC00

/* define itcm 256KB */
  sram_itcm_cm7 (rwx) :  ORIGIN = 0x0,        LENGTH = 0x40000 /* 256K bytes (alias RAM2) */

  /* define dtcm 256KB */
  sram_dtcm_cm7 (rwx) :  ORIGIN = 0x20000000, LENGTH = 0x40000 /* 256K bytes (alias RAM) */ 

  /* define ocram1 512KB */  
  sram_oc1 (rwx) :      ORIGIN = 0x20240000, LENGTH = 0x80000 /* 512K bytes (alias RAM3) */

  /* define ocram2 256KB */  
  sram_oc2 (rwx) :      ORIGIN = 0x202c0000, LENGTH = 0x40000 /* 256K bytes (alias RAM4) */

  /* define nocache regin 256KB */ 
  NCACHE_REGION (rwx) : ORIGIN = 0x20300000, LENGTH = 0x40000 /* 256K bytes (alias RAM5) */

  /* define ocram ecc1 and ecc2 64KB */ 
  sram_oc_ecc1 (rwx) :  ORIGIN = 0x20340000, LENGTH = 0x10000 /* 64K bytes (alias RAM6) */ 
  sram_oc_ecc2 (rwx) :  ORIGIN = 0x20350000, LENGTH = 0x10000 /* 64K bytes (alias RAM7) */

  /* define extern sdram 32MB*/
  board_sdram (rwx) :   ORIGIN = 0x80000000, LENGTH = 0x2000000 /* 32M bytes (alias RAM8) */ 

}

/* Define output sections */
SECTIONS
{
  .boot_data :
  {
    KEEP(*(.boot_hdr.conf))
  } > m_boot_data
  
  .image_vertor_table :
  {
    . = ALIGN(4);
    KEEP(*(.boot_hdr.ivt))
    KEEP(*(.boot_hdr.boot_data))
    KEEP(*(.boot_hdr.dcd_data))
    . = ALIGN(4);
  } > m_image_vertor_table

  /* The startup code goes first into internal RAM */
  .interrupts :
  {
    __VECTOR_TABLE = .;
    . = ALIGN(4);
    KEEP(*(.isr_vector))     /* Startup code */
    . = ALIGN(4);
  } > m_interrupts

  __VECTOR_RAM = __VECTOR_TABLE;
  __RAM_VECTOR_TABLE_SIZE_BYTES = 0x0;

  /* The program code and other data goes into internal RAM */
  .text :
  {
    . = ALIGN(4);
    *(.text)                 /* .text sections (code) */
    *(.text*)                /* .text* sections (code) */
    *(.rodata)               /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)              /* .rodata* sections (constants, strings, etc.) */
    *(.glue_7)               /* glue arm to thumb code */
    *(.glue_7t)              /* glue thumb to arm code */
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);


    /* section information for  shell */
    . = ALIGN(4);
    _shell_command_start = .;
    KEEP (*(shellCommand))
    _shell_command_end = .;
    . = ALIGN(4);

    __isrtbl_idx_start = .;
    KEEP(*(.isrtbl.idx))
    __isrtbl_start = .;
    KEEP(*(.isrtbl))
    __isrtbl_end = .;
    . = ALIGN(4);

    PROVIDE(g_service_table_start = ABSOLUTE(.));
    KEEP(*(.g_service_table))
    PROVIDE(g_service_table_end = ABSOLUTE(.));
  } > m_text

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > m_text

  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } > m_text

 .ctors :
  {
    __CTOR_LIST__ = .;
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
       from the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    __CTOR_END__ = .;
  } > m_text

  .dtors :
  {
    __DTOR_LIST__ = .;
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    __DTOR_END__ = .;
  } > m_text

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > m_text

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } > m_text

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > m_text

  __etext = .;    /* define a global symbol at end of code */
  __DATA_ROM = .; /* Symbol is used by startup for data initialization */

  .data : AT(__DATA_ROM)
  {
    . = ALIGN(4);
    __DATA_RAM = .;
    __data_start__ = .;      /* create a global symbol at data start */
    *(m_usb_dma_init_data)
    *(.data)                 /* .data sections */
    *(.data*)                /* .data* sections */
    KEEP(*(.jcr*))
    . = ALIGN(4);
    __data_end__ = .;        /* define a global symbol at data end */
  } > sram_dtcm_cm7

  __NDATA_ROM = __DATA_ROM + (__data_end__ - __data_start__);
  .ncache.init : AT(__NDATA_ROM)
  {
    __noncachedata_start__ = .;   /* create a global symbol at ncache data start */
    __NCACHE_REGION_START = .;
    *(NonCacheable.init)
    . = ALIGN(4);
    __noncachedata_init_end__ = .;   /* create a global symbol at initialized ncache data end */
  } > NCACHE_REGION
  . = __noncachedata_init_end__;
  .ncache :
  {
    *(NonCacheable)
    . = ALIGN(8192);
    __noncachedata_end__ = .;     /* define a global symbol at ncache data end */
    __NCACHE_REGION_END = .;
  } > NCACHE_REGION

  __DATA_END = __NDATA_ROM + (__noncachedata_init_end__ - __noncachedata_start__);
  text_end = ORIGIN(m_text) + LENGTH(m_text);
  ASSERT(__DATA_END <= text_end, "region m_text overflowed with text and data")

  /* Uninitialized data section */
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    . = ALIGN(4);
    __START_BSS = .;
    __bss_start__ = .;
    *(m_usb_dma_noninit_data)
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
    __END_BSS = .;
  } > sram_dtcm_cm7

  .stack :
  {
    . = ALIGN(8);
    stack_start = .;
    . += STACK_SIZE;
    stack_end = .;
    __StackTop = .;
    heap_start = .;
  } > sram_oc1

  PROVIDE(heap_end = ORIGIN(sram_oc1) + LENGTH(sram_oc2) + LENGTH(NCACHE_REGION));

  .ARM.attributes 0 : { *(.ARM.attributes) }
}

