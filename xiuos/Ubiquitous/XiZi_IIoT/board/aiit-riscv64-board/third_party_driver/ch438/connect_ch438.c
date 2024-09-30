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
* @file connect_ch438.c
* @brief support to register ch438 pointer and function
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-08-24
*/

#include <connect_ch438.h>
#include <drv_io_config.h>
#include <gpiohs.h>
#include <sleep.h>

static uint8 offset_addr[] = {0x00, 0x10, 0x20, 0x30, 0x08, 0x18, 0x28, 0x38};		/* Offset address of serial port number */
static uint8 interrupt_num[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

static void CH438SetOutput(void)
{
	gpiohs_set_drive_mode(FPIOA_CH438_D0, GPIO_DM_OUTPUT);
	gpiohs_set_drive_mode(FPIOA_CH438_D1, GPIO_DM_OUTPUT);
	gpiohs_set_drive_mode(FPIOA_CH438_D2, GPIO_DM_OUTPUT);
	gpiohs_set_drive_mode(FPIOA_CH438_D3, GPIO_DM_OUTPUT);
	gpiohs_set_drive_mode(FPIOA_CH438_D4, GPIO_DM_OUTPUT);
	gpiohs_set_drive_mode(FPIOA_CH438_D5, GPIO_DM_OUTPUT);
	gpiohs_set_drive_mode(FPIOA_CH438_D6, GPIO_DM_OUTPUT);
	gpiohs_set_drive_mode(FPIOA_CH438_D7, GPIO_DM_OUTPUT);
}

static void CH438SetInput(void)
{
	gpiohs_set_drive_mode(FPIOA_CH438_D0, GPIO_DM_INPUT_PULL_UP);
	gpiohs_set_drive_mode(FPIOA_CH438_D1, GPIO_DM_INPUT_PULL_UP);
	gpiohs_set_drive_mode(FPIOA_CH438_D2, GPIO_DM_INPUT_PULL_UP);
	gpiohs_set_drive_mode(FPIOA_CH438_D3, GPIO_DM_INPUT_PULL_UP);
	gpiohs_set_drive_mode(FPIOA_CH438_D4, GPIO_DM_INPUT_PULL_UP);
	gpiohs_set_drive_mode(FPIOA_CH438_D5, GPIO_DM_INPUT_PULL_UP);
	gpiohs_set_drive_mode(FPIOA_CH438_D6, GPIO_DM_INPUT_PULL_UP);
	gpiohs_set_drive_mode(FPIOA_CH438_D7, GPIO_DM_INPUT_PULL_UP);
}

uint8 ReadCH438Data(uint8 addr)
{
	uint8 dat = 0;

	gpiohs_set_pin(FPIOA_CH438_NWR, GPIO_PV_HIGH);	
	gpiohs_set_pin(FPIOA_CH438_NRD, GPIO_PV_HIGH);	
	gpiohs_set_pin(FPIOA_CH438_ALE, GPIO_PV_HIGH);	

	CH438SetOutput();
	usleep(1);
	
	if(addr &0x80)	gpiohs_set_pin(FPIOA_CH438_D7, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D7, GPIO_PV_LOW);	
	if(addr &0x40)	gpiohs_set_pin(FPIOA_CH438_D6, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D6, GPIO_PV_LOW);	
	if(addr &0x20)	gpiohs_set_pin(FPIOA_CH438_D5, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D5, GPIO_PV_LOW);	
	if(addr &0x10)	gpiohs_set_pin(FPIOA_CH438_D4, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D4, GPIO_PV_LOW);	
	if(addr &0x08)	gpiohs_set_pin(FPIOA_CH438_D3, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D3, GPIO_PV_LOW);	
	if(addr &0x04)	gpiohs_set_pin(FPIOA_CH438_D2, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D2, GPIO_PV_LOW);	
	if(addr &0x02)	gpiohs_set_pin(FPIOA_CH438_D1, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D1, GPIO_PV_LOW);	
	if(addr &0x01)	gpiohs_set_pin(FPIOA_CH438_D0, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D0, GPIO_PV_LOW);	
		
	usleep(1);

	gpiohs_set_pin(FPIOA_CH438_ALE, GPIO_PV_LOW);	

	usleep(1);		

	CH438SetInput();
	usleep(1);
	
	gpiohs_set_pin(FPIOA_CH438_NRD, GPIO_PV_LOW);	
	
	usleep(1);	
	
	dat = 0;
	if (gpiohs_get_pin(FPIOA_CH438_D7))	dat |= 0x80;
	if (gpiohs_get_pin(FPIOA_CH438_D6))	dat |= 0x40;
	if (gpiohs_get_pin(FPIOA_CH438_D5))	dat |= 0x20;
	if (gpiohs_get_pin(FPIOA_CH438_D4))	dat |= 0x10;
	if (gpiohs_get_pin(FPIOA_CH438_D3))	dat |= 0x08;
	if (gpiohs_get_pin(FPIOA_CH438_D2))	dat |= 0x04;
	if (gpiohs_get_pin(FPIOA_CH438_D1))	dat |= 0x02;
	if (gpiohs_get_pin(FPIOA_CH438_D0))	dat |= 0x01;
	
	gpiohs_set_pin(FPIOA_CH438_NRD, GPIO_PV_HIGH);	
	gpiohs_set_pin(FPIOA_CH438_ALE, GPIO_PV_HIGH);	

	usleep(1);

	return dat;
}
	
static void WriteCH438Data(uint8 addr, uint8 dat)
{
	gpiohs_set_pin(FPIOA_CH438_ALE, GPIO_PV_HIGH);	
	gpiohs_set_pin(FPIOA_CH438_NRD, GPIO_PV_HIGH);	
	gpiohs_set_pin(FPIOA_CH438_NWR, GPIO_PV_HIGH);	

	CH438SetOutput();
	usleep(1);	
	
	if(addr & 0x80)	gpiohs_set_pin(FPIOA_CH438_D7, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D7, GPIO_PV_LOW);	
	if(addr & 0x40)	gpiohs_set_pin(FPIOA_CH438_D6, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D6, GPIO_PV_LOW);	
	if(addr & 0x20)	gpiohs_set_pin(FPIOA_CH438_D5, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D5, GPIO_PV_LOW);	
	if(addr & 0x10)	gpiohs_set_pin(FPIOA_CH438_D4, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D4, GPIO_PV_LOW);	
	if(addr & 0x08)	gpiohs_set_pin(FPIOA_CH438_D3, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D3, GPIO_PV_LOW);	
	if(addr & 0x04)	gpiohs_set_pin(FPIOA_CH438_D2, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D2, GPIO_PV_LOW);	
	if(addr & 0x02)	gpiohs_set_pin(FPIOA_CH438_D1, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D1, GPIO_PV_LOW);	
	if(addr & 0x01)	gpiohs_set_pin(FPIOA_CH438_D0, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D0, GPIO_PV_LOW);	
	
	usleep(1);	
	
	gpiohs_set_pin(FPIOA_CH438_ALE, GPIO_PV_LOW);	
	usleep(1);
	
	if(dat & 0x80)	gpiohs_set_pin(FPIOA_CH438_D7, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D7, GPIO_PV_LOW);	
	if(dat & 0x40)	gpiohs_set_pin(FPIOA_CH438_D6, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D6, GPIO_PV_LOW);	
	if(dat & 0x20)	gpiohs_set_pin(FPIOA_CH438_D5, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D5, GPIO_PV_LOW);	
	if(dat & 0x10)	gpiohs_set_pin(FPIOA_CH438_D4, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D4, GPIO_PV_LOW);	
	if(dat & 0x08)	gpiohs_set_pin(FPIOA_CH438_D3, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D3, GPIO_PV_LOW);	
	if(dat & 0x04)	gpiohs_set_pin(FPIOA_CH438_D2, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D2, GPIO_PV_LOW);	
	if(dat & 0x02)	gpiohs_set_pin(FPIOA_CH438_D1, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D1, GPIO_PV_LOW);	
	if(dat & 0x01)	gpiohs_set_pin(FPIOA_CH438_D0, GPIO_PV_HIGH);	else	gpiohs_set_pin(FPIOA_CH438_D0, GPIO_PV_LOW);	
	
	usleep(1);	

	gpiohs_set_pin(FPIOA_CH438_NWR, GPIO_PV_LOW);	

	usleep(1);	
	
	gpiohs_set_pin(FPIOA_CH438_NWR, GPIO_PV_HIGH);	
	gpiohs_set_pin(FPIOA_CH438_ALE, GPIO_PV_HIGH);	
	
	usleep(1);	

	CH438SetInput();

	return;
}

static void WriteCH438Block(uint8 mAddr, uint8 mLen, uint8 *mBuf)   
{
    while (mLen--) {
		WriteCH438Data(mAddr, *mBuf++);
	}
}

void CH438UartSend(uint8 ext_uart_no, uint8 *Data, uint8 Num)
{
	uint8 REG_LSR_ADDR,REG_THR_ADDR;
	
	REG_LSR_ADDR = offset_addr[ext_uart_no] | REG_LSR0_ADDR;
	REG_THR_ADDR = offset_addr[ext_uart_no] | REG_THR0_ADDR;
			
   while (1) {
       while((ReadCH438Data(REG_LSR_ADDR) & BIT_LSR_TEMT) == 0);

       if (Num <= 128) {
           WriteCH438Block(REG_THR_ADDR, Num, Data);
           break;
       } else {
           WriteCH438Block(REG_THR_ADDR, 128, Data);
           Num -= 128;
           Data += 128;
       }
   }
}

uint8 CH438UartRecv(uint8 ext_uart_no, uint8 *buf, x_size_t size)
{
    x_size_t rcv_num = 0;
	uint8 dat = 0;
	uint8 REG_LSR_ADDR, REG_RBR_ADDR;
	uint8 *read_buffer;
	x_size_t buffer_index = 0;
	
	REG_LSR_ADDR = offset_addr[ext_uart_no] | REG_LSR0_ADDR;
	REG_RBR_ADDR = offset_addr[ext_uart_no] | REG_RBR0_ADDR;

	read_buffer = buf;
			
	while ((ReadCH438Data(REG_LSR_ADDR) & BIT_LSR_DATARDY) == 0);

    while (((ReadCH438Data(REG_LSR_ADDR) & BIT_LSR_DATARDY) == 0x01) && (size != 0)) {
		dat = ReadCH438Data(REG_RBR_ADDR);

		*read_buffer = dat;
		read_buffer++;
		buffer_index++;

		if (BUFFSIZE == buffer_index) {
			buffer_index = 0;
			read_buffer = buf;
		}
			
        rcv_num = rcv_num + 1;
		--size;
    }

    return rcv_num;
}

void CH438PortInit(uint8 ext_uart_no, uint32 BaudRate)
{
	uint32 div;
	uint8 DLL,DLM,dlab;
	uint8 REG_LCR_ADDR;
	uint8 REG_DLL_ADDR;
	uint8 REG_DLM_ADDR;
	uint8 REG_IER_ADDR;
	uint8 REG_MCR_ADDR;
	uint8 REG_FCR_ADDR;
	uint8 REG_RBR_ADDR;
	uint8 REG_THR_ADDR;
	uint8 REG_IIR_ADDR;
	
	REG_LCR_ADDR = offset_addr[ext_uart_no] | REG_LCR0_ADDR;
	REG_DLL_ADDR = offset_addr[ext_uart_no] | REG_DLL0_ADDR;
	REG_DLM_ADDR = offset_addr[ext_uart_no] | REG_DLM0_ADDR;
	REG_IER_ADDR = offset_addr[ext_uart_no] | REG_IER0_ADDR;
	REG_MCR_ADDR = offset_addr[ext_uart_no] | REG_MCR0_ADDR;
	REG_FCR_ADDR = offset_addr[ext_uart_no] | REG_FCR0_ADDR;
	REG_RBR_ADDR = offset_addr[ext_uart_no] | REG_RBR0_ADDR;
	REG_THR_ADDR = offset_addr[ext_uart_no] | REG_THR0_ADDR;
	REG_IIR_ADDR = offset_addr[ext_uart_no] | REG_IIR0_ADDR;
			
    WriteCH438Data(REG_IER_ADDR, BIT_IER_RESET);             /* Reset the serial port */
	MdelayKTask(50);
	
	dlab = ReadCH438Data(REG_IER_ADDR);
	dlab &= 0xDF;
	WriteCH438Data(REG_IER_ADDR, dlab);
	
	dlab = ReadCH438Data(REG_LCR_ADDR);
	dlab |= 0x80;
	WriteCH438Data(REG_LCR_ADDR, dlab);

    div = (Fpclk >> 4) / BaudRate;
    DLM = div >> 8;
    DLL = div & 0xff;
	WriteCH438Data(REG_DLL_ADDR, DLL);/* Set baud rate */
    WriteCH438Data(REG_DLM_ADDR, DLM);
	WriteCH438Data(REG_FCR_ADDR, BIT_FCR_RECVTG1 | BIT_FCR_RECVTG0 | BIT_FCR_FIFOEN);/* Set FIFO mode */

    WriteCH438Data(REG_LCR_ADDR, BIT_LCR_WORDSZ1 | BIT_LCR_WORDSZ0);

    WriteCH438Data(REG_IER_ADDR, BIT_IER_IERECV);

    WriteCH438Data(REG_MCR_ADDR, BIT_MCR_OUT2);

	WriteCH438Data(REG_FCR_ADDR, ReadCH438Data(REG_FCR_ADDR) | BIT_FCR_TFIFORST);
}

void CH438PortInitParityCheck(uint8 ext_uart_no, uint32	BaudRate)
{
	uint32 div;
	uint8 DLL,DLM,dlab;
	uint8 REG_LCR_ADDR;
	uint8 REG_DLL_ADDR;
	uint8 REG_DLM_ADDR;
	uint8 REG_IER_ADDR;
	uint8 REG_MCR_ADDR;
	uint8 REG_FCR_ADDR;
	uint8 REG_RBR_ADDR;
	uint8 REG_THR_ADDR;
	uint8 REG_IIR_ADDR;
	
	REG_LCR_ADDR = offset_addr[ext_uart_no] | REG_LCR0_ADDR;
	REG_DLL_ADDR = offset_addr[ext_uart_no] | REG_DLL0_ADDR;
	REG_DLM_ADDR = offset_addr[ext_uart_no] | REG_DLM0_ADDR;
	REG_IER_ADDR = offset_addr[ext_uart_no] | REG_IER0_ADDR;
	REG_MCR_ADDR = offset_addr[ext_uart_no] | REG_MCR0_ADDR;
	REG_FCR_ADDR = offset_addr[ext_uart_no] | REG_FCR0_ADDR;
	REG_RBR_ADDR = offset_addr[ext_uart_no] | REG_RBR0_ADDR;
	REG_THR_ADDR = offset_addr[ext_uart_no] | REG_THR0_ADDR;
	REG_IIR_ADDR = offset_addr[ext_uart_no] | REG_IIR0_ADDR;
			
    WriteCH438Data(REG_IER_ADDR, BIT_IER_RESET);/* Reset the serial port */
	MdelayKTask(50);
	
	dlab = ReadCH438Data(REG_IER_ADDR);
	dlab &= 0xDF;
	WriteCH438Data(REG_IER_ADDR, dlab);
	
	dlab = ReadCH438Data(REG_LCR_ADDR);
	dlab |= 0x80;
	WriteCH438Data(REG_LCR_ADDR, dlab);

    div = (Fpclk >> 4) / BaudRate;
    DLM = div >> 8;
    DLL = div & 0xff;
	WriteCH438Data(REG_DLL_ADDR, DLL);/* Set baud rate */
    WriteCH438Data(REG_DLM_ADDR, DLM);
	WriteCH438Data(REG_FCR_ADDR, BIT_FCR_RECVTG1 |  BIT_FCR_FIFOEN);/* Set FIFO mode */

    WriteCH438Data(REG_LCR_ADDR, BIT_LCR_WORDSZ1 | BIT_LCR_WORDSZ0 | BIT_LCR_PAREN | BIT_LCR_PARMODE0);

    WriteCH438Data(REG_IER_ADDR, BIT_IER_IERECV);

    WriteCH438Data(REG_MCR_ADDR, BIT_MCR_OUT2);

	WriteCH438Data(REG_FCR_ADDR, ReadCH438Data(REG_FCR_ADDR) | BIT_FCR_TFIFORST);
}

void CH438PortDisable(uint8 ext_uart_no, uint32 BaudRate)
{
	uint32 div;
	uint8 DLL,DLM,dlab;
	uint8 REG_LCR_ADDR;
	uint8 REG_DLL_ADDR;
	uint8 REG_DLM_ADDR;
	uint8 REG_IER_ADDR;
	uint8 REG_MCR_ADDR;
	uint8 REG_FCR_ADDR;
	uint8 REG_RBR_ADDR;
	uint8 REG_THR_ADDR;
	uint8 REG_IIR_ADDR;
	
	REG_LCR_ADDR = offset_addr[ext_uart_no] | REG_LCR0_ADDR;
	REG_DLL_ADDR = offset_addr[ext_uart_no] | REG_DLL0_ADDR;
	REG_DLM_ADDR = offset_addr[ext_uart_no] | REG_DLM0_ADDR;
	REG_IER_ADDR = offset_addr[ext_uart_no] | REG_IER0_ADDR;
	REG_MCR_ADDR = offset_addr[ext_uart_no] | REG_MCR0_ADDR;
	REG_FCR_ADDR = offset_addr[ext_uart_no] | REG_FCR0_ADDR;
	REG_RBR_ADDR = offset_addr[ext_uart_no] | REG_RBR0_ADDR;
	REG_THR_ADDR = offset_addr[ext_uart_no] | REG_THR0_ADDR;
	REG_IIR_ADDR = offset_addr[ext_uart_no] | REG_IIR0_ADDR;
			
    WriteCH438Data(REG_IER_ADDR, BIT_IER_RESET);             /* Reset the serial port */
	MdelayKTask(50);
	
	dlab = ReadCH438Data(REG_IER_ADDR);
	dlab &= 0xDF;
	WriteCH438Data(REG_IER_ADDR, dlab);
	
	dlab = ReadCH438Data(REG_LCR_ADDR);
	dlab |= 0x80;
	WriteCH438Data(REG_LCR_ADDR, dlab);

    div = (Fpclk >> 4) / BaudRate;
    DLM = div >> 8;
    DLL = div & 0xff;
	WriteCH438Data(REG_DLL_ADDR, DLL);/* Set baud rate */
    WriteCH438Data(REG_DLM_ADDR, DLM);
	WriteCH438Data(REG_FCR_ADDR, BIT_FCR_RECVTG1 | BIT_FCR_RECVTG0 | BIT_FCR_FIFOEN );/* Set FIFO mode */

    WriteCH438Data(REG_LCR_ADDR, BIT_LCR_WORDSZ1 | BIT_LCR_WORDSZ0);

    WriteCH438Data(REG_IER_ADDR, 0);

    WriteCH438Data(REG_MCR_ADDR, BIT_MCR_OUT2);

	WriteCH438Data(REG_FCR_ADDR, ReadCH438Data(REG_FCR_ADDR) | BIT_FCR_TFIFORST);
}

void CH438Port6Init(uint8 ext_uart_no, uint32 BaudRate)
{
	uint32 div;
	uint8 DLL,DLM,dlab;
	uint8 REG_LCR_ADDR;
	uint8 REG_DLL_ADDR;
	uint8 REG_DLM_ADDR;
	uint8 REG_IER_ADDR;
	uint8 REG_MCR_ADDR;
	uint8 REG_FCR_ADDR;
	uint8 REG_RBR_ADDR;
	uint8 REG_THR_ADDR;
	uint8 REG_IIR_ADDR;
	
	REG_LCR_ADDR = offset_addr[ext_uart_no] | REG_LCR0_ADDR;
	REG_DLL_ADDR = offset_addr[ext_uart_no] | REG_DLL0_ADDR;
	REG_DLM_ADDR = offset_addr[ext_uart_no] | REG_DLM0_ADDR;
	REG_IER_ADDR = offset_addr[ext_uart_no] | REG_IER0_ADDR;
	REG_MCR_ADDR = offset_addr[ext_uart_no] | REG_MCR0_ADDR;
	REG_FCR_ADDR = offset_addr[ext_uart_no] | REG_FCR0_ADDR;
	REG_RBR_ADDR = offset_addr[ext_uart_no] | REG_RBR0_ADDR;
	REG_THR_ADDR = offset_addr[ext_uart_no] | REG_THR0_ADDR;
	REG_IIR_ADDR = offset_addr[ext_uart_no] | REG_IIR0_ADDR;
			
    WriteCH438Data(REG_IER_ADDR, BIT_IER_RESET);/* Reset the serial port */
	MdelayKTask(50);
	
	dlab = ReadCH438Data(REG_IER_ADDR);
	dlab &= 0xDF;
	WriteCH438Data(REG_IER_ADDR, dlab);
	
	dlab = ReadCH438Data(REG_LCR_ADDR);
	dlab |= 0x80;
	WriteCH438Data(REG_LCR_ADDR, dlab);

    //div = ( 22118400 >> 4 ) / BaudRate;
	div = ( 44236800 >> 4 ) / BaudRate;
    DLM = div >> 8;
    DLL = div & 0xff;
	WriteCH438Data(REG_DLL_ADDR, DLL);/* Set baud rate */
    WriteCH438Data(REG_DLM_ADDR, DLM);
	WriteCH438Data(REG_FCR_ADDR, BIT_FCR_RECVTG1 | BIT_FCR_RECVTG0 | BIT_FCR_FIFOEN);/* Set FIFO mode */

    WriteCH438Data(REG_LCR_ADDR, BIT_LCR_WORDSZ1 | BIT_LCR_WORDSZ0);

    WriteCH438Data(REG_IER_ADDR, BIT_IER_IERECV | BIT_IER1_CK2X);

    WriteCH438Data(REG_MCR_ADDR, BIT_MCR_OUT2);

	WriteCH438Data(REG_FCR_ADDR, ReadCH438Data(REG_FCR_ADDR) | BIT_FCR_TFIFORST);
}

static uint32 Ch438Configure(struct SerialCfgParam *ext_serial_cfg)
{
	NULL_PARAM_CHECK(ext_serial_cfg);

	switch (ext_serial_cfg->data_cfg.port_configure)
	{
		case PORT_CFG_INIT:
			CH438PortInit(ext_serial_cfg->data_cfg.ext_uart_no, ext_serial_cfg->data_cfg.serial_baud_rate);
			break;
		case PORT_CFG_PARITY_CHECK:
			CH438PortInitParityCheck(ext_serial_cfg->data_cfg.ext_uart_no, ext_serial_cfg->data_cfg.serial_baud_rate);
			break;
		case PORT_CFG_DISABLE:
			CH438PortDisable(ext_serial_cfg->data_cfg.ext_uart_no, ext_serial_cfg->data_cfg.serial_baud_rate);
			break;		
		case PORT_CFG_DIV:
			CH438Port6Init(ext_serial_cfg->data_cfg.ext_uart_no, ext_serial_cfg->data_cfg.serial_baud_rate);
			break;	
		default:
			KPrintf("Ch438Configure do not support configure %d\n", ext_serial_cfg->data_cfg.port_configure);
			return ERROR;
			break;
	}

	return EOK;
}

static uint32 Ch438Init(struct SerialDriver *serial_drv, struct SerialCfgParam *ext_serial_cfg)
{
	NULL_PARAM_CHECK(serial_drv);

	gpiohs_set_pin(FPIOA_CH438_NWR, GPIO_PV_HIGH);
	gpiohs_set_pin(FPIOA_CH438_NRD, GPIO_PV_HIGH);
	gpiohs_set_pin(FPIOA_CH438_ALE, GPIO_PV_HIGH);

	return Ch438Configure(ext_serial_cfg);
}

static uint32 Ch438DrvConfigure(void *drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(drv);
    NULL_PARAM_CHECK(configure_info);

    x_err_t ret = EOK;

    struct SerialDriver *serial_drv = (struct SerialDriver *)drv;
	struct SerialCfgParam *ext_serial_cfg = (struct SerialCfgParam *)configure_info->private_data;

    switch (configure_info->configure_cmd)
    {
    case OPE_INT:
        ret = Ch438Init(serial_drv, ext_serial_cfg);
        break;
    default:
        break;
    }

    return ret;
}

static uint32 Ch438WriteData(void *dev, struct BusBlockWriteParam *write_param)
{
	NULL_PARAM_CHECK(dev);
	NULL_PARAM_CHECK(write_param);

	struct SerialHardwareDevice *serial_dev = (struct SerialHardwareDevice *)dev;
	struct SerialDevParam *dev_param = (struct SerialDevParam *)serial_dev->haldev.private_data;

	CH438UartSend(dev_param->ext_uart_no, (uint8 *)write_param->buffer, write_param->size);

	return EOK;
}

static uint32 Ch438ReadData(void *dev, struct BusBlockReadParam *read_param)
{
	NULL_PARAM_CHECK(dev);
	NULL_PARAM_CHECK(read_param);

	uint32 rcv_cnt = 0;
	uint8 rcv_num = 0;
	uint8 gInterruptStatus;
	uint8 InterruptStatus;
	static uint8 dat;
	uint8 REG_LCR_ADDR;
	uint8 REG_DLL_ADDR;
	uint8 REG_DLM_ADDR;
	uint8 REG_IER_ADDR;
	uint8 REG_MCR_ADDR;
	uint8 REG_FCR_ADDR;
	uint8 REG_RBR_ADDR;
	uint8 REG_THR_ADDR;
	uint8 REG_IIR_ADDR;
	uint8 REG_LSR_ADDR;
	uint8 REG_MSR_ADDR;

	struct SerialHardwareDevice *serial_dev = (struct SerialHardwareDevice *)dev;
	struct SerialDevParam *dev_param = (struct SerialDevParam *)serial_dev->private_data;

	do {
		rcv_cnt++;

		if (rcv_cnt > CH438_RECV_TIMEOUT) {
			break;
		}

		dat = ReadCH438Data(REG_IER0_ADDR);
		dat = ReadCH438Data(REG_IIR0_ADDR);
		dat = ReadCH438Data(REG_LCR0_ADDR);
		dat = ReadCH438Data(REG_MCR0_ADDR);
		dat = ReadCH438Data(REG_LSR0_ADDR);
		dat = ReadCH438Data(REG_MSR0_ADDR);
		dat = ReadCH438Data(REG_FCR0_ADDR);
		gInterruptStatus = ReadCH438Data(REG_SSR_ADDR);
		
		if (gInterruptStatus) { 
			if (gInterruptStatus & interrupt_num[dev_param->ext_uart_no]) {   /* Detect which serial port is interrupted */ 
				REG_LCR_ADDR = offset_addr[dev_param->ext_uart_no] | REG_LCR0_ADDR;
				REG_DLL_ADDR = offset_addr[dev_param->ext_uart_no] | REG_DLL0_ADDR;
				REG_DLM_ADDR = offset_addr[dev_param->ext_uart_no] | REG_DLM0_ADDR;
				REG_IER_ADDR = offset_addr[dev_param->ext_uart_no] | REG_IER0_ADDR;
				REG_MCR_ADDR = offset_addr[dev_param->ext_uart_no] | REG_MCR0_ADDR;
				REG_FCR_ADDR = offset_addr[dev_param->ext_uart_no] | REG_FCR0_ADDR;
				REG_RBR_ADDR = offset_addr[dev_param->ext_uart_no] | REG_RBR0_ADDR;
				REG_THR_ADDR = offset_addr[dev_param->ext_uart_no] | REG_THR0_ADDR;
				REG_IIR_ADDR = offset_addr[dev_param->ext_uart_no] | REG_IIR0_ADDR;
				REG_LSR_ADDR = offset_addr[dev_param->ext_uart_no] | REG_LSR0_ADDR;
				REG_MSR_ADDR = offset_addr[dev_param->ext_uart_no] | REG_MSR0_ADDR;

				/* The interrupted state of a read serial port */	
				InterruptStatus = ReadCH438Data(REG_IIR_ADDR) & 0x0f;
							
				switch( InterruptStatus )
				{
					case INT_NOINT:	
						break;
					case INT_THR_EMPTY:					
						break;
					case INT_RCV_OVERTIME:	/* Receiving timeout interruption, triggered when no further data is available four data times after receiving a frame*/
					case INT_RCV_SUCCESS:	/* Interrupts are available to receive data */
						rcv_num = CH438UartRecv(dev_param->ext_uart_no, (uint8 *)read_param->buffer, read_param->size);
						read_param->read_length = rcv_num;
						break;
					case INT_RCV_LINES:		/* Receiving line status interrupted */
						ReadCH438Data(REG_LSR_ADDR);
						break;
					case INT_MODEM_CHANGE:	/* MODEM input changes interrupt */
						ReadCH438Data(REG_MSR_ADDR);
						break;
					default:
						break;
				}
			}
		}
	} while (0 == (gInterruptStatus & interrupt_num[dev_param->ext_uart_no]));

	return rcv_num;
}

static const struct SerialDevDone dev_done = 
{
    NONE,
    NONE,
    Ch438WriteData,
    Ch438ReadData,
};

static void Ch438InitDefault(struct SerialDriver *serial_drv)
{
	CH438SetOutput();

 	gpiohs_set_drive_mode(FPIOA_CH438_NWR, GPIO_DM_OUTPUT);
	gpiohs_set_drive_mode(FPIOA_CH438_NRD, GPIO_DM_OUTPUT);
	gpiohs_set_drive_mode(FPIOA_CH438_ALE, GPIO_DM_OUTPUT);
	// gpiohs_set_drive_mode(FPIOA_CH438_INT, GPIO_DM_INPUT_PULL_UP);

	// gpiohs_set_drive_mode(FPIOA_CH438_INT, GPIO_DM_INPUT_PULL_UP);
	// gpiohs_set_pin_edge(FPIOA_CH438_INT, GPIO_PE_FALLING);
	// gpiohs_irq_register(FPIOA_CH438_INT, 1, Ch438Irq, NONE);
	
	gpiohs_set_pin(FPIOA_CH438_NWR, GPIO_PV_HIGH);
	gpiohs_set_pin(FPIOA_CH438_NRD, GPIO_PV_HIGH);
	gpiohs_set_pin(FPIOA_CH438_ALE, GPIO_PV_HIGH);
}

static uint32 Ch438DevRegister(struct SerialHardwareDevice *serial_dev, char *dev_name)
{
	x_err_t ret = EOK;
	serial_dev->dev_done = &dev_done;
	serial_dev->ext_serial_mode = RET_TRUE;
    ret = SerialDeviceRegister(serial_dev, NONE, dev_name);
    if (ret != EOK) {
        KPrintf("HwCh438Init Serial device %s register error %d\n", dev_name, ret);
        return ERROR;
    }

    ret = SerialDeviceAttachToBus(dev_name, CH438_BUS_NAME);
    if (ret != EOK) {
        KPrintf("HwCh438Init Serial device %s register error %d\n", dev_name, ret);
        return ERROR;
    }

	return ret;
}

int HwCh438Init(void)
{
    static struct SerialBus serial_bus;
    static struct SerialDriver serial_drv;

    x_err_t ret = EOK;
    
    ret = SerialBusInit(&serial_bus, CH438_BUS_NAME);
    if (ret != EOK) {
        KPrintf("HwCh438Init Serial bus init error %d\n", ret);
        return ERROR;
    }

    serial_drv.configure = &Ch438DrvConfigure;
    ret = SerialDriverInit(&serial_drv, CH438_DRIVER_NAME);
    if (ret != EOK) {
        KPrintf("HwCh438Init Serial driver init error %d\n", ret);
        return ERROR;
    }

    ret = SerialDriverAttachToBus(CH438_DRIVER_NAME, CH438_BUS_NAME);
    if (ret != EOK) {
        KPrintf("HwCh438Init Serial driver attach error %d\n", ret);
        return ERROR;
    }

	static struct SerialHardwareDevice serial_dev_0;
	static struct SerialDevParam dev_param_0;
	dev_param_0.ext_uart_no = 0;
	serial_dev_0.private_data = (void *)&dev_param_0;
	ret = Ch438DevRegister(&serial_dev_0, CH438_DEVICE_NAME_0);
    if (ret != EOK) {
        KPrintf("HwCh438Init Ch438DevRegister error %d\n", ret);
        return ERROR;
    }

	static struct SerialHardwareDevice serial_dev_1;
	static struct SerialDevParam dev_param_1;
	dev_param_1.ext_uart_no = 1;
	serial_dev_1.private_data = (void *)&dev_param_1;
	ret = Ch438DevRegister(&serial_dev_1, CH438_DEVICE_NAME_1);
    if (ret != EOK) {
        KPrintf("HwCh438Init Ch438DevRegister error %d\n", ret);
        return ERROR;
    }

	static struct SerialHardwareDevice serial_dev_2;
	static struct SerialDevParam dev_param_2;
	dev_param_2.ext_uart_no = 2;
	serial_dev_2.private_data = (void *)&dev_param_2;
	ret = Ch438DevRegister(&serial_dev_2, CH438_DEVICE_NAME_2);
    if (ret != EOK) {
        KPrintf("HwCh438Init Ch438DevRegister error %d\n", ret);
        return ERROR;
    }

	static struct SerialHardwareDevice serial_dev_3;
	static struct SerialDevParam dev_param_3;
	dev_param_3.ext_uart_no = 3;
	serial_dev_3.private_data = (void *)&dev_param_3;
	ret = Ch438DevRegister(&serial_dev_3, CH438_DEVICE_NAME_3);
    if (ret != EOK) {
        KPrintf("HwCh438Init Ch438DevRegister error %d\n", ret);
        return ERROR;
    }

	static struct SerialHardwareDevice serial_dev_4;
	static struct SerialDevParam dev_param_4;
	dev_param_4.ext_uart_no = 4;
	serial_dev_4.private_data = (void *)&dev_param_4;
	ret = Ch438DevRegister(&serial_dev_4, CH438_DEVICE_NAME_4);
    if (ret != EOK) {
        KPrintf("HwCh438Init Ch438DevRegister error %d\n", ret);
        return ERROR;
    }

	static struct SerialHardwareDevice serial_dev_5;
	static struct SerialDevParam dev_param_5;
	dev_param_5.ext_uart_no = 5;
	serial_dev_5.private_data = (void *)&dev_param_5;
	ret = Ch438DevRegister(&serial_dev_5, CH438_DEVICE_NAME_5);
    if (ret != EOK) {
        KPrintf("HwCh438Init Ch438DevRegister error %d\n", ret);
        return ERROR;
    }

	static struct SerialHardwareDevice serial_dev_6;
	static struct SerialDevParam dev_param_6;
	dev_param_6.ext_uart_no = 6;
	serial_dev_6.private_data = (void *)&dev_param_6;
	ret = Ch438DevRegister(&serial_dev_6, CH438_DEVICE_NAME_6);
    if (ret != EOK) {
        KPrintf("HwCh438Init Ch438DevRegister error %d\n", ret);
        return ERROR;
    }

	static struct SerialHardwareDevice serial_dev_7;
	static struct SerialDevParam dev_param_7;
	dev_param_7.ext_uart_no = 7;
	serial_dev_7.private_data = (void *)&dev_param_7;
	ret = Ch438DevRegister(&serial_dev_7, CH438_DEVICE_NAME_7);
    if (ret != EOK) {
        KPrintf("HwCh438Init Ch438DevRegister error %d\n", ret);
        return ERROR;
    }

	Ch438InitDefault(&serial_drv);

    return ret;
}
