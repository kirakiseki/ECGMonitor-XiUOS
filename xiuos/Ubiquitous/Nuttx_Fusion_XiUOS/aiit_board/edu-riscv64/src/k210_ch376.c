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
 * @file k210_ch376.c
 * @brief edu-riscv64 k210_ch376.c
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.10.10
 */


#if 0
#define DEF_IC_V43_U    1
#endif

#include "k210_ch376.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/
static int fd;

/* Serial port mode is not used */
void xEndCH376Cmd(void)
{
}

void xWriteCH376Cmd(UINT8 cmd)
{
    UINT8 temp[3];
    temp[0] = 0x57;
    temp[1] = 0xab;
    temp[2] = cmd;
    up_udelay(5);
    write(fd, temp, 3);
}

void xWriteCH376Data(UINT8 dat)
{
    UINT8 tmp = dat;
    write(fd, &tmp, 1);
    up_udelay(2);
}

UINT8 xReadCH376Data(void)
{
    UINT32 i;
    UINT8 data;
    int  res;
    for(i=0;i<500000;i++) 
    {
        res = read(fd, &data, 1);
        if(res == 1)
        {
            return ((UINT8)data);
        }
        up_udelay(1);
    }
    return ERR_USB_UNKNOWN;
}

UINT8 CH376ReadBlock(PUINT8 buf)
{
    UINT8 s, l;
    xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
    s = l = xReadCH376Data( );
    if ( l ) {
        do {
            *buf = xReadCH376Data( );
            buf ++;
        } while ( -- l );
    }
    xEndCH376Cmd( );
    return( s );
}

UINT8 Query376Interrupt(void)
{
    //When an interrupt occurs, the serial port will receive a data, read it directly, and discard it
    if(xReadCH376Data() == ERR_USB_UNKNOWN) return FALSE ;
    else return TRUE ;
}

/* CH376 INIT */
UINT8 mInitCH376Host(void)
{
    UINT8 res;
    /* After power on, delay operation for at least 50ms */
    up_mdelay(50);
    fd = open("/dev/ttyS3", O_RDWR);
    up_mdelay(600);
    /* Test the communication interface between SCM and CH376 */
    xWriteCH376Cmd(CMD11_CHECK_EXIST);
    xWriteCH376Data(0x65);
    res = xReadCH376Data();
    xEndCH376Cmd();
    if ( res != 0x9A ) return( ERR_USB_UNKNOWN );

    xWriteCH376Cmd(CMD11_SET_USB_MODE);  /* SET USB MODE */
    xWriteCH376Data(0x06);
    up_udelay(20);
    res = xReadCH376Data();
    xEndCH376Cmd();
    if (res == CMD_RET_SUCCESS) return(USB_INT_SUCCESS);
    else return(ERR_USB_UNKNOWN);  /* SET MODE ERROR */
    
}

/* Write the requested data block to the internally specified buffer, and return the length */
UINT8 CH376WriteReqBlock(PUINT8 buf)
{
    UINT8 s, l;
    xWriteCH376Cmd( CMD01_WR_REQ_DATA );
    s = l = xReadCH376Data();
    if ( l ) {
        do {
            xWriteCH376Data( *buf );
            buf ++;
        } while ( -- l );
    }
    xEndCH376Cmd( );
    return( s );
}

/* Write data block to the send buffer of USB host endpoint */
void CH376WriteHostBlock(PUINT8 buf, UINT8 len)
{
    xWriteCH376Cmd( CMD10_WR_HOST_DATA );
    xWriteCH376Data( len );
    if ( len ) {
        do {
            xWriteCH376Data( *buf );
            buf ++;
        } while ( -- len );
    }
    xEndCH376Cmd( );
}

/* Specify offset address to write data block to internal buffer */
void CH376WriteOfsBlock( PUINT8 buf, UINT8 ofs, UINT8 len )
{
    xWriteCH376Cmd( CMD20_WR_OFS_DATA );
    xWriteCH376Data( ofs );  /* Offset address */
    xWriteCH376Data( len );  /* length */
    if ( len ) {
        do {
            xWriteCH376Data( *buf );
            buf ++;
        } while ( -- len );
    }
    xEndCH376Cmd( );
}

/* Set the file name of the file to be operated on */
void CH376SetFileName( PUINT8 name )
{
    UINT8   c;
#ifndef DEF_IC_V43_U
    UINT8   s;
    xWriteCH376Cmd( CMD01_GET_IC_VER );
    if ( xReadCH376Data( ) < 0x43 ) {
        if ( CH376ReadVar8( VAR_DISK_STATUS ) < DEF_DISK_READY ) {
            xWriteCH376Cmd( CMD10_SET_FILE_NAME );
            xWriteCH376Data( 0 );
            s = CH376SendCmdWaitInt( CMD0H_FILE_OPEN );
            if ( s == USB_INT_SUCCESS ) {
                s = CH376ReadVar8( 0xCF );
                if ( s ) {
                    CH376WriteVar32( 0x4C, CH376ReadVar32( 0x4C ) + ( (UINT16)s << 8 ) );
                    CH376WriteVar32( 0x50, CH376ReadVar32( 0x50 ) + ( (UINT16)s << 8 ) );
                    CH376WriteVar32( 0x70, 0 );
                }
            }
        }
    }
#endif
    xWriteCH376Cmd( CMD10_SET_FILE_NAME );
    c = *name;
    xWriteCH376Data( c );
    while ( c ) {
        name ++;
        c = *name;
        /* Force the file name to expire */
        if ( c == DEF_SEPAR_CHAR1 || c == DEF_SEPAR_CHAR2 ) c = 0;
        xWriteCH376Data( c );
    }
    xEndCH376Cmd( );
}

/* Read 32-bit data from CH376 chip and end the command */
UINT32 CH376Read32bitDat( void )
{
    UINT8 c0, c1, c2, c3;
    c0 = xReadCH376Data( );
    c1 = xReadCH376Data( );
    c2 = xReadCH376Data( );
    c3 = xReadCH376Data( );
    xEndCH376Cmd( );
    return( c0 | (UINT16)c1 << 8 | (UINT32)c2 << 16 | (UINT32)c3 << 24 );
}


UINT8 CH376ReadVar8( UINT8 var )
{
    UINT8 c0;
    xWriteCH376Cmd( CMD11_READ_VAR8 );
    xWriteCH376Data( var );
    c0 = xReadCH376Data( );
    xEndCH376Cmd( );
    return( c0 );
}

void CH376WriteVar8( UINT8 var, UINT8 dat )
{
    xWriteCH376Cmd( CMD20_WRITE_VAR8 );
    xWriteCH376Data( var );
    xWriteCH376Data( dat );
    xEndCH376Cmd( );
}

UINT32 CH376ReadVar32( UINT8 var )
{
    xWriteCH376Cmd( CMD14_READ_VAR32 );
    xWriteCH376Data( var );
    return( CH376Read32bitDat( ) );
}

void CH376WriteVar32( UINT8 var, UINT32 dat )
{
    xWriteCH376Cmd( CMD50_WRITE_VAR32 );
    xWriteCH376Data( var );
    xWriteCH376Data( (UINT8)dat );
    xWriteCH376Data( (UINT8)( (UINT16)dat >> 8 ) );
    xWriteCH376Data( (UINT8)( dat >> 16 ) );
    xWriteCH376Data( (UINT8)( dat >> 24 ) );
    xEndCH376Cmd( );
}

void CH376EndDirInfo( void )
{
    CH376WriteVar8( 0x0D, 0x00 );
}

UINT32 CH376GetFileSize( void )
{
    return( CH376ReadVar32( VAR_FILE_SIZE ) );
}

UINT8   CH376GetDiskStatus( void )
{
    return( CH376ReadVar8( VAR_DISK_STATUS ) );
}

UINT8 CH376GetIntStatus( void )
{
    UINT8 s;
    xWriteCH376Cmd( CMD01_GET_STATUS );
    s = xReadCH376Data( );
    xEndCH376Cmd( );
    return( s );
}

#ifndef NO_DEFAULT_CH376_INT
UINT8 Wait376Interrupt( void )
{
#ifdef  DEF_INT_TIMEOUT
#if DEF_INT_TIMEOUT < 1
    while ( Query376Interrupt( ) == FALSE );
    return( CH376GetIntStatus( ) );
#else
    UINT32  i;
    for ( i = 0; i < DEF_INT_TIMEOUT; i ++ ) {
        if ( Query376Interrupt( ) ) return( CH376GetIntStatus( ) );
    }
    return( ERR_USB_UNKNOWN );
#endif
#else
    UINT32  i;
    for ( i = 0; i < 5000000; i ++ ) {
        if ( Query376Interrupt( ) ) return( CH376GetIntStatus( ) );
    }
    return( ERR_USB_UNKNOWN );
#endif
}
#endif

UINT8 CH376SendCmdWaitInt( UINT8 mCmd )
{
    xWriteCH376Cmd( mCmd );
    xEndCH376Cmd( );
    return( Wait376Interrupt( ) );
}

UINT8 CH376SendCmdDatWaitInt( UINT8 mCmd, UINT8 mDat )
{
    xWriteCH376Cmd( mCmd );
    xWriteCH376Data( mDat );
    xEndCH376Cmd( );
    return(Wait376Interrupt());
}

UINT8 CH376DiskReqSense( void )
{
    UINT8 s;
    up_mdelay( 5 );
    s = CH376SendCmdWaitInt( CMD0H_DISK_R_SENSE );
    up_mdelay( 5 );
    return( s );
}

UINT8 CH376DiskConnect( void )
{
    if ( Query376Interrupt( ) ) CH376GetIntStatus( ); 
    return( CH376SendCmdWaitInt( CMD0H_DISK_CONNECT ) );
}

UINT8 CH376DiskMount( void )
{
    return( CH376SendCmdWaitInt( CMD0H_DISK_MOUNT ) );
}

UINT8 CH376FileOpen( PUINT8 name )
{
    CH376SetFileName( name );
#ifndef DEF_IC_V43_U
    if ( name[0] == DEF_SEPAR_CHAR1 || name[0] == DEF_SEPAR_CHAR2 ) CH376WriteVar32( VAR_CURRENT_CLUST, 0 );
#endif
    return( CH376SendCmdWaitInt( CMD0H_FILE_OPEN ) );
}

UINT8 CH376FileCreate( PUINT8 name )
{
    if ( name ) CH376SetFileName( name );
    return( CH376SendCmdWaitInt( CMD0H_FILE_CREATE ) );
}

UINT8 CH376DirCreate( PUINT8 name )
{
    CH376SetFileName( name );
#ifndef DEF_IC_V43_U
    if ( name[0] == DEF_SEPAR_CHAR1 || name[0] == DEF_SEPAR_CHAR2 ) CH376WriteVar32( VAR_CURRENT_CLUST, 0 );
#endif
    return( CH376SendCmdWaitInt( CMD0H_DIR_CREATE ) );
}

UINT8 CH376SeparatePath( PUINT8 path )
{
    PUINT8  pName;
    for ( pName = path; *pName != 0; ++ pName );
    while ( *pName != DEF_SEPAR_CHAR1 && *pName != DEF_SEPAR_CHAR2 && pName != path ) pName --;
    if ( pName != path ) pName ++;
    return( pName - path );
}

UINT8 CH376FileOpenDir( PUINT8 PathName, UINT8 StopName )
{
    UINT8   i, s;
    s = 0;
    i = 1;
    while ( 1 ) {
        while ( PathName[i] != DEF_SEPAR_CHAR1 && PathName[i] != DEF_SEPAR_CHAR2 && PathName[i] != 0 ) ++ i;
        if ( PathName[i] ) i ++;
        else i = 0;
        s = CH376FileOpen( &PathName[s] );
        if ( i && i != StopName ) {
            if ( s != ERR_OPEN_DIR ) {
                if ( s == USB_INT_SUCCESS ) return( ERR_FOUND_NAME );
                else if ( s == ERR_MISS_FILE ) return( ERR_MISS_DIR );
                else return( s );
            }
            s = i;
        }
        else return( s );
    }
}

UINT8 CH376FileOpenPath( PUINT8 PathName )
{
    return( CH376FileOpenDir( PathName, 0xFF ) );
}

UINT8 CH376FileCreatePath( PUINT8 PathName )
{
    UINT8 s;
    UINT8 Name;
    Name = CH376SeparatePath( PathName );
    if ( Name ) {
        s = CH376FileOpenDir( PathName, Name ); 
        if ( s != ERR_OPEN_DIR ) {
            if ( s == USB_INT_SUCCESS ) return( ERR_FOUND_NAME );
            else if ( s == ERR_MISS_FILE ) return( ERR_MISS_DIR );
            else return( s );
        }
    }
    return( CH376FileCreate( &PathName[Name] ) );
}

#ifdef  EN_DIR_CREATE
UINT8 CH376DirCreatePath( PUINT8 PathName )
{
    UINT8 s;
    UINT8 Name;
    UINT8 ClustBuf[4];
    Name = CH376SeparatePath( PathName );
    if ( Name ) {
        s = CH376FileOpenDir( PathName, Name );
        if ( s != ERR_OPEN_DIR ) {
            if ( s == USB_INT_SUCCESS ) return( ERR_FOUND_NAME );
            else if ( s == ERR_MISS_FILE ) return( ERR_MISS_DIR );
            else return( s );
        }
        xWriteCH376Cmd( CMD14_READ_VAR32 );
        xWriteCH376Data( VAR_START_CLUSTER );
        for ( s = 0; s != 4; s ++ ) ClustBuf[ s ] = xReadCH376Data( );
        xEndCH376Cmd( );
        s = CH376DirCreate( &PathName[Name] );
        if ( s != USB_INT_SUCCESS ) return( s );
        CH376WriteVar32( VAR_FILE_SIZE, sizeof(FAT_DIR_INFO) * 2 );
        s = CH376ByteLocate( sizeof(FAT_DIR_INFO) + STRUCT_OFFSET( FAT_DIR_INFO, DIR_FstClusHI ) );
        if ( s != USB_INT_SUCCESS ) return( s );
        s = CH376ByteWrite( &ClustBuf[2], 2, NULL );
        if ( s != USB_INT_SUCCESS ) return( s );
        s = CH376ByteLocate( sizeof(FAT_DIR_INFO) + STRUCT_OFFSET( FAT_DIR_INFO, DIR_FstClusLO ) );
        if ( s != USB_INT_SUCCESS ) return( s );
        s = CH376ByteWrite( ClustBuf, 2, NULL ); 
        if ( s != USB_INT_SUCCESS ) return( s );
        s = CH376ByteLocate( 0 );
        if ( s != USB_INT_SUCCESS ) return( s );
        CH376WriteVar32( VAR_FILE_SIZE, 0 );
        return( s );
    }
    else {
        if ( PathName[0] == DEF_SEPAR_CHAR1 || PathName[0] == DEF_SEPAR_CHAR2 ) return( CH376DirCreate( PathName ) );
        else return( ERR_MISS_DIR ); 
    }
}
#endif

UINT8 CH376FileErase( PUINT8 PathName )
{
    UINT8 s;
    if ( PathName ) {
        for ( s = 1; PathName[s] != DEF_SEPAR_CHAR1 && PathName[s] != DEF_SEPAR_CHAR2 && PathName[s] != 0; ++ s );
        if ( PathName[s] ) {
            s = CH376FileOpenPath( PathName );
            if ( s != USB_INT_SUCCESS && s != ERR_OPEN_DIR ) return( s );
        }
        else CH376SetFileName( PathName );
    }
    return( CH376SendCmdWaitInt( CMD0H_FILE_ERASE ) );
}

UINT8 CH376FileClose( UINT8 UpdateSz )
{
	return( CH376SendCmdDatWaitInt( CMD1H_FILE_CLOSE, UpdateSz ) );
}

UINT8 CH376DirInfoRead( void )
{
    return( CH376SendCmdDatWaitInt( CMD1H_DIR_INFO_READ, 0xFF ) );
}

UINT8   CH376DirInfoSave( void )
{
    return( CH376SendCmdWaitInt( CMD0H_DIR_INFO_SAVE ) );
}

UINT8 CH376ByteLocate( UINT32 offset )
{
    xWriteCH376Cmd( CMD4H_BYTE_LOCATE );
    xWriteCH376Data( (UINT8)offset );
    xWriteCH376Data( (UINT8)((UINT16)offset>>8) );
    xWriteCH376Data( (UINT8)(offset>>16) );
    xWriteCH376Data( (UINT8)(offset>>24) );
    xEndCH376Cmd( );
    return( Wait376Interrupt( ) );
}

UINT8 CH376ByteRead( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount )
{
    UINT8 s;
    xWriteCH376Cmd( CMD2H_BYTE_READ );
    xWriteCH376Data( (UINT8)ReqCount );
    xWriteCH376Data( (UINT8)(ReqCount>>8) );
    xEndCH376Cmd( );
    if ( RealCount ) *RealCount = 0;
    while ( 1 ) {
        s = Wait376Interrupt( );
        if ( s == USB_INT_DISK_READ ) {
            s = CH376ReadBlock( buf );
            xWriteCH376Cmd( CMD0H_BYTE_RD_GO );
            xEndCH376Cmd( );
            buf += s;
            if ( RealCount ) *RealCount += s;
        }
        else return( s );
    }
}

UINT8 CH376ByteWrite( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount )
{
    UINT8   s;
    xWriteCH376Cmd( CMD2H_BYTE_WRITE );
    xWriteCH376Data( (UINT8)ReqCount );
    xWriteCH376Data( (UINT8)(ReqCount>>8) );
    xEndCH376Cmd( );
    if ( RealCount ) *RealCount = 0;
    while ( 1 ) {
        s = Wait376Interrupt( );
        if ( s == USB_INT_DISK_WRITE ) {
			s = CH376WriteReqBlock( buf ); 
			xWriteCH376Cmd( CMD0H_BYTE_WR_GO );
			xEndCH376Cmd( );
			buf += s;
			if ( RealCount ) *RealCount += s;
		}
		else return( s );
	}
}

#ifdef	EN_DISK_QUERY

UINT8 CH376DiskCapacity( PUINT32 DiskCap )
{
    UINT8   s;
    s = CH376SendCmdWaitInt( CMD0H_DISK_CAPACITY );
    if ( s == USB_INT_SUCCESS ) {
        xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
        xReadCH376Data( );
        *DiskCap = CH376Read32bitDat( );
    }
    else *DiskCap = 0;
    return( s );
}

UINT8 CH376DiskQuery( PUINT32 DiskFre )
{
    UINT8   s;
    UINT8   c0, c1, c2, c3;
#ifndef DEF_IC_V43_U
    xWriteCH376Cmd( CMD01_GET_IC_VER );
    if ( xReadCH376Data( ) < 0x43 ) {
        if ( CH376ReadVar8( VAR_DISK_STATUS ) >= DEF_DISK_READY ) CH376WriteVar8( VAR_DISK_STATUS, DEF_DISK_MOUNTED );
    }
#endif
    s = CH376SendCmdWaitInt( CMD0H_DISK_QUERY );
    if ( s == USB_INT_SUCCESS ) {
        xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
        xReadCH376Data( );
        xReadCH376Data( ); 
        xReadCH376Data( );
        xReadCH376Data( );
        xReadCH376Data( );
        c0 = xReadCH376Data( );
        c1 = xReadCH376Data( );
        c2 = xReadCH376Data( );
        c3 = xReadCH376Data( );
        *DiskFre = c0 | (UINT16)c1 << 8 | (UINT32)c2 << 16 | (UINT32)c3 << 24;
        xReadCH376Data( );
        xEndCH376Cmd( );
    }
    else *DiskFre = 0;
    return( s );
}

#endif

UINT8 CH376SecLocate( UINT32 offset )
{
    xWriteCH376Cmd( CMD4H_SEC_LOCATE );
    xWriteCH376Data( (UINT8)offset );
    xWriteCH376Data( (UINT8)((UINT16)offset>>8) );
    xWriteCH376Data( (UINT8)(offset>>16) );
    xWriteCH376Data( 0 );
    xEndCH376Cmd( );
        return( Wait376Interrupt( ) );
}

#ifdef	EN_SECTOR_ACCESS

UINT8 CH376DiskReadSec( PUINT8 buf, UINT32 iLbaStart, UINT8 iSectorCount )

{
    UINT8 s, err;
    UINT16 mBlockCount;
    for ( err = 0; err != 3; ++ err ) {
        xWriteCH376Cmd( CMD5H_DISK_READ );
        xWriteCH376Data( (UINT8)iLbaStart );
        xWriteCH376Data( (UINT8)( (UINT16)iLbaStart >> 8 ) );
        xWriteCH376Data( (UINT8)( iLbaStart >> 16 ) );
        xWriteCH376Data( (UINT8)( iLbaStart >> 24 ) );
        xWriteCH376Data( iSectorCount );
        xEndCH376Cmd( );
        for ( mBlockCount = iSectorCount * DEF_SECTOR_SIZE / CH376_DAT_BLOCK_LEN; mBlockCount != 0; -- mBlockCount ) {
            s = Wait376Interrupt( );
            if ( s == USB_INT_DISK_READ ) {
                s = CH376ReadBlock( buf );
                xWriteCH376Cmd( CMD0H_DISK_RD_GO );
                xEndCH376Cmd( );
                buf += s;
            }
            else break;
        }
        if ( mBlockCount == 0 ) {
            s = Wait376Interrupt( );
            if ( s == USB_INT_SUCCESS ) return( USB_INT_SUCCESS );
        }
        if ( s == USB_INT_DISCONNECT ) return( s );
        CH376DiskReqSense( );
    }
    return( s );
}

UINT8 CH376DiskWriteSec( PUINT8 buf, UINT32 iLbaStart, UINT8 iSectorCount )
{
	UINT8	s, err;
	UINT16	mBlockCount;
	for ( err = 0; err != 3; ++ err ) {
		xWriteCH376Cmd( CMD5H_DISK_WRITE );
		xWriteCH376Data( (UINT8)iLbaStart ); 
		xWriteCH376Data( (UINT8)( (UINT16)iLbaStart >> 8 ) );
		xWriteCH376Data( (UINT8)( iLbaStart >> 16 ) );
		xWriteCH376Data( (UINT8)( iLbaStart >> 24 ) );
		xWriteCH376Data( iSectorCount );
		xEndCH376Cmd( );
		for ( mBlockCount = iSectorCount * DEF_SECTOR_SIZE / CH376_DAT_BLOCK_LEN; mBlockCount != 0; -- mBlockCount ) {
			s = Wait376Interrupt( );
			if ( s == USB_INT_DISK_WRITE ) {
				CH376WriteHostBlock( buf, CH376_DAT_BLOCK_LEN );
				xWriteCH376Cmd( CMD0H_DISK_WR_GO );
				xEndCH376Cmd( );
				buf += CH376_DAT_BLOCK_LEN;
			}
			else break; 
		}
		if ( mBlockCount == 0 ) {
			s = Wait376Interrupt( );
			if ( s == USB_INT_SUCCESS ) return( USB_INT_SUCCESS );
		}
		if ( s == USB_INT_DISCONNECT ) return( s ); 
		CH376DiskReqSense( ); 
	}
	return( s );
}

UINT8 CH376SecRead( PUINT8 buf, UINT8 ReqCount, PUINT8 RealCount )
{
	UINT8	s;
	UINT8	cnt;
	UINT32	StaSec;
#ifndef	DEF_IC_V43_U
	UINT32	fsz, fofs;
#endif
	if ( RealCount ) *RealCount = 0;
	do {
#ifndef	DEF_IC_V43_U
		xWriteCH376Cmd( CMD01_GET_IC_VER );
		cnt = xReadCH376Data( );
		if ( cnt == 0x41 ) {
			xWriteCH376Cmd( CMD14_READ_VAR32 );
			xWriteCH376Data( VAR_FILE_SIZE );
			xReadCH376Data( );
			fsz = xReadCH376Data( );
			fsz |= (UINT16)(xReadCH376Data( )) << 8;
			cnt = xReadCH376Data( );
			fsz |= (UINT32)cnt << 16;
			xWriteCH376Cmd( CMD14_READ_VAR32 );
			xWriteCH376Data( VAR_CURRENT_OFFSET );
			xReadCH376Data( );
			fofs = xReadCH376Data( );
			fofs |= (UINT16)(xReadCH376Data( )) << 8;
			fofs |= (UINT32)(xReadCH376Data( )) << 16;
			if ( fsz >= fofs + 510 ) CH376WriteVar8( VAR_FILE_SIZE + 3, 0xFF );
			else cnt = 0xFF;
		}
		else cnt = 0xFF;
#endif
		xWriteCH376Cmd( CMD1H_SEC_READ );
		xWriteCH376Data( ReqCount );
		xEndCH376Cmd( );
		s = Wait376Interrupt( );
#ifndef	DEF_IC_V43_U
		if ( cnt != 0xFF ) CH376WriteVar8( VAR_FILE_SIZE + 3, cnt );
#endif
		if ( s != USB_INT_SUCCESS ) return( s );
		xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
		xReadCH376Data( );
		cnt = xReadCH376Data( );
		xReadCH376Data( );
		xReadCH376Data( );
		xReadCH376Data( );
		StaSec = CH376Read32bitDat( );
		if ( cnt == 0 ) break;
		s = CH376DiskReadSec( buf, StaSec, cnt );
		if ( s != USB_INT_SUCCESS ) return( s );
		buf += cnt * DEF_SECTOR_SIZE;
		if ( RealCount ) *RealCount += cnt;
		ReqCount -= cnt;
	} while ( ReqCount );
	return( s );
}

UINT8 CH376SecWrite( PUINT8 buf, UINT8 ReqCount, PUINT8 RealCount )
{
	UINT8	s;
	UINT8	cnt;
	UINT32	StaSec;
	if ( RealCount ) *RealCount = 0;
	do {
		xWriteCH376Cmd( CMD1H_SEC_WRITE );
		xWriteCH376Data( ReqCount );
		xEndCH376Cmd( );
		s = Wait376Interrupt( );
		if ( s != USB_INT_SUCCESS ) return( s );
		xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
		xReadCH376Data( );
		cnt = xReadCH376Data( );
		xReadCH376Data( );
		xReadCH376Data( );
		xReadCH376Data( );
		StaSec = CH376Read32bitDat( );
		if ( cnt == 0 ) break;
		s = CH376DiskWriteSec( buf, StaSec, cnt );
		if ( s != USB_INT_SUCCESS ) return( s );
		buf += cnt * DEF_SECTOR_SIZE;
		if ( RealCount ) *RealCount += cnt;
		ReqCount -= cnt;
	} while ( ReqCount );
	return( s );
}

#endif

#ifdef	EN_LONG_NAME

UINT8 CH376LongNameWrite( PUINT8 buf, UINT16 ReqCount )
{
	UINT8	s;
#ifndef	DEF_IC_V43_U
	UINT8	c;
	c = CH376ReadVar8( VAR_DISK_STATUS );
	if ( c == DEF_DISK_OPEN_ROOT ) CH376WriteVar8( VAR_DISK_STATUS, DEF_DISK_OPEN_DIR );
#endif
	xWriteCH376Cmd( CMD2H_BYTE_WRITE );
	xWriteCH376Data( (UINT8)ReqCount );
	xWriteCH376Data( (UINT8)(ReqCount>>8) );
	xEndCH376Cmd( );
	while ( 1 ) {
		s = Wait376Interrupt( );
		if ( s == USB_INT_DISK_WRITE ) {
			if ( buf ) buf += CH376WriteReqBlock( buf );
			else {
				xWriteCH376Cmd( CMD01_WR_REQ_DATA );
				s = xReadCH376Data( );
				while ( s -- ) xWriteCH376Data( 0 );
			}
			xWriteCH376Cmd( CMD0H_BYTE_WR_GO );
			xEndCH376Cmd( );
		}
		else {
#ifndef	DEF_IC_V43_U
			if ( c == DEF_DISK_OPEN_ROOT ) CH376WriteVar8( VAR_DISK_STATUS, c );
#endif
			return( s );
		}
	}
}

UINT8 CH376CheckNameSum( PUINT8 DirName )
{
	UINT8	NameLen;
	UINT8	CheckSum;
	CheckSum = 0;
	for ( NameLen = 0; NameLen != 11; NameLen ++ ) CheckSum = ( CheckSum & 1 ? 0x80 : 0x00 ) + ( CheckSum >> 1 ) + *DirName++;
	return( CheckSum );
}

UINT8 CH376LocateInUpDir( PUINT8 PathName )
{
	UINT8	s;
	xWriteCH376Cmd( CMD14_READ_VAR32 );
	xWriteCH376Data( VAR_FAT_DIR_LBA );
	for ( s = 4; s != 8; s ++ ) GlobalBuf[ s ] = xReadCH376Data( );
	xEndCH376Cmd( );
	s = CH376SeparatePath( PathName );
	if ( s ) s = CH376FileOpenDir( PathName, s );
	else s = CH376FileOpen( "/" ); 
	if ( s != ERR_OPEN_DIR ) return( s );
	*(PUINT32)(&GlobalBuf[0]) = 0;  
	while ( 1 ) {
		s = CH376SecLocate( *(PUINT32)(&GlobalBuf[0]) );
		if ( s != USB_INT_SUCCESS ) return( s );
		CH376ReadBlock( &GlobalBuf[8] ); 
		if ( *(PUINT32)(&GlobalBuf[8]) == *(PUINT32)(&GlobalBuf[4]) ) return( USB_INT_SUCCESS ); 
		xWriteCH376Cmd( CMD50_WRITE_VAR32 );
		xWriteCH376Data( VAR_FAT_DIR_LBA );
		for ( s = 8; s != 12; s ++ ) xWriteCH376Data( GlobalBuf[ s ] );
		xEndCH376Cmd( );
		++ *(PUINT32)(&GlobalBuf[0]);
	}
}

UINT8 CH376GetLongName( PUINT8 PathName, PUINT8 LongName ) 
{
	UINT8 s;
	UINT16 NameCount;
	s = CH376FileOpenPath( PathName );
	if ( s != USB_INT_SUCCESS && s != ERR_OPEN_DIR ) return( s );
	s = CH376DirInfoRead( );
	if ( s != USB_INT_SUCCESS ) return( s );
	CH376ReadBlock( GlobalBuf ); 
	CH376EndDirInfo( );
	GlobalBuf[32] = CH376CheckNameSum( GlobalBuf );
	GlobalBuf[33] = CH376ReadVar8( VAR_FILE_DIR_INDEX );
	NameCount = 0;
	while ( 1 ) {
		if ( GlobalBuf[33] == 0 ) {
			s = CH376LocateInUpDir( PathName );
			if ( s != USB_INT_SUCCESS ) break;
			if ( CH376ReadVar32( VAR_CURRENT_OFFSET ) == 0 ) {
				s = ERR_LONG_NAME_ERR;
				break;
			}
			GlobalBuf[33] = DEF_SECTOR_SIZE / sizeof( FAT_DIR_INFO );
		}
		GlobalBuf[33] --;
		s = CH376SendCmdDatWaitInt( CMD1H_DIR_INFO_READ, GlobalBuf[33] );
		if ( s != USB_INT_SUCCESS ) break;
		CH376ReadBlock( GlobalBuf ); 
		CH376EndDirInfo( ); 
		if ( ( GlobalBuf[11] & ATTR_LONG_NAME_MASK ) != ATTR_LONG_NAME || GlobalBuf[13] != GlobalBuf[32] ) {
			s = ERR_LONG_NAME_ERR;
			break;
		}
		for ( s = 1; s < sizeof( FAT_DIR_INFO ); s += 2 ) {
			if ( s == 1 + 5 * 2 ) s = 14;
			else if ( s == 14 + 6 * 2 ) s = 28;
			LongName[ NameCount++ ] = GlobalBuf[ s ];
			LongName[ NameCount++ ] = GlobalBuf[ s + 1 ];
			if ( GlobalBuf[ s ] == 0 && GlobalBuf[ s + 1 ] == 0 ) break;
			if ( NameCount >= LONG_NAME_BUF_LEN ) {
				s = ERR_LONG_BUF_OVER;
				goto CH376GetLongNameE;
			}
		}
		if ( GlobalBuf[0] & 0x40 ) {
			if ( s >= sizeof( FAT_DIR_INFO ) ) *(PUINT16)( &LongName[ NameCount ] ) = 0x0000;
			s = USB_INT_SUCCESS; 
			break;
		}
	}
CH376GetLongNameE:
	CH376FileClose( FALSE );
	return( s );
}

UINT8 CH376CreateLongName( PUINT8 PathName, PUINT8 LongName )
{
	UINT8	s, i;
	UINT8	DirBlockCnt;
	UINT16	count;		
	UINT16	NameCount;	
	UINT32	NewFileLoc;	
	for ( count = 0; count < LONG_NAME_BUF_LEN; count += 2 ) if ( *(PUINT16)(&LongName[count]) == 0 ) break;  
	if ( count == 0 || count >= LONG_NAME_BUF_LEN || count > LONE_NAME_MAX_CHAR ) return( ERR_LONG_NAME_ERR );
	DirBlockCnt = count / LONG_NAME_PER_DIR;
	i = count - DirBlockCnt * LONG_NAME_PER_DIR;
	if ( i ) {
		if ( ++ DirBlockCnt * LONG_NAME_PER_DIR > LONG_NAME_BUF_LEN ) return( ERR_LONG_BUF_OVER );
		count += 2; 
		i += 2;
		if ( i < LONG_NAME_PER_DIR ) {
			while ( i++ < LONG_NAME_PER_DIR ) LongName[count++] = 0xFF;
		}
	}
	s = CH376FileOpenPath( PathName ); 
	if ( s == USB_INT_SUCCESS ) {
		s = ERR_NAME_EXIST;
		goto CH376CreateLongNameE;
	}
	if ( s != ERR_MISS_FILE ) return( s );
	s = CH376FileCreatePath( PathName );
	if ( s != USB_INT_SUCCESS ) return( s );
	i = CH376ReadVar8( VAR_FILE_DIR_INDEX );
	s = CH376LocateInUpDir( PathName ); 
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE; 
	NewFileLoc = CH376ReadVar32( VAR_CURRENT_OFFSET ) + i * sizeof(FAT_DIR_INFO); 
	s = CH376ByteLocate( NewFileLoc ); 
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	s = CH376ByteRead( &GlobalBuf[ sizeof(FAT_DIR_INFO) ], sizeof(FAT_DIR_INFO), NULL ); 
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	for ( i = DirBlockCnt; i != 0; -- i ) {
		s = CH376ByteRead( GlobalBuf, sizeof(FAT_DIR_INFO), &count );
		if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
		if ( count == 0 ) break; 
		if ( GlobalBuf[0] && GlobalBuf[0] != 0xE5 ) {
			s = CH376ByteLocate( NewFileLoc );
			if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
			GlobalBuf[ 0 ] = 0xE5;
			for ( s = 1; s != sizeof(FAT_DIR_INFO); s ++ ) GlobalBuf[ s ] = GlobalBuf[ sizeof(FAT_DIR_INFO) + s ];
			s = CH376LongNameWrite( GlobalBuf, sizeof(FAT_DIR_INFO) );
			if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
			do {
				s = CH376ByteRead( GlobalBuf, sizeof(FAT_DIR_INFO), &count );
				if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
			} while ( count && GlobalBuf[0] ); 
			NewFileLoc = CH376ReadVar32( VAR_CURRENT_OFFSET ); 
			i = DirBlockCnt + 1;
			if ( count == 0 ) break; 
			NewFileLoc -= sizeof(FAT_DIR_INFO); 
		}
	}
	if ( i ) {
		s = CH376ReadVar8( VAR_SEC_PER_CLUS );
		if ( s == 128 ) {
			s = ERR_FDT_OVER; 
			goto CH376CreateLongNameE;
		}
		count = s * DEF_SECTOR_SIZE;
		if ( count < i * sizeof(FAT_DIR_INFO) ) count <<= 1;
		s = CH376LongNameWrite( NULL, count );
		if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	}
	s = CH376ByteLocate( NewFileLoc );
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	GlobalBuf[11] = ATTR_LONG_NAME;
	GlobalBuf[12] = 0x00;
	GlobalBuf[13] = CH376CheckNameSum( &GlobalBuf[ sizeof(FAT_DIR_INFO) ] ); 
	GlobalBuf[26] = 0x00;
	GlobalBuf[27] = 0x00;
	for ( s = 0; DirBlockCnt != 0; ) {
		GlobalBuf[0] = s ? DirBlockCnt : DirBlockCnt | 0x40; 
		DirBlockCnt --;
		NameCount = DirBlockCnt * LONG_NAME_PER_DIR;
		for ( s = 1; s < sizeof( FAT_DIR_INFO ); s += 2 ) {
			if ( s == 1 + 5 * 2 ) s = 14; 
			else if ( s == 14 + 6 * 2 ) s = 28;
			GlobalBuf[ s ] = LongName[ NameCount++ ];
			GlobalBuf[ s + 1 ] = LongName[ NameCount++ ];
		}
		s = CH376LongNameWrite( GlobalBuf, sizeof(FAT_DIR_INFO) );
		if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	}
	s = CH376LongNameWrite( &GlobalBuf[ sizeof(FAT_DIR_INFO) ], sizeof(FAT_DIR_INFO) ); 
CH376CreateLongNameE:
	CH376FileClose( FALSE );
	return( s );
}

#endif
