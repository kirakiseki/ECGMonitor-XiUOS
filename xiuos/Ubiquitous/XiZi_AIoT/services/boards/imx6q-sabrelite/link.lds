

STACK_SIZE =  48*1024;
L1_PAGE_TABLE_SIZE = 16K;
RAM_VECTORS_SIZE = 72;

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
    OCRAM (rwx) : ORIGIN = 0x00900000, LENGTH = 256K
    DDR (rwx)   : ORIGIN = 0x10000000, LENGTH = 2048M
}

SECTIONS
{
/*
 * -- OCRAM --
 *
 * Nothing in OCRAM can be loaded at boot, because the boot image must be a contiguous
 * region of memory.
 */
    /* MMU L1 page table */
    .l1_page_table (NOLOAD) :
    {
        __l1_page_table_start = .;
        . += L1_PAGE_TABLE_SIZE;
    } > OCRAM

    /* allocate a heap in ocram */
    .heap.ocram (NOLOAD) : ALIGN(4)
    {
        __heap_ocram_start = .;
        . += LENGTH(OCRAM) - L1_PAGE_TABLE_SIZE - RAM_VECTORS_SIZE ;
        __heap_ocram_end = .;
    } > OCRAM
    
    /* RAM vector table comes at the end of OCRAM */
    .ram_vectors (ORIGIN(OCRAM) + LENGTH(OCRAM) - RAM_VECTORS_SIZE) (NOLOAD) :
    {
        __ram_vectors_start = .;
        . += RAM_VECTORS_SIZE;
        __ram_vectors_end = .;
    } > OCRAM    

/*
 * -- DDR --
 */

    /* -- read-only sections -- */
    
    _start_image_add = ORIGIN(DDR);
    
    .ivt (ORIGIN(DDR)) :
    {
        . = . + 0x400;
        *(.ivt)
    } > DDR
    
    .boot_data :
    {
        __start_boot_data = .;
        *(.boot_data)
    } > DDR
    
    /* aligned to ease the hexdump read of generated binary */ 
    .dcd_hdr : ALIGN(16)
    {
        __start_dcd = .;
        *(.dcd_hdr)
    } > DDR
    .dcd_wrt_cmd :
    {
        *(.dcd_wrt_cmd)
    } > DDR
    .dcd_data :
    {
        *(.dcd_data)
    } > DDR

    __text_start = .;
    .text :
    {
        *(.startup)
        *(.text)
        *(.text.*)

        /* section information for finsh shell */
        . = ALIGN(4);
        __isrtbl_idx_start = .;
        KEEP(*(.isrtbl.idx))
        __isrtbl_start = .;
        KEEP(*(.isrtbl))
        __isrtbl_end = .;
        . = ALIGN(4);
    } > DDR
    __text_end = .;

    __rodata_start = .;
    .rodata  : 
    {
        *(.rodata) *(.rodata.*) 
    } > DDR
    __rodata_end = .;

    . = ALIGN(4);
    .ctors :
    {
        PROVIDE(__ctors_start__ = .);
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))
        PROVIDE(__ctors_end__ = .);
    } > DDR

    .dtors :
    {
        PROVIDE(__dtors_start__ = .);
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        PROVIDE(__dtors_end__ = .);
    } > DDR

    /* . = ALIGN(16 * 1024);
    .l1_page_table :
    {
        __l1_page_table_start = .;
        . += 16K;
    } */

    . = ALIGN(8);
    __data_start = .;
    .data :
    {
        *(.data)
        *(.data.*)
    } > DDR
    __data_end = .;

    . = ALIGN(8);
    __bss_start = .;
    .bss :
    {
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
    } > DDR
    . = ALIGN(4);
    __bss_end = .;

    .stacks :
    {
        __stacks_start = .;
        . += STACK_SIZE;
        __stacks_end = .;
        stack_top = .;
    } > DDR

    /* Stabs debugging sections.  */
    .stab 0 : { *(.stab) }
    .stabstr 0 : { *(.stabstr) }
    .stab.excl 0 : { *(.stab.excl) }
    .stab.exclstr 0 : { *(.stab.exclstr) }
    .stab.index 0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment 0 : { *(.comment) }

    _end = .;
}
