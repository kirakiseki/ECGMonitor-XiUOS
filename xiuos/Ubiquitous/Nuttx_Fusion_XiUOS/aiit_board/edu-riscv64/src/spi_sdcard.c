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

/**
* @file soft_spi.h
* @brief edu-riscv64 soft_spi.h
* @version 1.0
* @author AIIT XUOS Lab
* @date 2022-1-3
*/

#include "spi_sdcard.h"

static uint16_t cyc = 64;
static uint8_t SD_Type = 0;

/* When the SD card is initialized, it needs a low speed. */
static void SD_SPI_SpeedLow(void)
{
    cyc = 512;
}

/* When the SD card is working normally, it can be at high speed.*/
static void SD_SPI_SpeedHigh(void)
{
    cyc = 8;
}

/* spi bus delay function */
static void delay(void)
{
    uint16_t i;
    for(i = 0;i < cyc; i++);
}

/* spi io initialization function */
static void SPI_IoInit(void)
{
    /* config simluate SPI bus */
    k210_fpioa_config(SOFT_SPI_CS, HS_GPIO(FPIOA_SOFT_SPI_CS) | K210_IOFLAG_GPIOHS);
    k210_fpioa_config(SOFT_SPI_SCK, HS_GPIO(FPIOA_SOFT_SPI_SCK) | K210_IOFLAG_GPIOHS);
    k210_fpioa_config(SOFT_SPI_MOSI, HS_GPIO(FPIOA_SOFT_SPI_MOSI) | K210_IOFLAG_GPIOHS);
    k210_fpioa_config(SOFT_SPI_MISO, HS_GPIO(FPIOA_SOFT_SPI_MISO) | K210_IOFLAG_GPIOHS);


    k210_gpiohs_set_direction(FPIOA_SOFT_SPI_CS, GPIO_DM_INPUT);
    k210_gpiohs_set_direction(FPIOA_SOFT_SPI_SCK, GPIO_DM_OUTPUT);
    k210_gpiohs_set_direction(FPIOA_SOFT_SPI_MOSI, GPIO_DM_OUTPUT);
    k210_gpiohs_set_direction(FPIOA_SOFT_SPI_MISO, GPIO_DM_OUTPUT);

    k210_gpiohs_set_value(FPIOA_SOFT_SPI_CS, GPIO_PV_HIGH);
    k210_gpiohs_set_value(FPIOA_SOFT_SPI_SCK,  GPIO_PV_LOW);
}

/* spi writes a byte */
static void SoftSpiWriteByte(uint8_t data)
{
    int8_t i = 0;
    uint8_t temp = 0;
    for (i = 0; i < 8; i++)
    {
        temp = ((data & 0x80) == 0x80) ? 1 : 0;
        data = data << 1;
        SPI_SCK_0;
        delay();
        if (temp)
        {
            SPI_MOSI_1;
        }
        else
        {
            SPI_MOSI_0;
        }
        delay();
        SPI_SCK_1;
        delay();  
    }
    SPI_SCK_0;
}

/* spi read a byte */
static uint8_t SoftSpiReadByte(void)
{
    uint8_t i = 0, read_data = 0xFF;
    for (i = 0; i < 8; i++)
    {
        read_data = read_data << 1;
        SPI_SCK_0;
        delay();
        SPI_SCK_1;
        delay();
        if (SPI_READ_MISO)
        {
            read_data |= 0x01;
        }
        delay();
    }

    SPI_SCK_0;
    return read_data;
}



/****************************************************************************
 * Name: SD_DisSelect
 * Description: Deselect, release SPI bus
 * Input: None
 * Output: None
 * return: None
 ****************************************************************************/
void SD_DisSelect(void)
{
    SPI_CS_1;
    SoftSpiWriteByte(0xFF); 
}

/****************************************************************************
 * Name: SD_Select
 * Description: Select the SD card and wait for the SD card to be ready
 * Input: None
 * Output: None
 * return: Return value: 0, success; 1, failure
 ****************************************************************************/
uint8_t SD_Select(void)
{
    SPI_CS_0;
    if (SD_WaitReady() == 0)
    {
        return 0; //waiting for success
    }
  SD_DisSelect();
  return 1; // wait for failure
}

/****************************************************************************
 * Name: SD_WaitReady
 * Description: wait for the card to be ready
 * Input: None
 * Output: None
 * return: 0, ready; 1, not ready
 ****************************************************************************/
uint8_t SD_WaitReady(void)
{
    uint16_t count = 0;
    
    do
    {
        if (SoftSpiReadByte() == 0xFF)
        {
            return 0;
        }
        count++;
    }while (count < 0xFFF0);
    return 1;
}

/****************************************************************************
 * Name: SD_GetResponse
 * Description: Wait for the SD card to respond
 * Input: Response: the response value to get
 * Output: None
 * return: 0, successfully obtained the response value,Others, 
   failed to get the response value
 ****************************************************************************/
uint8_t SD_GetResponse(uint8_t Response)
{
    uint16_t count = 0x1FFF; //Wait times
    while ((SoftSpiReadByte() != Response) && count)
        count--;  //waiting for an accurate response  	  
    if (count == 0)
        return MSD_RESPONSE_FAILURE;  //Response failed
    else
        return MSD_RESPONSE_NO_ERROR; //Respond successfully
}

/****************************************************************************
 * Name: SD_RecvData
 * Description: Read the content of a data packet from the sd card
 * Input: buff:data buffer,len: The length of the data to be read.
 * Output: None
 * return:0, success; 1, failure
 ****************************************************************************/
uint8_t SD_RecvData(uint8_t *buff, uint32_t len)
{
    if (SD_GetResponse(0xFE) != MSD_RESPONSE_NO_ERROR) // Wait for the SD card to send back the data start command 0xFE
        return 1;   //read failure

    while (len--)  //start receiving data
    {
        *buff = SoftSpiReadByte();
        buff++;
    }
    SoftSpiWriteByte(0xFF);
    SoftSpiWriteByte(0xFF);
    return 0;   //read success
}

/****************************************************************************
 * Name: SD_SendBlock
 * Description: Write the content of a data packet to the sd card 512 bytes
 * Input: buff:data buffer,cmd: instruction.
 * Output: None
 * return:0, success; others, failure
 ****************************************************************************/
uint8_t SD_SendBlock(uint8_t *buff, uint8_t cmd)
{   
    uint8_t retval;
    if (SD_WaitReady() == 1)
        return 1;  //wait for readiness to fail
    SoftSpiWriteByte(cmd);
    if (cmd != 0xFD)   //not end command
    {
        uint16_t count;	
        for (count = 0; count < 512; count++)
        SoftSpiWriteByte(buff[count]);   //send data
        SoftSpiWriteByte(0xFF);
        SoftSpiWriteByte(0xFF);
        retval = SoftSpiReadByte();   //receive response
        if ((retval & 0x1F) != MSD_DATA_OK)
            return 2;    //response error		
        /* Delay and wait for the completion of writing the internal data of the SD card. */
        if (SD_WaitReady() == 1)
            return 1;  //wait for readiness to fail
    }
  return 0;
}

/****************************************************************************
 * Name: SD_SendCmd
 * Description: Send a command to the SD card
 * Input: cmd:command,arg:command parameter,crc:CRC check value.
 * Output: None
 * return:the response returned by the SD card;
 ****************************************************************************/
uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t retval;
    uint8_t count = 0xFF; 

    SD_DisSelect();//Cancel the last chip selection
    if (SD_Select() == 1)
        return 0xFF;  //chip select failure 

    SoftSpiWriteByte(cmd | 0x40);   //Write commands separately
    SoftSpiWriteByte(arg >> 24);
    SoftSpiWriteByte(arg >> 16);
    SoftSpiWriteByte(arg >> 8);
    SoftSpiWriteByte(arg);
    SoftSpiWriteByte(crc); 
    if (cmd == TF_CMD12)
        SoftSpiWriteByte(0xFF);  // Skip a stuff byte when stop reading
    do
    {
        retval = SoftSpiReadByte();
    }while ((retval & 0x80) && count--);
    
  return retval;//return status value
}

/****************************************************************************
 * Name: SD_GetCID
 * Description: Get the CID information of the SD card, including manufacturer information
 * Input: cid:memory for storing CID, at least 16Byte
 * Output: None
 * return:0: no error,1:error;
 ****************************************************************************/
uint8_t SD_GetCID(uint8_t *cid)
{
    uint8_t retval;	
    
    retval = SD_SendCmd(TF_CMD10, 0, 0x39);  //send CMD10, read CID
    if (retval == 0x00)
    {
        retval = SD_RecvData(cid, 16);  //receiver 16 bytes data 
    }

    SD_DisSelect();  //Cancel chip selection
    if (retval)
        return 1;
    else 
        return 0;
}

/****************************************************************************
 * Name: SD_GetCSD
 * Description: Get the CSD information of the SD card, including capacity and speed information
 * Input: csd:memory for storing CSD, at least 16Byte
 * Output: None
 * return:0: no error,1:error;
 ****************************************************************************/
uint8_t SD_GetCSD(uint8_t *csd)
{
    uint8_t retval;
    retval = SD_SendCmd(TF_CMD9, 0, 0xAF);  //send CMD10, read CID
    if (retval == 0)
    {
        retval = SD_RecvData(csd, 16);   //receiver 16 bytes data 
    }

    SD_DisSelect();  //Cancel chip selection
    if (retval)
        return 1;
    else 
        return 0;
}

/****************************************************************************
 * Name: SD_GetSectorCount
 * Description: Get the total number of sectors of the SD card (number of sectors)
 * Input: None
 * Output: None
 * return:0: get capacity error,Others: SD card capacity (number of sectors/512 bytes)
 ****************************************************************************/
uint32_t SD_GetSectorCount(void)
{
    uint8_t csd[16];
    uint32_t capacity, csize, n;
    // Get CSD information
    if (SD_GetCSD(csd) != 0)
        return 0;

    //If it is an SDHC card, calculate it as follows
    if((csd[0] & 0xC0) == 0x40)	 //SDV2.0
    {
      csize = csd[9] + ((uint32_t)csd[8] << 8) + ((uint32_t)(csd[7] & 63) << 16) + 1;
        capacity = csize << 9;
    }
    else   //SDV1.0
    {
        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
        csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
        capacity= (uint32_t)csize << (n - 9);  //get sector number   
    }
    capacity = SD_GetCapacity() / 512;
    return capacity;
}

/****************************************************************************
 * Name: SD_GetCapacity
 * Description: Get the capacity of the SD card
 * Input: None
 * Output: None
 * return:SD card capacity
 ****************************************************************************/
uint32_t SD_GetCapacity(void)
{
    uint8_t csd[16];
    uint32_t capacity;
    uint16_t n;
    uint32_t csize; 

    if (SD_GetCSD(csd) != 0)
        return 0;

    if ((csd[0] & 0xC0) == 0x40)
    { 
        csize = csd[9] + ((uint32_t)csd[8] << 8) + ((uint32_t)(csd[7] & 63) << 16) + 1;
        capacity = csize << 9; 
    }
    else
    { 
        n = (csd[5] & 0x0F) + ((csd[10] & 0x80) >> 7) + ((csd[9] & 0x03) << 1) + 2;
        csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 0x03) << 10) + 1;
        capacity = (uint32_t)csize << (n - 10);
    }
    return capacity;
}

/****************************************************************************
 * Name: SD_Card_Init
 * Description: Initialize SD card
 * Input: None
 * Output: None
 * return:SD card capacity
 ****************************************************************************/
uint8_t SD_Card_Init(void)
{
    uint8_t r1;
    uint16_t retry;
    uint8_t buf[4];
    uint16_t i;
 
    SPI_IoInit();
    SPI_MOSI_1;
    SPI_SCK_1;
    SPI_CS_1;
 
   SD_SPI_SpeedLow(); 
   for(i = 0;i < 10;i++)
       SoftSpiWriteByte(0xFF);
 
    retry=20;
    do
    {
        r1=SD_SendCmd(TF_CMD0,0,0x95);
    }while((r1!=0x01) && retry--);

 
   SD_Type = 0;
 
    if(r1 == 0X01)
    {
        if(SD_SendCmd(TF_CMD8,0x1AA,0x87) == 1) //SD V2.0
        {
            for(i =0;i < 4;i++)
            {
                buf[i] = SoftSpiReadByte();
            }
            if(buf[2] == 0X01 && buf[3] == 0XAA)
            {
                retry = 0XFFFE;
                do
                {
                    SD_SendCmd(TF_CMD55,0,0X01);
                    r1 = SD_SendCmd(TF_CMD41,0x40000000,0X01);
                }while(r1 && retry--);

                if(retry && SD_SendCmd(TF_CMD58,0,0X01) == 0)
                {
                    for(i = 0;i < 4;i++)
                        buf[i] = SoftSpiReadByte();
                    if(buf[0] & 0x40)
                        SD_Type = TF_TYPE_SDHC;
                    else
                        SD_Type = TF_TYPE_SDV2;
                }
            }
        }
        else //SDV1.0 MMC V3
        {
            SD_SendCmd(TF_CMD55,0,0X01);         //send CMD55
            r1 = SD_SendCmd(TF_CMD41,0,0X01);    //send CMD41
            if(r1 <= 1)
            {
                SD_Type = TF_TYPE_SDV1;
                retry = 0XFFFE;
                do //wait exit IDLE 
                {
                    SD_SendCmd(TF_CMD55,0,0X01);      //send CMD55
                    r1 = SD_SendCmd(TF_CMD41,0,0X01); //send CMD41
                }while(r1 && retry--);
            }else
            {
                SD_Type = TF_TYPE_MMC; //MMC
                retry = 0XFFFE;
                do //wait exit IDLE 
                {
                    r1 = SD_SendCmd(TF_CMD1,0,0X01);  //send CMD1
                }while(r1&&retry--);
            }
            if(retry == 0 || SD_SendCmd(TF_CMD16,512,0X01) != 0)
                SD_Type = TF_TYPE_ERROR;
        }
    }
 
    SD_DisSelect();
    SD_SPI_SpeedHigh();
 
    if(SD_Type)
        return 0;
    else if(r1)
        return r1;
    return 0xaa;
}
