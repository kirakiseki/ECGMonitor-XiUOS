/*
 * Copyright (c) 2022 AIIT XUOS Lab
 * XiUOS is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *        http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#ifndef snap7_h
#define snap7_h
//---------------------------------------------------------------------------
# include <stdint.h>
# include <unistd.h>

//******************************************************************************
//                                   COMMON
//******************************************************************************
// Exact length types regardless of platform/processor
typedef uint8_t    byte;
typedef uint16_t   word;
typedef uint32_t   longword;
typedef byte* pbyte;
typedef word* pword;
typedef uintptr_t  S7Object; // multi platform/processor object reference
                             // DON'T CONFUSE IT WITH AN OLE OBJECT, IT'S SIMPLY
                             // AN INTEGER VALUE (32 OR 64 BIT) USED AS HANDLE.

#ifndef __cplusplus
typedef struct
{
   int   tm_sec;
   int   tm_min;
   int   tm_hour;
   int   tm_mday;
   int   tm_mon;
   int   tm_year;
   int   tm_wday;
   int   tm_yday;
   int   tm_isdst;
}tm;

typedef int bool;
#define false 0;
#define true  1;
#endif

static const int errLibInvalidParam = -1;
static const int errLibInvalidObject = -2;

// CPU status
#define S7CpuStatusUnknown  0x00
#define S7CpuStatusRun      0x08
#define S7CpuStatusStop     0x04

// ISO Errors
static const longword errIsoConnect = 0x00010000; // Connection error
static const longword errIsoDisconnect = 0x00020000; // Disconnect error
static const longword errIsoInvalidPDU = 0x00030000; // Bad format
static const longword errIsoInvalidDataSize = 0x00040000; // Bad Datasize passed to send/recv buffer is invalid
static const longword errIsoNullPointer = 0x00050000; // Null passed as pointer
static const longword errIsoShortPacket = 0x00060000; // A short packet received
static const longword errIsoTooManyFragments = 0x00070000; // Too many packets without EoT flag
static const longword errIsoPduOverflow = 0x00080000; // The sum of fragments data exceded maximum packet size
static const longword errIsoSendPacket = 0x00090000; // An error occurred during send
static const longword errIsoRecvPacket = 0x000A0000; // An error occurred during recv
static const longword errIsoInvalidParams = 0x000B0000; // Invalid TSAP params
static const longword errIsoResvd_1 = 0x000C0000; // Unassigned
static const longword errIsoResvd_2 = 0x000D0000; // Unassigned
static const longword errIsoResvd_3 = 0x000E0000; // Unassigned
static const longword errIsoResvd_4 = 0x000F0000; // Unassigned

// Tag Struct
typedef struct {
   int Area;
   int DBNumber;
   int Start;
   int Size;
   int WordLen;
}TS7Tag, * PS7Tag;

//------------------------------------------------------------------------------
//                                  PARAMS LIST            
//------------------------------------------------------------------------------
static const int p_u16_LocalPort = 1;
static const int p_u16_RemotePort = 2;
static const int p_i32_PingTimeout = 3;
static const int p_i32_SendTimeout = 4;
static const int p_i32_RecvTimeout = 5;
static const int p_i32_WorkInterval = 6;
static const int p_u16_SrcRef = 7;
static const int p_u16_DstRef = 8;
static const int p_u16_SrcTSap = 9;
static const int p_i32_PDURequest = 10;
static const int p_i32_MaxClients = 11;
static const int p_i32_BSendTimeout = 12;
static const int p_i32_BRecvTimeout = 13;
static const int p_u32_RecoveryTime = 14;
static const int p_u32_KeepAliveTime = 15;

// Client/Partner Job status 
static const int JobComplete = 0;
static const int JobPending = 1;

//******************************************************************************
//                                   CLIENT
//******************************************************************************

// Error codes
static const longword errNegotiatingPDU = 0x00100000;
static const longword errCliInvalidParams = 0x00200000;
static const longword errCliJobPending = 0x00300000;
static const longword errCliTooManyItems = 0x00400000;
static const longword errCliInvalidWordLen = 0x00500000;
static const longword errCliPartialDataWritten = 0x00600000;
static const longword errCliSizeOverPDU = 0x00700000;
static const longword errCliInvalidPlcAnswer = 0x00800000;
static const longword errCliAddressOutOfRange = 0x00900000;
static const longword errCliInvalidTransportSize = 0x00A00000;
static const longword errCliWriteDataSizeMismatch = 0x00B00000;
static const longword errCliItemNotAvailable = 0x00C00000;
static const longword errCliInvalidValue = 0x00D00000;
static const longword errCliCannotStartPLC = 0x00E00000;
static const longword errCliAlreadyRun = 0x00F00000;
static const longword errCliCannotStopPLC = 0x01000000;
static const longword errCliCannotCopyRamToRom = 0x01100000;
static const longword errCliCannotCompress = 0x01200000;
static const longword errCliAlreadyStop = 0x01300000;
static const longword errCliFunNotAvailable = 0x01400000;
static const longword errCliUploadSequenceFailed = 0x01500000;
static const longword errCliInvalidDataSizeRecvd = 0x01600000;
static const longword errCliInvalidBlockType = 0x01700000;
static const longword errCliInvalidBlockNumber = 0x01800000;
static const longword errCliInvalidBlockSize = 0x01900000;
static const longword errCliDownloadSequenceFailed = 0x01A00000;
static const longword errCliInsertRefused = 0x01B00000;
static const longword errCliDeleteRefused = 0x01C00000;
static const longword errCliNeedPassword = 0x01D00000;
static const longword errCliInvalidPassword = 0x01E00000;
static const longword errCliNoPasswordToSetOrClear = 0x01F00000;
static const longword errCliJobTimeout = 0x02000000;
static const longword errCliPartialDataRead = 0x02100000;
static const longword errCliBufferTooSmall = 0x02200000;
static const longword errCliFunctionRefused = 0x02300000;
static const longword errCliDestroying = 0x02400000;
static const longword errCliInvalidParamNumber = 0x02500000;
static const longword errCliCannotChangeParam = 0x02600000;

static const int MaxVars = 20; // Max vars that can be transferred with MultiRead/MultiWrite

// Client Connection Type
static const word CONNTYPE_PG = 0x0001;  // Connect to the PLC as a PG
static const word CONNTYPE_OP = 0x0002;  // Connect to the PLC as an OP
static const word CONNTYPE_BASIC = 0x0003;  // Basic connection

// Area ID
static const byte S7AreaPE = 0x81;
static const byte S7AreaPA = 0x82;
static const byte S7AreaMK = 0x83;
static const byte S7AreaDB = 0x84;
static const byte S7AreaCT = 0x1C;
static const byte S7AreaTM = 0x1D;

// Word Length
static const int S7WLBit = 0x01;
static const int S7WLByte = 0x02;
static const int S7WLWord = 0x04;
static const int S7WLDWord = 0x06;
static const int S7WLReal = 0x08;
static const int S7WLCounter = 0x1C;
static const int S7WLTimer = 0x1D;

// Block type
static const byte Block_OB = 0x38;
static const byte Block_DB = 0x41;
static const byte Block_SDB = 0x42;
static const byte Block_FC = 0x43;
static const byte Block_SFC = 0x44;
static const byte Block_FB = 0x45;
static const byte Block_SFB = 0x46;

// Sub Block Type
static const byte SubBlk_OB = 0x08;
static const byte SubBlk_DB = 0x0A;
static const byte SubBlk_SDB = 0x0B;
static const byte SubBlk_FC = 0x0C;
static const byte SubBlk_SFC = 0x0D;
static const byte SubBlk_FB = 0x0E;
static const byte SubBlk_SFB = 0x0F;

// Block languages
static const byte BlockLangAWL = 0x01;
static const byte BlockLangKOP = 0x02;
static const byte BlockLangFUP = 0x03;
static const byte BlockLangSCL = 0x04;
static const byte BlockLangDB = 0x05;
static const byte BlockLangGRAPH = 0x06;

// Read/Write Multivars
typedef struct {
   int   Area;
   int   WordLen;
   int   Result;
   int   DBNumber;
   int   Start;
   int   Amount;
   void* pdata;
} TS7DataItem, * PS7DataItem;

//typedef int TS7ResultItems[MaxVars];
//typedef TS7ResultItems *PS7ResultItems;

// List Blocks
typedef struct {
   int OBCount;
   int FBCount;
   int FCCount;
   int SFBCount;
   int SFCCount;
   int DBCount;
   int SDBCount;
} TS7BlocksList, * PS7BlocksList;

// Blocks info
typedef struct {
   int BlkType;    // Block Type (OB, DB) 
   int BlkNumber;  // Block number
   int BlkLang;    // Block Language
   int BlkFlags;   // Block flags
   int MC7Size;    // The real size in bytes
   int LoadSize;   // Load memory size
   int LocalData;  // Local data
   int SBBLength;  // SBB Length
   int CheckSum;   // Checksum
   int Version;    // Block version
   // Chars info
   char CodeDate[11]; // Code date
   char IntfDate[11]; // Interface date 
   char Author[9];    // Author
   char Family[9];    // Family
   char Header[9];    // Header
} TS7BlockInfo, * PS7BlockInfo;

typedef word TS7BlocksOfType[0x2000];
typedef TS7BlocksOfType* PS7BlocksOfType;

// Order code
typedef struct {
   char Code[21];
   byte V1;
   byte V2;
   byte V3;
} TS7OrderCode, * PS7OrderCode;

// CPU Info
typedef struct {
   char ModuleTypeName[33];
   char SerialNumber[25];
   char ASName[25];
   char Copyright[27];
   char ModuleName[25];
} TS7CpuInfo, * PS7CpuInfo;

// CP Info
typedef struct {
   int MaxPduLengt;
   int MaxConnections;
   int MaxMpiRate;
   int MaxBusRate;
} TS7CpInfo, * PS7CpInfo;

// See §33.1 of "System Software for S7-300/400 System and Standard Functions"
// and see SFC51 description too
typedef struct {
   word LENTHDR;
   word N_DR;
} SZL_HEADER, * PSZL_HEADER;

typedef struct {
   SZL_HEADER Header;
   byte Data[0x4000 - 4];
} TS7SZL, * PS7SZL;

// SZL List of available SZL IDs : same as SZL but List items are big-endian adjusted
typedef struct {
   SZL_HEADER Header;
   word List[0x2000 - 2];
} TS7SZLList, * PS7SZLList;

// See §33.19 of "System Software for S7-300/400 System and Standard Functions"
typedef struct {
   word  sch_schal;
   word  sch_par;
   word  sch_rel;
   word  bart_sch;
   word  anl_sch;
} TS7Protection, * PS7Protection;

// Client completion callback
typedef void (*pfn_CliCompletion) (void* usrPtr, int opCode, int opResult);
//------------------------------------------------------------------------------
//  Import prototypes
//------------------------------------------------------------------------------
S7Object Cli_Create();
void Cli_Destroy(S7Object* Client);
int Cli_ConnectTo(S7Object Client, const char* Address, int Rack, int Slot);
int Cli_SetConnectionParams(S7Object Client, const char* Address, word LocalTSAP, word RemoteTSAP);
int Cli_SetConnectionType(S7Object Client, word ConnectionType);
int Cli_Connect(S7Object Client);
int Cli_Disconnect(S7Object Client);
int Cli_GetParam(S7Object Client, int ParamNumber, void* pValue);
int Cli_SetParam(S7Object Client, int ParamNumber, void* pValue);
int Cli_SetAsCallback(S7Object Client, pfn_CliCompletion pCompletion, void* usrPtr);
// Data I/O main functions
int Cli_ReadArea(S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, void* pUsrData);
int Cli_WriteArea(S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, void* pUsrData);
int Cli_ReadMultiVars(S7Object Client, PS7DataItem Item, int ItemsCount);
int Cli_WriteMultiVars(S7Object Client, PS7DataItem Item, int ItemsCount);
// Data I/O Lean functions
int Cli_DBRead(S7Object Client, int DBNumber, int Start, int Size, void* pUsrData);
int Cli_DBWrite(S7Object Client, int DBNumber, int Start, int Size, void* pUsrData);
int Cli_MBRead(S7Object Client, int Start, int Size, void* pUsrData);
int Cli_MBWrite(S7Object Client, int Start, int Size, void* pUsrData);
int Cli_EBRead(S7Object Client, int Start, int Size, void* pUsrData);
int Cli_EBWrite(S7Object Client, int Start, int Size, void* pUsrData);
int Cli_ABRead(S7Object Client, int Start, int Size, void* pUsrData);
int Cli_ABWrite(S7Object Client, int Start, int Size, void* pUsrData);
int Cli_TMRead(S7Object Client, int Start, int Amount, void* pUsrData);
int Cli_TMWrite(S7Object Client, int Start, int Amount, void* pUsrData);
int Cli_CTRead(S7Object Client, int Start, int Amount, void* pUsrData);
int Cli_CTWrite(S7Object Client, int Start, int Amount, void* pUsrData);
// Directory functions
int Cli_ListBlocks(S7Object Client, TS7BlocksList* pUsrData);
int Cli_GetAgBlockInfo(S7Object Client, int BlockType, int BlockNum, TS7BlockInfo* pUsrData);
int Cli_GetPgBlockInfo(S7Object Client, void* pBlock, TS7BlockInfo* pUsrData, int Size);
int Cli_ListBlocksOfType(S7Object Client, int BlockType, TS7BlocksOfType* pUsrData, int* ItemsCount);
// Blocks functions
int Cli_Upload(S7Object Client, int BlockType, int BlockNum, void* pUsrData, int* Size);
int Cli_FullUpload(S7Object Client, int BlockType, int BlockNum, void* pUsrData, int* Size);
int Cli_Download(S7Object Client, int BlockNum, void* pUsrData, int Size);
int Cli_Delete(S7Object Client, int BlockType, int BlockNum);
int Cli_DBGet(S7Object Client, int DBNumber, void* pUsrData, int* Size);
int Cli_DBFill(S7Object Client, int DBNumber, int FillChar);
// Date/Time functions
int Cli_GetPlcDateTime(S7Object Client, tm* DateTime);
int Cli_SetPlcDateTime(S7Object Client, tm* DateTime);
int Cli_SetPlcSystemDateTime(S7Object Client);
// System Info functions
int Cli_GetOrderCode(S7Object Client, TS7OrderCode* pUsrData);
int Cli_GetCpuInfo(S7Object Client, TS7CpuInfo* pUsrData);
int Cli_GetCpInfo(S7Object Client, TS7CpInfo* pUsrData);
int Cli_ReadSZL(S7Object Client, int ID, int Index, TS7SZL* pUsrData, int* Size);
int Cli_ReadSZLList(S7Object Client, TS7SZLList* pUsrData, int* ItemsCount);
// Control functions
int Cli_PlcHotStart(S7Object Client);
int Cli_PlcColdStart(S7Object Client);
int Cli_PlcStop(S7Object Client);
int Cli_CopyRamToRom(S7Object Client, int Timeout);
int Cli_Compress(S7Object Client, int Timeout);
int Cli_GetPlcStatus(S7Object Client, int* Status);
// Security functions
int Cli_GetProtection(S7Object Client, TS7Protection* pUsrData);
int Cli_SetSessionPassword(S7Object Client, char* Password);
int Cli_ClearSessionPassword(S7Object Client);
// Low level
int Cli_IsoExchangeBuffer(S7Object Client, void* pUsrData, int* Size);
// Misc
int Cli_GetExecTime(S7Object Client, int* Time);
int Cli_GetLastError(S7Object Client, int* LastError);
int Cli_GetPduLength(S7Object Client, int* Requested, int* Negotiated);
int Cli_ErrorText(int Error, char* Text, int TextLen);
// 1.1.0
int Cli_GetConnected(S7Object Client, int* Connected);
//------------------------------------------------------------------------------
//  Async functions
//------------------------------------------------------------------------------
int Cli_AsReadArea(S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, void* pUsrData);
int Cli_AsWriteArea(S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, void* pUsrData);
int Cli_AsDBRead(S7Object Client, int DBNumber, int Start, int Size, void* pUsrData);
int Cli_AsDBWrite(S7Object Client, int DBNumber, int Start, int Size, void* pUsrData);
int Cli_AsMBRead(S7Object Client, int Start, int Size, void* pUsrData);
int Cli_AsMBWrite(S7Object Client, int Start, int Size, void* pUsrData);
int Cli_AsEBRead(S7Object Client, int Start, int Size, void* pUsrData);
int Cli_AsEBWrite(S7Object Client, int Start, int Size, void* pUsrData);
int Cli_AsABRead(S7Object Client, int Start, int Size, void* pUsrData);
int Cli_AsABWrite(S7Object Client, int Start, int Size, void* pUsrData);
int Cli_AsTMRead(S7Object Client, int Start, int Amount, void* pUsrData);
int Cli_AsTMWrite(S7Object Client, int Start, int Amount, void* pUsrData);
int Cli_AsCTRead(S7Object Client, int Start, int Amount, void* pUsrData);
int Cli_AsCTWrite(S7Object Client, int Start, int Amount, void* pUsrData);
int Cli_AsListBlocksOfType(S7Object Client, int BlockType, TS7BlocksOfType* pUsrData, int* ItemsCount);
int Cli_AsReadSZL(S7Object Client, int ID, int Index, TS7SZL* pUsrData, int* Size);
int Cli_AsReadSZLList(S7Object Client, TS7SZLList* pUsrData, int* ItemsCount);
int Cli_AsUpload(S7Object Client, int BlockType, int BlockNum, void* pUsrData, int* Size);
int Cli_AsFullUpload(S7Object Client, int BlockType, int BlockNum, void* pUsrData, int* Size);
int Cli_AsDownload(S7Object Client, int BlockNum, void* pUsrData, int Size);
int Cli_AsCopyRamToRom(S7Object Client, int Timeout);
int Cli_AsCompress(S7Object Client, int Timeout);
int Cli_AsDBGet(S7Object Client, int DBNumber, void* pUsrData, int* Size);
int Cli_AsDBFill(S7Object Client, int DBNumber, int FillChar);
int Cli_CheckAsCompletion(S7Object Client, int* opResult);
int Cli_WaitAsCompletion(S7Object Client, int Timeout);


#endif // snap7_h
