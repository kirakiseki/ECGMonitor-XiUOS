/*
* Copyright (c) 2020 AIIT XUOS Lab
* XiUOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/

/**
 * @file ch376_demo.c
 * @brief edu-riscv64 ch376_demo.c
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.10.11
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "k210_ch376.h"

uint8_t 	buf[64];

void CH376Demo(void)
{
    uint8_t s;
	s = mInitCH376Host();
	printf ("ch376 init stat=0x%02x\n",(uint16_t)s);

    printf( "Wait Udisk/SD\n" );
    while ( CH376DiskConnect( ) != USB_INT_SUCCESS ) 
    {
        up_mdelay( 100 );
    }
    
    for ( s = 0; s < 10; s ++ ) 
    {
        up_mdelay( 50 );
        printf( "Ready ?\n" );
        if ( CH376DiskMount( ) == USB_INT_SUCCESS ) break;
    }
    s = CH376ReadBlock( buf );
    if ( s == sizeof( INQUIRY_DATA ) ) 
    {
        buf[ s ] = 0;
        printf( "UdiskInfo: %s\n", ((P_INQUIRY_DATA)buf) -> VendorIdStr );
    }

    printf( "Create /YEAR2022/DEMO2022.TXT \n" );
    s = CH376DirCreate((PUINT8)"/YEAR2022" );
    printf("CH376DirCreate:0x%02x\n",(uint16_t)s );
    
    s = CH376FileCreatePath((PUINT8)"/YEAR2022/DEMO2022.TXT" );
    printf( "CH376FileCreatePath:0x%02x\n",(uint16_t)s );
    
    printf( "Write some data to file\n" );
    strcpy( (char *)buf, "This is test case!\xd\xa" );
    s = CH376ByteWrite(buf, strlen((char *)buf), NULL );
    printf( "CH376ByteWrite:0x%02x\n",(uint16_t)s );
    
    printf( "Close\n" );
    s = CH376FileClose( TRUE );
    printf( "CH376FileClose:0x%02x\n",(uint16_t)s );	
}
