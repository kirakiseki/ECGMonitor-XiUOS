/*
* Copyright (c) 2020 AIIT XUOS Lab
* XiOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/

/**
 * @file .c
 * @brief imxrt board sd card automount
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.06.30
 */

#include "ch438.h"
#include <math.h>

#ifdef BSP_USING_CH438

#define RT (0U)      // If config this macro ,we can use rt gpio for ch438 us

/* This array shows whether the current serial port is selected */
static bool const g_uart_selected[CH438PORTNUM] =
{
#ifdef CONFIG_CH438_EXTUART0
  [0] = true,
#endif
#ifdef CONFIG_CH438_EXTUART1
  [1] = true,
#endif
#ifdef CONFIG_CH438_EXTUART2
  [2] = true,
#endif
#ifdef CONFIG_CH438_EXTUART3
  [3] = true,
#endif
#ifdef CONFIG_CH438_EXTUART4
  [4] = true,
#endif
#ifdef CONFIG_CH438_EXTUART5
  [5] = true,
#endif
#ifdef CONFIG_CH438_EXTUART6
  [6] = true,
#endif
#ifdef CONFIG_CH438_EXTUART7
  [7] = true,
#endif
};


/* rt-thread sem and serial definition */
struct rt_serial_device *extuart_serial_parm[CH438PORTNUM];
static rt_sem_t rx_sem[CH438PORTNUM]={RT_NULL};
char * sem[CH438PORTNUM]={"sem0","sem1","sem2","sem3","sem4","sem5","sem6","sem7"};
/* rt-thread workqueue*/
struct rt_workqueue* rq;
/* there is data available on the corresponding port */
static volatile bool done[CH438PORTNUM] = {false,false,false,false,false,false,false,false};

/* Eight port data buffer */
static uint8_t buff[CH438PORTNUM][CH438_BUFFSIZE];

/* the value of interrupt number of SSR register */
static uint8_t Interruptnum[CH438PORTNUM] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,};

/* Offset address of serial port number */
static uint8_t offsetadd[CH438PORTNUM] = {0x00,0x10,0x20,0x30,0x08,0x18,0x28,0x38,};


const struct rt_uart_ops ch438_ops={
    rt_ch438_configure,
    rt_ch438_control,
    rt_ch438_putc,
    rt_ch438_getc,
    RT_NULL
};


void Config_Interrupt(void){
	   gpio_pin_config_t int_config = {
        kGPIO_DigitalInput,
        0,
        kGPIO_IntFallingEdge,
     };
		 GPIO_PinInit(CH438_CTL_GPIO, CH438_INT_PIN, &int_config);
		  /* Enable GPIO pin interrupt */
     GPIO_PortEnableInterrupts(CH438_CTL_GPIO, 1U << CH438_INT_PIN);
}


static void Ch438Irq(void * parameter)
{
		rt_uint8_t gInterruptStatus;
		rt_uint8_t port = 0;
    struct rt_serial_device *serial = (struct rt_serial_device *)parameter;
		/* multi irq may happen*/
		gInterruptStatus = ReadCH438Data(REG_SSR_ADDR);
    port=CH438_INT_PORT;
    rt_hw_serial_isr(extuart_serial_parm[port], RT_SERIAL_EVENT_RX_IND);
}


static rt_err_t rt_ch438_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
		rt_uint32_t baud_rate = cfg->baud_rate;
		rt_uint16_t port = cfg->reserved;
		CH438PortInit(port, baud_rate);
		return RT_EOK;
}

static rt_err_t rt_ch438_control(struct rt_serial_device *serial, int cmd, void *arg)
{
		rt_uint16_t ext_uart_no = serial->config.reserved;
		static rt_uint16_t register_flag = 0;

    switch (cmd)
    {
			case RT_DEVICE_CTRL_CLR_INT:
			if(1 == register_flag)
			{
				/* Close interrupt of CH438 */
				/* GPIO3_3 INT*/
				  rt_pin_irq_enable(CH438_INT, PIN_IRQ_DISABLE );
					register_flag = 0;
		   }break;
       case RT_DEVICE_CTRL_SET_INT:
		   if(0 == register_flag)
			{ 
				  rt_pin_mode(CH438_INT, PIN_MODE_INPUT_PULLUP);
			    rt_pin_attach_irq(CH438_INT, PIN_IRQ_MODE_FALLING,Ch438Irq,(void *)serial);
				  rt_pin_irq_enable(CH438_INT, PIN_IRQ_ENABLE);
          register_flag = 1;
			}break;
    }
    return (RT_EOK);
}

static int rt_ch438_putc(struct rt_serial_device *serial, char c)
{
	uint16_t ext_uart_no = serial->config.reserved;
	rt_uint8_t	REG_LSR_ADDR,REG_THR_ADDR;
	
	REG_LSR_ADDR = offsetadd[ext_uart_no] | REG_LSR0_ADDR;
	REG_THR_ADDR = offsetadd[ext_uart_no] | REG_THR0_ADDR;
  rt_thread_mdelay(5);
	if((ReadCH438Data( REG_LSR_ADDR ) & BIT_LSR_TEMT) != 0)
	{     

		WriteCH438Block( REG_THR_ADDR, 1, &c );
		return 1;
	} else {
		return 0;
	}
	
}


static int rt_ch438_getc(struct rt_serial_device *serial)
{  

	 rt_uint8_t	dat = 0;
	 rt_uint8_t	REG_LSR_ADDR,REG_RBR_ADDR;
	 uint16_t ext_uart_no = serial->config.reserved;///< get extern uart port
	
	 REG_LSR_ADDR = offsetadd[ext_uart_no] | REG_LSR0_ADDR;
	 REG_RBR_ADDR = offsetadd[ext_uart_no] | REG_RBR0_ADDR;
   rt_thread_mdelay(5);
	 if((ReadCH438Data(REG_LSR_ADDR) & BIT_LSR_DATARDY) == 0x01)
	 {
			dat = ReadCH438Data( REG_RBR_ADDR );
			if(dat >= 0)
			return dat;
		}else{
		return -1;
		}

}


int rt_hw_ch438_init(void)
{
    struct rt_serial_device *extserial;
    struct device_uart      *extuart;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
	  rt_err_t ret;
    #ifdef CONFIG_CH438_EXTUART0
        static struct rt_serial_device  extserial0;

        extserial  = &extserial0;
        extserial->ops              = &ch438_ops;
        extserial->config           = config;
        extserial->config.baud_rate = 115200;
		    extserial->config.reserved  = 0; ///< extern uart port

        extuart_serial_parm[0] = &extserial0;

        ret = rt_hw_serial_register(extserial,
                              "dev0",
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                              extuart);
		if(ret < 0){
			rt_kprintf("extuart_dev0 register failed.\n");
		}
    #endif
	  #ifdef CONFIG_CH438_EXTUART1 
        static struct rt_serial_device  extserial1;

        extserial  = &extserial1;
        extserial->ops              = &ch438_ops;
        extserial->config           = config;
        extserial->config.baud_rate = 115200;
		    extserial->config.reserved = 1; ///< extern uart port

        extuart_serial_parm[1] = &extserial1;

        ret = rt_hw_serial_register(extserial,
                              "dev1",
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                              extuart);
		if(ret < 0){
			rt_kprintf("extuart_dev1 register failed.\n");
		}
    #endif
    #ifdef CONFIG_CH438_EXTUART2
        static struct rt_serial_device  extserial2;

        extserial  = &extserial2;
        extserial->ops              = &ch438_ops;
        extserial->config           = config;
        extserial->config.baud_rate = 9600;
		    extserial->config.reserved = 2; ///< extern uart port

        extuart_serial_parm[2] = &extserial2;

       ret = rt_hw_serial_register(extserial,
                              "dev2",
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                              extuart);
		if(ret < 0){
			rt_kprintf("extuart_dev2 register failed.\n");
		}
	  rt_kprintf("extuart_dev2 register succeed.\n");
    #endif
	  #ifdef CONFIG_CH438_EXTUART3
        static struct rt_serial_device  extserial3;

        extserial  = &extserial3;
        extserial->ops              = &ch438_ops;
        extserial->config           = config;
        extserial->config.baud_rate = 9600;
		    extserial->config.reserved = 3; ///< extern uart port

       ret = rt_hw_serial_register(extserial,
                              "dev3",
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                              extuart);
		if(ret < 0){
			rt_kprintf("extuart_dev3 register failed.\n");
		}

		extuart_serial_parm[3] = &extserial3;
    #endif
    #ifdef CONFIG_CH438_EXTUART4
        static struct rt_serial_device  extserial4;

        extserial  = &extserial4;
        extserial->ops              = &ch438_ops;
        extserial->config           = config;
        extserial->config.baud_rate = 9600;
		extserial->config.reserved = 4; ///< extern uart port

       ret = rt_hw_serial_register(extserial,
                              "dev4",
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                              extuart);
		if(ret < 0){
			rt_kprintf("extuart_dev4 register failed.\n");
		}

		extuart_serial_parm[4] = &extserial4;
    #endif
	  #ifdef CONFIG_CH438_EXTUART5
        static struct rt_serial_device  extserial5;

        extserial  = &extserial5;
        extserial->ops              = &ch438_ops;
        extserial->config           = config;
        extserial->config.baud_rate = 115200;
		extserial->config.reserved = 5; ///< extern uart port

       ret = rt_hw_serial_register(extserial,
                              "dev5",
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                              extuart);
		if(ret < 0){
			rt_kprintf("extuart_dev5 register failed.\n");
		}

		extuart_serial_parm[5] = &extserial5;
    #endif
	  #ifdef CONFIG_CH438_EXTUART6
        static struct rt_serial_device  extserial6;

        extserial  = &extserial6;
        extserial->ops              = &ch438_ops;
        extserial->config           = config;
        extserial->config.baud_rate = 57600;
		extserial->config.reserved = 6; ///< extern uart port

       ret = rt_hw_serial_register(extserial,
                              "dev6",
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                              extuart);
		if(ret < 0){
			rt_kprintf("extuart_dev6 register failed.\n");
		}

		extuart_serial_parm[6] = &extserial6;
    #endif
	  #ifdef CONFIG_CH438_EXTUART7
        static struct rt_serial_device  extserial7;

        extserial  = &extserial7;
        extserial->ops              = &ch438_ops;
        extserial->config           = config;
        extserial->config.baud_rate = 9600;
		    extserial->config.reserved  = 7; ///< extern uart port

        ret = rt_hw_serial_register(extserial,
                              "dev7",
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                              extuart);
		if(ret < 0){
			rt_kprintf("extuart_dev7 register failed.\n");
		}
		rt_kprintf("extuart_dev7 register succeed.\n");
		extuart_serial_parm[7] = &extserial7;
    #endif
    Ch438InitDefault();
		
    return 0;
		
}
INIT_DEVICE_EXPORT(rt_hw_ch438_init);


void up_udelay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < EXAMPLE_DELAY_COUNT; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

void up_mdelay(uint32_t time){

	while(time--){
	   udelay(1000);
	}
}
/****************************************************************************
 * Name: CH438SetOutput
 *
 * Description:
 *   Configure pin mode to output
 *
 ****************************************************************************/
static void CH438SetOutput(void)
{
  gpio_pin_config_t ch438_d0_config ={kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
  GPIO_PinInit(CH438_D_GPIO,CH438_D0_PIN,&ch438_d0_config);
  GPIO_PinInit(CH438_D_GPIO,CH438_D1_PIN,&ch438_d0_config);
	GPIO_PinInit(CH438_D_GPIO,CH438_D2_PIN,&ch438_d0_config);
  GPIO_PinInit(CH438_D_GPIO,CH438_D3_PIN,&ch438_d0_config);
	GPIO_PinInit(CH438_D_GPIO,CH438_D4_PIN,&ch438_d0_config);
  GPIO_PinInit(CH438_D_GPIO,CH438_D5_PIN,&ch438_d0_config);
	GPIO_PinInit(CH438_D_GPIO,CH438_D6_PIN,&ch438_d0_config);
  GPIO_PinInit(CH438_D_GPIO,CH438_D7_PIN,&ch438_d0_config);

}

/****************************************************************************
 * Name: CH438SetInput
 *
 * Description:
 *   Configure pin mode to input
 *
 ****************************************************************************/
static void CH438SetInput(void)
{
	gpio_pin_config_t ch438_d0_config ={kGPIO_DigitalInput, 0, kGPIO_NoIntmode};
	GPIO_PinInit(CH438_D_GPIO,CH438_D0_PIN,&ch438_d0_config);
  GPIO_PinInit(CH438_D_GPIO,CH438_D1_PIN,&ch438_d0_config);
	GPIO_PinInit(CH438_D_GPIO,CH438_D2_PIN,&ch438_d0_config);
  GPIO_PinInit(CH438_D_GPIO,CH438_D3_PIN,&ch438_d0_config);
	GPIO_PinInit(CH438_D_GPIO,CH438_D4_PIN,&ch438_d0_config);
  GPIO_PinInit(CH438_D_GPIO,CH438_D5_PIN,&ch438_d0_config);
	GPIO_PinInit(CH438_D_GPIO,CH438_D6_PIN,&ch438_d0_config);
  GPIO_PinInit(CH438_D_GPIO,CH438_D7_PIN,&ch438_d0_config);

}
//#define ADAPTER_ZIGBEE_E18
/****************************************************************************
 * Name: ReadCH438Data
 *
 * Description:
 *   Read data from ch438 address
 *
 ****************************************************************************/
static uint8_t ReadCH438Data(uint8_t addr)
{
    uint8_t dat = 0;
	  GPIO_PinWrite(CH438_CTL_GPIO,CH438_NWR_PIN,PIN_HIGH);
		GPIO_PinWrite(CH438_CTL_GPIO,CH438_NRD_PIN,PIN_HIGH);
		GPIO_PinWrite(CH438_CTL_GPIO,CH438_ALE_PIN,PIN_HIGH);
 
	  
	  CH438SetOutput();
   
	  #ifdef ADAPTER_ZIGBEE_E18
    udelay(10);
    #else
    udelay(1);   
	  #endif	    
	  if(addr &0x80)    GPIO_PinWrite(CH438_D_GPIO,CH438_D7_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D7_PIN,PIN_LOW);
    if(addr &0x40)    GPIO_PinWrite(CH438_D_GPIO,CH438_D6_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D6_PIN,PIN_LOW);
    if(addr &0x20)    GPIO_PinWrite(CH438_D_GPIO,CH438_D5_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D5_PIN,PIN_LOW);
    if(addr &0x10)    GPIO_PinWrite(CH438_D_GPIO,CH438_D4_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D4_PIN,PIN_LOW);
    if(addr &0x08)    GPIO_PinWrite(CH438_D_GPIO,CH438_D3_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D3_PIN,PIN_LOW);
    if(addr &0x04)    GPIO_PinWrite(CH438_D_GPIO,CH438_D2_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D2_PIN,PIN_LOW);
    if(addr &0x02)    GPIO_PinWrite(CH438_D_GPIO,CH438_D1_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D1_PIN,PIN_LOW);
    if(addr &0x01)    GPIO_PinWrite(CH438_D_GPIO,CH438_D0_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D0_PIN,PIN_LOW);

		#ifdef ADAPTER_ZIGBEE_E18
    udelay(10);
    #else
    udelay(1);   
	  #endif	    
	
    GPIO_PinWrite(CH438_CTL_GPIO,CH438_ALE_PIN,PIN_LOW);
   
		
		#ifdef ADAPTER_ZIGBEE_E18
    udelay(10);
    #else
    udelay(1);   
	  #endif	    
	
    CH438SetInput();
		
    #ifdef ADAPTER_ZIGBEE_E18
    udelay(10);
    #else
    udelay(1);   
	  #endif	    
	
    GPIO_PinWrite(CH438_CTL_GPIO,CH438_NRD_PIN,PIN_LOW);
    
		#ifdef ADAPTER_ZIGBEE_E18
    udelay(10);
    #else
    udelay(1);   
	  #endif	    
	  
   
    if (GPIO_PinRead(CH438_D_GPIO,CH438_D7_PIN))    dat |= 0x80;
    if (GPIO_PinRead(CH438_D_GPIO,CH438_D6_PIN))    dat |= 0x40;
    if (GPIO_PinRead(CH438_D_GPIO,CH438_D5_PIN))    dat |= 0x20;
    if (GPIO_PinRead(CH438_D_GPIO,CH438_D4_PIN))    dat |= 0x10;
    if (GPIO_PinRead(CH438_D_GPIO,CH438_D3_PIN))    dat |= 0x08;
    if (GPIO_PinRead(CH438_D_GPIO,CH438_D2_PIN))    dat |= 0x04;
    if (GPIO_PinRead(CH438_D_GPIO,CH438_D1_PIN))    dat |= 0x02;
    if (GPIO_PinRead(CH438_D_GPIO,CH438_D0_PIN))    dat |= 0x01;
  
		
		
		GPIO_PinWrite(CH438_CTL_GPIO,CH438_NRD_PIN,PIN_HIGH);
		GPIO_PinWrite(CH438_CTL_GPIO,CH438_ALE_PIN,PIN_HIGH);

		
    #ifdef ADAPTER_ZIGBEE_E18
    udelay(10);
    #else
    udelay(1);   
	  #endif	    
	

    return dat;
}

/****************************************************************************
 * Name: WriteCH438Data
 *
 * Description:
 *   write data to ch438 address
 *
 ****************************************************************************/
static void WriteCH438Data(uint8_t addr, const uint8_t dat)
{   
	  
    GPIO_PinWrite(CH438_CTL_GPIO,CH438_ALE_PIN,PIN_HIGH);
		GPIO_PinWrite(CH438_CTL_GPIO,CH438_NRD_PIN,PIN_HIGH);
	  GPIO_PinWrite(CH438_CTL_GPIO,CH438_NWR_PIN,PIN_HIGH);	
 
	
	  CH438SetOutput();
    
    
	  #ifdef ADAPTER_ZIGBEE_E18
    udelay(100);
    #else
    udelay(1);   
	  #endif
    
    
		if(addr &0x80)    GPIO_PinWrite(CH438_D_GPIO,CH438_D7_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D7_PIN,PIN_LOW);
    if(addr &0x40)    GPIO_PinWrite(CH438_D_GPIO,CH438_D6_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D6_PIN,PIN_LOW);
    if(addr &0x20)    GPIO_PinWrite(CH438_D_GPIO,CH438_D5_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D5_PIN,PIN_LOW);
    if(addr &0x10)    GPIO_PinWrite(CH438_D_GPIO,CH438_D4_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D4_PIN,PIN_LOW);
    if(addr &0x08)    GPIO_PinWrite(CH438_D_GPIO,CH438_D3_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D3_PIN,PIN_LOW);
    if(addr &0x04)    GPIO_PinWrite(CH438_D_GPIO,CH438_D2_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D2_PIN,PIN_LOW);
    if(addr &0x02)    GPIO_PinWrite(CH438_D_GPIO,CH438_D1_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D1_PIN,PIN_LOW);
    if(addr &0x01)    GPIO_PinWrite(CH438_D_GPIO,CH438_D0_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D0_PIN,PIN_LOW);

		
	  #ifdef ADAPTER_ZIGBEE_E18
    udelay(100);
    #else
    udelay(1);   
	  #endif	 
    
		
    GPIO_PinWrite(CH438_CTL_GPIO,CH438_ALE_PIN,PIN_LOW);
   
		
		
		#ifdef ADAPTER_ZIGBEE_E18
    udelay(100);
    #else
    udelay(1);   
	  #endif	
    
		
    if(dat &0x80)    GPIO_PinWrite(CH438_D_GPIO,CH438_D7_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D7_PIN,PIN_LOW);
    if(dat &0x40)    GPIO_PinWrite(CH438_D_GPIO,CH438_D6_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D6_PIN,PIN_LOW);
    if(dat &0x20)    GPIO_PinWrite(CH438_D_GPIO,CH438_D5_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D5_PIN,PIN_LOW);
    if(dat &0x10)    GPIO_PinWrite(CH438_D_GPIO,CH438_D4_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D4_PIN,PIN_LOW);
    if(dat &0x08)    GPIO_PinWrite(CH438_D_GPIO,CH438_D3_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D3_PIN,PIN_LOW);
    if(dat &0x04)    GPIO_PinWrite(CH438_D_GPIO,CH438_D2_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D2_PIN,PIN_LOW);
    if(dat &0x02)    GPIO_PinWrite(CH438_D_GPIO,CH438_D1_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D1_PIN,PIN_LOW);
    if(dat &0x01)    GPIO_PinWrite(CH438_D_GPIO,CH438_D0_PIN,PIN_HIGH);  else    GPIO_PinWrite(CH438_D_GPIO,CH438_D0_PIN,PIN_LOW);
 
   	
		#ifdef ADAPTER_ZIGBEE_E18
    udelay(100);
    #else
    udelay(1);   
	  #endif	   
    
		
    GPIO_PinWrite(CH438_CTL_GPIO,CH438_NWR_PIN,PIN_LOW);
 
		
		#ifdef ADAPTER_ZIGBEE_E18
    udelay(100);
    #else
    udelay(1);   
	  #endif	  
    
	
		GPIO_PinWrite(CH438_CTL_GPIO,CH438_NWR_PIN,PIN_HIGH);
    GPIO_PinWrite(CH438_CTL_GPIO,CH438_ALE_PIN,PIN_HIGH);

		
		#ifdef ADAPTER_ZIGBEE_E18
    udelay(100);
    #else
    udelay(1);   
	  #endif	    

    CH438SetInput();

    return;
}

/****************************************************************************
 * Name: WriteCH438Block
 *
 * Description:
 *   Write data block from ch438 address
 *
 ****************************************************************************/
static void WriteCH438Block(uint8_t mAddr, uint8_t mLen, const uint8_t *mBuf)
{
    while(mLen--)
        WriteCH438Data(mAddr, *mBuf++);
}

/****************************************************************************
 * Name: CH438UARTSend
 *
 * Description:
 *   Enable FIFO mode, which is used for ch438 serial port to send multi byte data,
 *   with a maximum of 128 bytes of data sent at a time
 *
 ****************************************************************************/
static void Ch438UartSend(uint8_t ext_uart_no, const uint8_t *Data, uint16_t Num)
{
    uint8_t REG_LSR_ADDR,REG_THR_ADDR;
    REG_LSR_ADDR = offsetadd[ext_uart_no] | REG_LSR0_ADDR;
    REG_THR_ADDR = offsetadd[ext_uart_no] | REG_THR0_ADDR;
    
    while(1)
    {
        while((ReadCH438Data(REG_LSR_ADDR) & BIT_LSR_TEMT) == 0); /* wait for sending data done, THR and TSR is NULL */
        if(Num <= 128)
        {
            WriteCH438Block(REG_THR_ADDR, Num, Data);
            break;
        }
        else
        {
            WriteCH438Block(REG_THR_ADDR, 128, Data);
            Num -= 128;
            Data += 128;
        }
    }
		
}

/****************************************************************************
 * Name: CH438UARTRcv
 *
 * Description:
 *   Disable FIFO mode for ch438 serial port to receive multi byte data
 *
 ****************************************************************************/
uint8_t CH438UARTRcv(uint8_t ext_uart_no, uint8_t *buf, size_t size)
{
    uint8_t rcv_num = 0;
    uint8_t dat = 0;
    uint8_t REG_LSR_ADDR,REG_RBR_ADDR;
    uint8_t *read_buffer;
    size_t buffer_index = 0;

    read_buffer = buf;

    REG_LSR_ADDR = offsetadd[ext_uart_no] | REG_LSR0_ADDR;
    REG_RBR_ADDR = offsetadd[ext_uart_no] | REG_RBR0_ADDR;
    
    /* Wait for the data to be ready */	
	  int count=0;
    while ((ReadCH438Data(REG_LSR_ADDR) & BIT_LSR_DATARDY) == 0){
		}
    while (((ReadCH438Data(REG_LSR_ADDR) & BIT_LSR_DATARDY) == 0x01) && (size != 0))
    {   count++;
			  if(1==count){
					rt_thread_mdelay(5000);
				}
        rt_thread_mdelay(5);
			  dat = ReadCH438Data(REG_RBR_ADDR);
        *read_buffer = dat;
        read_buffer++;
        buffer_index++;
        if (255 == buffer_index) {
            buffer_index = 0;
            read_buffer = buf;
        }

        ++rcv_num;
        --size;
    }

    return rcv_num;
}

/****************************************************************************
 * Name: ImxrtCH438Init
 *
 * Description:
 *   ch438 initialization
 *
 ****************************************************************************/
static void ImxrtCH438Init(void)
{
    CH438SetOutput();
	  gpio_pin_config_t ch438_ctl_config ={kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
    GPIO_PinInit(CH438_CTL_GPIO,CH438_NWR_PIN,&ch438_ctl_config);
		GPIO_PinInit(CH438_CTL_GPIO,CH438_NRD_PIN,&ch438_ctl_config);
		GPIO_PinInit(CH438_CTL_GPIO,CH438_ALE_PIN,&ch438_ctl_config);
		
	  GPIO_PinWrite(CH438_CTL_GPIO,CH438_NWR_PIN,PIN_HIGH);
		GPIO_PinWrite(CH438_CTL_GPIO,CH438_NRD_PIN,PIN_HIGH);
		GPIO_PinWrite(CH438_CTL_GPIO,CH438_ALE_PIN,PIN_HIGH);
}

/****************************************************************************
 * Name: CH438PortInit
 *
 * Description:
 *   ch438 port initialization
 *
 ****************************************************************************/
static void CH438PortInit(uint8_t ext_uart_no, uint32_t baud_rate)
{
    uint32_t div;
    uint8_t DLL,DLM,dlab;
    uint8_t REG_LCR_ADDR;
    uint8_t REG_DLL_ADDR;
    uint8_t REG_DLM_ADDR;
    uint8_t REG_IER_ADDR;
    uint8_t REG_MCR_ADDR;
    uint8_t REG_FCR_ADDR;

    REG_LCR_ADDR = offsetadd[ext_uart_no] | REG_LCR0_ADDR;
    REG_DLL_ADDR = offsetadd[ext_uart_no] | REG_DLL0_ADDR;
    REG_DLM_ADDR = offsetadd[ext_uart_no] | REG_DLM0_ADDR;
    REG_IER_ADDR = offsetadd[ext_uart_no] | REG_IER0_ADDR;
    REG_MCR_ADDR = offsetadd[ext_uart_no] | REG_MCR0_ADDR;
    REG_FCR_ADDR = offsetadd[ext_uart_no] | REG_FCR0_ADDR;

    /* reset the uart */
    WriteCH438Data(REG_IER_ADDR, BIT_IER_RESET); 
    up_mdelay(50);

    dlab = ReadCH438Data(REG_IER_ADDR);
    dlab &= 0xDF;
    WriteCH438Data(REG_IER_ADDR, dlab);
    
    /* set LCR register DLAB bit 1 */
    dlab = ReadCH438Data(REG_LCR_ADDR);
    dlab |= 0x80;
    WriteCH438Data(REG_LCR_ADDR, dlab);

    div = (Fpclk >> 4) / baud_rate;
    DLM = div >> 8;
    DLL = div & 0xff;

    /* set bps */
    WriteCH438Data(REG_DLL_ADDR, DLL);
    WriteCH438Data(REG_DLM_ADDR, DLM);

    /* set FIFO mode, 112 bytes */
    WriteCH438Data(REG_FCR_ADDR, BIT_FCR_RECVTG1 | BIT_FCR_RECVTG0 | BIT_FCR_FIFOEN);

    /* 8 bit word size, 1 bit stop bit, no crc */
    WriteCH438Data(REG_LCR_ADDR, BIT_LCR_WORDSZ1 | BIT_LCR_WORDSZ0);

    /* enable interrupt */
    WriteCH438Data(REG_IER_ADDR, BIT_IER_IERECV);

    /* allow interrupt output, DTR and RTS is 1 */
    WriteCH438Data(REG_MCR_ADDR, BIT_MCR_OUT2);

    /* release the data in FIFO */
    WriteCH438Data(REG_FCR_ADDR, ReadCH438Data(REG_FCR_ADDR)| BIT_FCR_TFIFORST);
}
/****************************************************************************
 * Name: ImxrtCh438ReadData
 *
 * Description:
 *   Read data from ch438 port
 *
 ****************************************************************************/
static int ImxrtCh438WriteData(uint8_t ext_uart_no, const uint8_t *write_buffer, size_t size)
{
    int write_len, write_len_continue;
    int i, write_index;
    if(write_buffer == NULL){
		   return ERROR;
		}

    write_len = size;
    write_len_continue = size;

    if(write_len > 256) 
    {
        if(0 == write_len % 256) 
        {
            write_index = write_len / 256;
            for(i = 0; i < write_index; i ++) 
            {
                Ch438UartSend(ext_uart_no, write_buffer + i * 256, 256);
            }
        }
        else
        {
            write_index = 0;
            while(write_len_continue > 256) 
            {
                Ch438UartSend(ext_uart_no, write_buffer + write_index * 256, 256);
                write_index++;
                write_len_continue = write_len - write_index * 256;
            }
            Ch438UartSend(ext_uart_no, write_buffer + write_index * 256, write_len_continue);
        }
    } 
    else
    {
        Ch438UartSend(ext_uart_no, write_buffer, write_len);
    }

    return OK;
}

/****************************************************************************
 * Name: ImxrtCh438ReadData
 *
 * Description:
 *   Read data from ch438 port
 *
 ****************************************************************************/
static size_t ImxrtCh438ReadData(uint8_t ext_uart_no, size_t size)
{
    size_t RevLen = 0;
    uint8_t InterruptStatus;
    uint8_t REG_IIR_ADDR;
    uint8_t REG_LSR_ADDR;
    uint8_t REG_MSR_ADDR;

        REG_IIR_ADDR = offsetadd[ext_uart_no] | REG_IIR0_ADDR;
        REG_LSR_ADDR = offsetadd[ext_uart_no] | REG_LSR0_ADDR;
        REG_MSR_ADDR = offsetadd[ext_uart_no] | REG_MSR0_ADDR;
        /* Read the interrupt status of the serial port */
        InterruptStatus = ReadCH438Data(REG_IIR_ADDR) & 0x0f;
        rt_kprintf("InterruptStatus is %d\n", InterruptStatus);

        switch(InterruptStatus)
        {
            case INT_NOINT:            /* no interrupt */
                break;
            case INT_THR_EMPTY:        /* the transmit hold register is not interrupted */
                break;
            case INT_RCV_OVERTIME:     /* receive data timeout interrupt */
            case INT_RCV_SUCCESS:      /* receive data available interrupt */
                RevLen = CH438UARTRcv(ext_uart_no, buff[ext_uart_no], size);
                break;
            case INT_RCV_LINES:       /* receive line status interrupt */
                ReadCH438Data(REG_LSR_ADDR);
                break;
            case INT_MODEM_CHANGE:    /* modem input change interrupt */
                ReadCH438Data(REG_MSR_ADDR);
                break;
            default:
                break;
        }

    return RevLen;
}

/****************************************************************************
 * Name: Ch438InitDefault
 *
 * Description:
 *   Ch438 default initialization function
 *
 ****************************************************************************/
static void Ch438InitDefault(void)
{ 
   /*Dynamically create semaphores */
   semaphoreChInit();
   
	 ImxrtCH438Init();

/* If a port is checked, the port will be initialized. Otherwise, the interrupt of the port will be disabled. */

#ifdef CONFIG_CH438_EXTUART0
    CH438PortInit(0, CONFIG_CH438_EXTUART0_BAUD);
#else
    WriteCH438Data(REG_IER0_ADDR, 0x00);
#endif

#ifdef CONFIG_CH438_EXTUART1
    CH438PortInit(1, CONFIG_CH438_EXTUART1_BAUD);
#else
    WriteCH438Data(REG_IER1_ADDR, 0x00);
#endif

#ifdef CONFIG_CH438_EXTUART2
    CH438PortInit(2, CONFIG_CH438_EXTUART2_BAUD);
#else
    WriteCH438Data(REG_IER2_ADDR, 0x00);
#endif

#ifdef CONFIG_CH438_EXTUART3
    CH438PortInit(3, CONFIG_CH438_EXTUART3_BAUD);
#else
    WriteCH438Data(REG_IER3_ADDR, 0x00);
#endif

#ifdef CONFIG_CH438_EXTUART4
    CH438PortInit(4, CONFIG_CH438_EXTUART4_BAUD);
#else
    WriteCH438Data(REG_IER4_ADDR, 0x00);
#endif

#ifdef CONFIG_CH438_EXTUART5
    CH438PortInit(5, CONFIG_CH438_EXTUART5_BAUD);
#else
    WriteCH438Data(REG_IER5_ADDR, 0x00);
#endif

#ifdef CONFIG_CH438_EXTUART6
    CH438PortInit(6, CONFIG_CH438_EXTUART6_BAUD);
#else
    WriteCH438Data(REG_IER6_ADDR, 0x00);
#endif

#ifdef CONFIG_CH438_EXTUART7
    CH438PortInit(7, CONFIG_CH438_EXTUART7_BAUD);
#else
    WriteCH438Data(REG_IER7_ADDR, 0x00);
#endif

    up_mdelay(10);
  
}
/****************************************************************************
 * Name: static void getChInterruptStatus( void *arg)
 *
 * Description:
 *   read Interrupt register of ch438
 *   
 ****************************************************************************/

static int getCh438InterruptStatus(void ){
		uint8_t gChInterruptStatus=0;    /*Interrupt register status*/
	  uint8_t i=7;
	  gChInterruptStatus=ReadCH438Data(REG_SSR_ADDR);
	  rt_kprintf("gChInterruptStatus: %x",gChInterruptStatus);
	  if(gChInterruptStatus){
				for(i=0;i<CH438PORTNUM;i++){
						if(g_uart_selected[i] &&(gChInterruptStatus && Interruptnum[i])){
						   goto ch_out;
						}
				}
		}
ch_out:
		return i;
}
/****************************************************************************
 * Name: int semaphoreChInit(void)
 *
 * Description:
 *   Initialize semaphore which is related to ch438
 *   
 ****************************************************************************/
int semaphoreChInit(void){
	  
   for(uint8_t i=0;i<CH438PORTNUM;i++){
			
		 if(g_uart_selected[i]){
					rx_sem[i]=rt_sem_create(sem[i],0, RT_IPC_FLAG_FIFO);
				  
			   if(rx_sem[i]!=RT_NULL){
						
					 rt_kprintf("create sem:%s done\r\n",sem[i]);
					}else{
					  
						rt_kprintf("create sem:%s failed\r\n",sem[i]);
						return -1;
					}
			}
	 }
}

/****************************************************************************
 * Name: CH438Test
 *
 * Description:
 *   Test ch438 port 7
 *   use port 7 ,send data to buf
 ****************************************************************************/
#ifdef CONFIG_CH438_EXTUART7
void CH438Test(void){
	
	/* CH438 Test */
  uint8_t buff[10]={0};
	uint8_t str[]="rt device write successfully!\r\n";
	static rt_device_t serial;        
	Ch438InitDefault();    //config deafult
	
	/* RT-Thread CH438 Test */
	#if AT_7
	serial=rt_device_find(EXTU_UART_7);
	rt_device_open(serial,RT_DEVICE_OFLAG_RDWR);
	rt_device_write(serial, 0, str, (sizeof(str) ));
	Ch438UartSend(7,str,sizeof(str));
  while(rt_device_read(serial,0, buff, (sizeof(buff))) ==0);
	rt_device_write(serial, 0, "OK\r\n", (sizeof("OK\r\n") ));
	#endif
}
MSH_CMD_EXPORT(CH438Test,ch438 test sample);


void CH438Init(void){
	Ch438InitDefault();           //config deafult
}
MSH_CMD_EXPORT(CH438Init,ch438 Init sample);
#endif
/****************************************************************************
 * Name: HC08Test
 *
 * Description:
 *   Test hc08 
 *   use port 2 
 ****************************************************************************/
#define AT_7 (0U)
#define AT_2 (0U)
#define AT_7D (0U)

void HC08Test(void){
 
	uint8_t buf2[10]={0};
	int time=20;
	char cmd[]="AT";
	#if AT_7D
	
	uint8_t buf7[10]={0};
	static rt_device_t seria2;
  seria2=rt_device_find(EXTU_UART_2);
	rt_device_open(seria2,RT_DEVICE_OFLAG_RDWR);
	rt_device_write(seria2,0, cmd, (sizeof(cmd)));
  rt_device_read(seria2,0, cmd, (sizeof(cmd)));
	
	static rt_device_t seria7;
	seria7=rt_device_find(EXTU_UART_7);
	rt_device_open(seria7,RT_DEVICE_OFLAG_RDWR);
	rt_device_write(seria7,0, cmd, (sizeof(cmd)));
  rt_device_read(seria7,0, cmd, (sizeof(cmd)));
  #endif
   #ifdef CONFIG_CH438_EXTUART2
	 #if AT_2
   Ch438UartSend(2,(const uint8_t *)"AT",sizeof("AT"));
	 rt_thread_mdelay(100);
   Ch438UartSend(2,(const uint8_t *)"AT+DEFAULT",sizeof("AT+DEFAULT"));
   rt_thread_mdelay(3000);
	
   while(time--){
	 rt_kprintf("send AT\r\n");
	 Ch438UartSend(2,(const uint8_t *)"AT",sizeof("AT"));
	 rt_thread_mdelay(3000);
		 
	 rt_kprintf("AT+LUUID=1233\r\n");
   Ch438UartSend(2,(const uint8_t *)"AT+LUUID=1233",sizeof("AT+LUUID=1233"));
	 rt_thread_mdelay(3000);
		
	 rt_kprintf("AT+SUUID=1233\r\n");
	 Ch438UartSend(2,(const uint8_t *)"AT+SUUID=1233",sizeof("AT+SUUID=1233"));
	 rt_thread_mdelay(3000);
		 
	 rt_kprintf("AT+TUUID=1233\r\n");
	 Ch438UartSend(2,(const uint8_t *)"AT+TUUID=1233",sizeof("AT+TUUID=1233"));
	 rt_thread_mdelay(3000);
		 
	 CH438UARTRcv(2,buf2,sizeof(buf2));
   rt_kprintf("buf2:%s\r\n",buf2);
	 rt_thread_mdelay(1000);
	 } 
   #endif 
	 #endif
	 #if AT_7
	 rt_kprintf("send AT\r\n");
   Ch438UartSend(0,(const uint8_t *)"AT",sizeof("AT"));
	 rt_thread_mdelay(100);
	 Ch438UartSend(0,(const uint8_t *)"AT+DEFAULT",sizeof("AT+DEFAULT"));
   rt_thread_mdelay(3000);

	
   while(time--){
	 rt_kprintf("send AT\r\n");
	 Ch438UartSend(0,(const uint8_t *)"AT",sizeof("AT"));
	 rt_thread_mdelay(3000);
	 rt_kprintf("AT+LUUID=1233\r\n");
   Ch438UartSend(0,(const uint8_t *)"AT+LUUID=1233",sizeof("AT+LUUID=1233"));
	 rt_thread_mdelay(3000);
	 rt_kprintf("AT+SUUID=1233\r\n");
	 Ch438UartSend(0,(const uint8_t *)"AT+SUUID=1233",sizeof("AT+SUUID=1233"));
	 rt_thread_mdelay(3000);
	 rt_kprintf("AT+TUUID=1233\r\n");
	 Ch438UartSend(0,(const uint8_t *)"AT+TUUID=1233",sizeof("AT+TUUID=1233"));
   
	 CH438UARTRcv(0,buf2,sizeof(buf2));
   rt_kprintf("buf2:%s\r\n",buf2);
	 rt_thread_mdelay(1000);
	 }
	 #endif
   rt_kprintf("send AT\r\n");
   Ch438UartSend(7,(const uint8_t *)"AT",sizeof("AT"));
	 rt_thread_mdelay(1000);
	 CH438UARTRcv(7,buf2,sizeof(buf2));
	 Ch438UartSend(7,(const uint8_t *)"AT+DEFAULT",sizeof("AT+DEFAULT"));
   rt_thread_mdelay(3000);

	
   while(time--){
	 rt_kprintf("send AT\r\n");
	 Ch438UartSend(7,(const uint8_t *)"AT",sizeof("AT"));
	 rt_thread_mdelay(3000);
	 rt_kprintf("AT+LUUID=1233\r\n");
   Ch438UartSend(7,(const uint8_t *)"AT+LUUID=1233",sizeof("AT+LUUID=1233"));
	 rt_thread_mdelay(3000);
	 rt_kprintf("AT+SUUID=1233\r\n");
	 Ch438UartSend(7,(const uint8_t *)"AT+SUUID=1233",sizeof("AT+SUUID=1233"));
	 rt_thread_mdelay(3000);
	 rt_kprintf("AT+TUUID=1233\r\n");
	 Ch438UartSend(7,(const uint8_t *)"AT+TUUID=1233",sizeof("AT+TUUID=1233"));
   
	 CH438UARTRcv(7,buf2,sizeof(buf2));
   rt_kprintf("buf2:%s\r\n",buf2);
	 rt_thread_mdelay(1000);
	 }
 
	
	

}
MSH_CMD_EXPORT(HC08Test,hc08 test sample);
#endif

/****************************************************************************
 * Name: void ZigBeeTest(void)
 *
 * Description:
 *   Test E18
 *   use port 1 
 ****************************************************************************/
#ifdef CONFIG_CH438_EXTUART1
void ZigBeeConfig(void){
    uint8_t buf1[300]={0};
    gpio_pin_config_t ZigBee_IO_config ={kGPIO_DigitalOutput, 1, kGPIO_NoIntmode};
    GPIO_PinInit(ZIGBEE_GPIO,ZIGBEE_MODE_PIN,&ZigBee_IO_config);
    rt_kprintf("AT+MODE=?\r\n");
		Ch438UartSend(1,(const uint8_t *)"AT+MODE=?",sizeof("AT+MODE=?"));
    CH438UARTRcv(1,buf1,sizeof(buf1));
    rt_kprintf("buf1:%s\r\n",buf1);
}
MSH_CMD_EXPORT(ZigBeeConfig,e18 config sample);



void ZigBeeHexConfig(void){
    uint8_t buf1[100]={0};
    gpio_pin_config_t ZigBee_IO_config ={kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
    GPIO_PinInit(ZIGBEE_GPIO,ZIGBEE_MODE_PIN,&ZigBee_IO_config);
    rt_kprintf("hex config\r\n");
		Ch438UartSend(1,(const uint8_t *)"+++",sizeof("+++"));
    CH438UARTRcv(1,buf1,sizeof(buf1));
    rt_kprintf("buf1:%s\r\n",buf1);
}
MSH_CMD_EXPORT(ZigBeeHexConfig,e18 config sample);

void  ZigBeeTest(void){
	static rt_device_t seria1;
	uint8_t buf1[30]={0};
	const char cmd[]="+++";


	seria1=rt_device_find(EXTU_UART_1);
	rt_device_open(seria1,RT_DEVICE_OFLAG_RDWR);
	int time=20;
	while(time--){
			rt_device_write(seria1,0, cmd, (sizeof(cmd)));
		  rt_thread_mdelay(1000);
      rt_device_read(seria1,0, buf1, (sizeof(buf1)));
		  printf("%s",buf1);
	}
}
MSH_CMD_EXPORT(ZigBeeTest,e18 test sample);

#endif
/* ---------------------------------LORA测试代码--------------------------------- */
#ifdef CONFIG_CH438_EXTUART3
void TestHex(void){
	  uint8_t buf[]={0xC0,0x00,0x08,0x00,0x01,0x64,0x00,0x05,0x03,0x00,0x00};
		for(int i=0;i<11;i++){
			rt_kprintf("num:%x \r\n",i);
			rt_kprintf("num:%x \r\n",buf[i]);
			Ch438UartSend(0,(const uint8_t *)&buf[i],1);
		}
		Ch438UartSend(0,(const uint8_t *)buf,11);

}
MSH_CMD_EXPORT(TestHex,Hex test sample);

#define E22400T_M1_PIN (11U)
#define E22400T_M0_PIN (9U)
#define CLIENT (0U)
#define SERVER (0U)

void lora_send(void){
	
		uint8_t recbuffer[20]={0};
		uint8_t cmd[10]={0xFF,0xFF,0x02,0xAA,0XBB,0xCC};   //sned AA BB CC to address 03 channel02
		uint8_t times=1;

		
		/* 发送模式 */
    rt_pin_mode (E22400T_M1_PIN, PIN_MODE_OUTPUT);
	  rt_pin_mode (E22400T_M0_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(E22400T_M1_PIN, PIN_LOW);
	  rt_pin_write(E22400T_M0_PIN, PIN_HIGH);
	  rt_thread_mdelay(5000);
		
		while(times--){
				Ch438UartSend(3,(const uint8_t *)cmd,6);
			  rt_kprintf("send %d time \r\n",20-times);
			  rt_thread_mdelay(2000);
		}
	
}
MSH_CMD_EXPORT(lora_send,Lora send test sample);


void lora_rec(void){
		
		uint8_t recbuffer[20]={0};
		uint8_t cmd[10]={0x00,0x04,0x02,0x07,0X08,0x09};   //sned AA BB CC to address 04 channel02
		uint8_t recnum=0;
		/* 发送模式 */
    rt_pin_mode (E22400T_M1_PIN, PIN_MODE_OUTPUT);
	  rt_pin_mode (E22400T_M0_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(E22400T_M1_PIN, PIN_LOW);
	  rt_pin_write(E22400T_M0_PIN, PIN_HIGH);
	  rt_thread_mdelay(5000);

		
		recnum=CH438UARTRcv(3,recbuffer,12);
		rt_kprintf("recive num:%d,data:%d,%d,%d,%d,%d\r\n",recnum,recbuffer[0],recbuffer[1],recbuffer[2],recbuffer[3],recbuffer[4]);
	  memset(recbuffer,0,12);
	
	

}
MSH_CMD_EXPORT(lora_rec,Lora receive test sample);


void lora_send_config(void){
		
		uint8_t recbuffer[20]={0};                              //发送模块配置
		uint8_t cmd1[10]={0xC0,0x04,0x01,0x02};                 //配置信道为0x02
		uint8_t cmd2[10]={0xC0,0x00,0x03,0xFF,0xFF,0x61};       //配置模块地址为FF FF，串口(9600,8N1)，空速（2.4K）
		uint8_t recnum=0;
		/* 发送模式 */
    rt_pin_mode (E22400T_M1_PIN, PIN_MODE_OUTPUT);
	  rt_pin_mode (E22400T_M0_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(E22400T_M1_PIN, PIN_HIGH);
	  rt_pin_write(E22400T_M0_PIN, PIN_HIGH);
	  rt_thread_mdelay(5000);
		Ch438UartSend(3,(const uint8_t *)cmd1,4);
		rt_thread_mdelay(500);
		recnum=CH438UARTRcv(3,recbuffer,4);
		rt_kprintf("recive num:%d,data:%d,%d,%d,%d\r\n",recnum,recbuffer[0],recbuffer[1],recbuffer[2],recbuffer[3]);
	  memset(recbuffer,0,4);
		Ch438UartSend(3,(const uint8_t *)cmd2,6);
		rt_thread_mdelay(500);
		recnum=CH438UARTRcv(3,recbuffer,6);
		rt_kprintf("recive num:%d,data:%d,%d,%d,%d,%d,%d\r\n",recnum,recbuffer[0],recbuffer[1],recbuffer[2],recbuffer[3],recbuffer[4],recbuffer[5]);
		memset(recbuffer,0,6);
 
}
MSH_CMD_EXPORT(lora_send_config,Lora send test sample);

void lora_rec_config(void){
		
		uint8_t recbuffer[20]={0};                              //接收模块配置
		uint8_t cmd1[10]={0xC0,0x04,0x01,0x02};                 //配置信道为0x02
		uint8_t cmd2[10]={0xC0,0x00,0x03,0x00,0x04,0x61};       //配置模块地址为00 04，串口(9600,8N1)，空速（2.4K）
		uint8_t recnum=0;
		/* 配置模式 */
    rt_pin_mode (E22400T_M1_PIN, PIN_MODE_OUTPUT);
	  rt_pin_mode (E22400T_M0_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(E22400T_M1_PIN, PIN_HIGH);
	  rt_pin_write(E22400T_M0_PIN, PIN_HIGH);
	  rt_thread_mdelay(5000);
		Ch438UartSend(3,(const uint8_t *)cmd1,4);
		rt_thread_mdelay(500);
		recnum=CH438UARTRcv(3,recbuffer,4);
		rt_kprintf("recive num:%d,data:%d,%d,%d,%d\r\n",recnum,recbuffer[0],recbuffer[1],recbuffer[2],recbuffer[3]);
	  memset(recbuffer,0,4);
		Ch438UartSend(3,(const uint8_t *)cmd2,6);
		rt_thread_mdelay(500);
		recnum=CH438UARTRcv(3,recbuffer,6);
		rt_kprintf("recive num:%d,data:%d,%d,%d,%d,%d,%d\r\n",recnum,recbuffer[0],recbuffer[1],recbuffer[2],recbuffer[3],recbuffer[4],recbuffer[5]);
		memset(recbuffer,0,6);
 
}
MSH_CMD_EXPORT(lora_rec_config,Lora receive test sample);


void lora_read_info(void){
	  uint8_t recbuffer[20]={0};   
		uint8_t cmd1[5]={0xC1,0x04,0x01};    //获取信道
		uint8_t cmd2[5]={0xC1,0x00,0x03};    //获取地址
		uint8_t recnum=0;
		/* 配置模式 */
    rt_pin_mode (E22400T_M1_PIN, PIN_MODE_OUTPUT);
	  rt_pin_mode (E22400T_M0_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(E22400T_M1_PIN, PIN_HIGH);
	  rt_pin_write(E22400T_M0_PIN, PIN_HIGH);
		rt_thread_mdelay(5000);
		Ch438UartSend(3,(const uint8_t *)cmd1,3);
		rt_thread_mdelay(500);
		recnum=CH438UARTRcv(3,recbuffer,4);
		rt_kprintf("recive num:%d,data:%d,%d,%d,%d\r\n",recnum,recbuffer[0],recbuffer[1],recbuffer[2],recbuffer[3]);
	  memset(recbuffer,0,4);
		Ch438UartSend(3,(const uint8_t *)cmd2,3);
		rt_thread_mdelay(500);
		recnum=CH438UARTRcv(3,recbuffer,6);
		rt_kprintf("recive num:%d,data:%d,%d,%d,%d,%d,%d\r\n",recnum,recbuffer[0],recbuffer[1],recbuffer[2],recbuffer[3],recbuffer[4],recbuffer[5]);
		memset(recbuffer,0,6);
}
MSH_CMD_EXPORT(lora_read_info,Lora get info test sample);

#endif