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
 * @file imxrt_ch438.c
 * @brief imxrt board sd card automount
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.04.26
 */

#include "imxrt_ch438.h"

#define CH438PORTNUM  8
#define CH438_BUFFSIZE  256
#define CH438_INCREMENT  MSEC2TICK(33)

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/
static FAR void getInterruptStatus(FAR void *arg);
static void CH438SetOutput(void);
static void CH438SetInput(void);
static uint8_t ReadCH438Data(uint8_t addr);
static void WriteCH438Data(uint8_t addr, const uint8_t dat);
static void WriteCH438Block(uint8_t mAddr, uint8_t mLen, const uint8_t *mBuf);
static void Ch438UartSend(uint8_t ext_uart_no, const uint8_t *Data, uint16_t Num);
uint8_t CH438UARTRcv(uint8_t ext_uart_no, uint8_t *buf, size_t size);
static void ImxrtCH438Init(void);
static void CH438PortInit(uint8_t ext_uart_no, uint32_t	baud_rate);
static int ImxrtCh438WriteData(uint8_t ext_uart_no, const uint8_t *write_buffer, size_t size);
static size_t ImxrtCh438ReadData(uint8_t ext_uart_no, size_t size);
static void Ch438InitDefault(void);

static int ch438_open(FAR struct file *filep);
static int ch438_close(FAR struct file *filep);
static ssize_t ch438_read(FAR struct file *filep, FAR char *buffer, size_t buflen);
static ssize_t ch438_write(FAR struct file *filep, FAR const char *buffer, size_t buflen);
static int ch438_ioctl(FAR struct file *filep, int cmd, unsigned long arg);
static int ch438_register(FAR const char *devpath, uint8_t ext_uart_no);

/****************************************************************************
 * Private type
 ****************************************************************************/
struct ch438_dev_s
{
  sem_t devsem;    /* ch438 port devsem */
  uint8_t port;    /* ch438 port number*/
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/*mutex of corresponding port*/
static pthread_mutex_t mutex[CH438PORTNUM] =
{
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER
};

/* Condition variable of corresponding port */
static pthread_cond_t cond[CH438PORTNUM] =
{
  PTHREAD_COND_INITIALIZER,
  PTHREAD_COND_INITIALIZER,
  PTHREAD_COND_INITIALIZER,
  PTHREAD_COND_INITIALIZER,
  PTHREAD_COND_INITIALIZER,
  PTHREAD_COND_INITIALIZER,
  PTHREAD_COND_INITIALIZER,
  PTHREAD_COND_INITIALIZER
};

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

/* ch438 Callback work queue structure */
static struct work_s g_ch438irqwork;

/* there is data available on the corresponding port */
static volatile bool done[CH438PORTNUM] = {false,false,false,false,false,false,false,false};

/* Eight port data buffer */
static uint8_t buff[CH438PORTNUM][CH438_BUFFSIZE];

/* the value of interrupt number of SSR register */
static uint8_t Interruptnum[CH438PORTNUM] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,};

/* Offset address of serial port number */
static uint8_t offsetadd[CH438PORTNUM] = {0x00,0x10,0x20,0x30,0x08,0x18,0x28,0x38,};

/* port open status global variable */
static volatile bool g_ch438open[CH438PORTNUM] = {false,false,false,false,false,false,false,false};

/* Ch438 POSIX interface */
static const struct file_operations g_ch438fops =
{
  ch438_open,
  ch438_close,
  ch438_read,
  ch438_write,
  NULL,
  ch438_ioctl,
  NULL
};

/****************************************************************************
 * Name: getInterruptStatus
 *
 * Description:
 *   thread task getInterruptStatus
 *
 ****************************************************************************/
static FAR void getInterruptStatus(FAR void *arg)
{
    uint8_t i;
    uint8_t gInterruptStatus;  /* Interrupt register status */

    gInterruptStatus = ReadCH438Data(REG_SSR_ADDR);

    if(gInterruptStatus)
    {
        for(i = 0; i < CH438PORTNUM; i++)
        {
            if(g_uart_selected[i] && (gInterruptStatus & Interruptnum[i]))
            {
                pthread_mutex_lock(&mutex[i]);
                done[i] = true;
                pthread_cond_signal(&cond[i]);
                pthread_mutex_unlock(&mutex[i]);
            }
        }
    }

    work_queue(HPWORK, &g_ch438irqwork, getInterruptStatus, NULL, CH438_INCREMENT);
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
    imxrt_config_gpio(CH438_D0_PIN_OUT);
    imxrt_config_gpio(CH438_D1_PIN_OUT);
    imxrt_config_gpio(CH438_D2_PIN_OUT);
    imxrt_config_gpio(CH438_D3_PIN_OUT);
    imxrt_config_gpio(CH438_D4_PIN_OUT);
    imxrt_config_gpio(CH438_D5_PIN_OUT);
    imxrt_config_gpio(CH438_D6_PIN_OUT);
    imxrt_config_gpio(CH438_D7_PIN_OUT);
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
    imxrt_config_gpio(CH438_D0_PIN_INPUT);
    imxrt_config_gpio(CH438_D1_PIN_INPUT);
    imxrt_config_gpio(CH438_D2_PIN_INPUT);
    imxrt_config_gpio(CH438_D3_PIN_INPUT);
    imxrt_config_gpio(CH438_D4_PIN_INPUT);
    imxrt_config_gpio(CH438_D5_PIN_INPUT);
    imxrt_config_gpio(CH438_D6_PIN_INPUT);
    imxrt_config_gpio(CH438_D7_PIN_INPUT);
}

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
    imxrt_gpio_write(CH438_NWR_PIN, true);
    imxrt_gpio_write(CH438_NRD_PIN, true);
    imxrt_gpio_write(CH438_ALE_PIN, true);

    CH438SetOutput();
    up_udelay(1);
    
    if(addr &0x80)    imxrt_gpio_write(CH438_D7_PIN_OUT, true);   else    imxrt_gpio_write(CH438_D7_PIN_OUT, false);
    if(addr &0x40)    imxrt_gpio_write(CH438_D6_PIN_OUT, true);   else    imxrt_gpio_write(CH438_D6_PIN_OUT, false);
    if(addr &0x20)    imxrt_gpio_write(CH438_D5_PIN_OUT, true);   else    imxrt_gpio_write(CH438_D5_PIN_OUT, false);
    if(addr &0x10)    imxrt_gpio_write(CH438_D4_PIN_OUT, true);   else    imxrt_gpio_write(CH438_D4_PIN_OUT, false);
    if(addr &0x08)    imxrt_gpio_write(CH438_D3_PIN_OUT, true);   else    imxrt_gpio_write(CH438_D3_PIN_OUT, false);
    if(addr &0x04)    imxrt_gpio_write(CH438_D2_PIN_OUT, true);   else    imxrt_gpio_write(CH438_D2_PIN_OUT, false);
    if(addr &0x02)    imxrt_gpio_write(CH438_D1_PIN_OUT, true);   else    imxrt_gpio_write(CH438_D1_PIN_OUT, false);
    if(addr &0x01)    imxrt_gpio_write(CH438_D0_PIN_OUT, true);   else    imxrt_gpio_write(CH438_D0_PIN_OUT, false);
        
    up_udelay(1);

    imxrt_gpio_write(CH438_ALE_PIN, false);
    up_udelay(1);        

    CH438SetInput();
    up_udelay(1);
    
    imxrt_gpio_write(CH438_NRD_PIN, false);
    up_udelay(1);    
    
    if (imxrt_gpio_read(CH438_D7_PIN_INPUT))    dat |= 0x80;
    if (imxrt_gpio_read(CH438_D6_PIN_INPUT))    dat |= 0x40;
    if (imxrt_gpio_read(CH438_D5_PIN_INPUT))    dat |= 0x20;
    if (imxrt_gpio_read(CH438_D4_PIN_INPUT))    dat |= 0x10;
    if (imxrt_gpio_read(CH438_D3_PIN_INPUT))    dat |= 0x08;
    if (imxrt_gpio_read(CH438_D2_PIN_INPUT))    dat |= 0x04;
    if (imxrt_gpio_read(CH438_D1_PIN_INPUT))    dat |= 0x02;
    if (imxrt_gpio_read(CH438_D0_PIN_INPUT))    dat |= 0x01;
    
    imxrt_gpio_write(CH438_NRD_PIN, true);
    imxrt_gpio_write(CH438_ALE_PIN, true);
    up_udelay(1);

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
    imxrt_gpio_write(CH438_ALE_PIN, true);
    imxrt_gpio_write(CH438_NRD_PIN, true);
    imxrt_gpio_write(CH438_NWR_PIN, true);

    CH438SetOutput();
    up_udelay(1);    
    
    if(addr &0x80)    imxrt_gpio_write(CH438_D7_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D7_PIN_OUT, false);
    if(addr &0x40)    imxrt_gpio_write(CH438_D6_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D6_PIN_OUT, false);
    if(addr &0x20)    imxrt_gpio_write(CH438_D5_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D5_PIN_OUT, false);
    if(addr &0x10)    imxrt_gpio_write(CH438_D4_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D4_PIN_OUT, false);
    if(addr &0x08)    imxrt_gpio_write(CH438_D3_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D3_PIN_OUT, false);
    if(addr &0x04)    imxrt_gpio_write(CH438_D2_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D2_PIN_OUT, false);
    if(addr &0x02)    imxrt_gpio_write(CH438_D1_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D1_PIN_OUT, false);
    if(addr &0x01)    imxrt_gpio_write(CH438_D0_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D0_PIN_OUT, false);

    up_udelay(1);    
    
    imxrt_gpio_write(CH438_ALE_PIN, false);
    up_udelay(1);

    if(dat &0x80)    imxrt_gpio_write(CH438_D7_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D7_PIN_OUT, false);
    if(dat &0x40)    imxrt_gpio_write(CH438_D6_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D6_PIN_OUT, false);
    if(dat &0x20)    imxrt_gpio_write(CH438_D5_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D5_PIN_OUT, false);
    if(dat &0x10)    imxrt_gpio_write(CH438_D4_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D4_PIN_OUT, false);
    if(dat &0x08)    imxrt_gpio_write(CH438_D3_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D3_PIN_OUT, false);
    if(dat &0x04)    imxrt_gpio_write(CH438_D2_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D2_PIN_OUT, false);
    if(dat &0x02)    imxrt_gpio_write(CH438_D1_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D1_PIN_OUT, false);
    if(dat &0x01)    imxrt_gpio_write(CH438_D0_PIN_OUT, true);    else    imxrt_gpio_write(CH438_D0_PIN_OUT, false);
    
    up_udelay(1);    

    imxrt_gpio_write(CH438_NWR_PIN, false);
    up_udelay(1);    
    
    imxrt_gpio_write(CH438_NWR_PIN, true);
    imxrt_gpio_write(CH438_ALE_PIN, true);
    up_udelay(1);    

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
    while ((ReadCH438Data(REG_LSR_ADDR) & BIT_LSR_DATARDY) == 0);

    while (((ReadCH438Data(REG_LSR_ADDR) & BIT_LSR_DATARDY) == 0x01) && (size != 0))
    {
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
    imxrt_config_gpio(CH438_NWR_PIN);
    imxrt_config_gpio(CH438_NRD_PIN);
    imxrt_config_gpio(CH438_ALE_PIN);
    
    imxrt_gpio_write(CH438_NWR_PIN,true);
    imxrt_gpio_write(CH438_NRD_PIN,true);
    imxrt_gpio_write(CH438_ALE_PIN,true);
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
    DEBUGASSERT(write_buffer != NULL);

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

    pthread_mutex_lock(&mutex[ext_uart_no]);
    while(done[ext_uart_no] == false)
        pthread_cond_wait(&cond[ext_uart_no], &mutex[ext_uart_no]);
    if(done[ext_uart_no] == true)
    {
        REG_IIR_ADDR = offsetadd[ext_uart_no] | REG_IIR0_ADDR;
        REG_LSR_ADDR = offsetadd[ext_uart_no] | REG_LSR0_ADDR;
        REG_MSR_ADDR = offsetadd[ext_uart_no] | REG_MSR0_ADDR;
        /* Read the interrupt status of the serial port */
        InterruptStatus = ReadCH438Data(REG_IIR_ADDR) & 0x0f;
        ch438info("InterruptStatus is %d\n", InterruptStatus);

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
        done[ext_uart_no]  = false;
    }
    pthread_mutex_unlock(&mutex[ext_uart_no]);

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
    int ret = 0;
    int i;

    /* Initialize the mutex */
    for(i = 0; i < CH438PORTNUM; i++)
    {
        if(!g_uart_selected[i])
        {
            continue;
        }
        
        ret = pthread_mutex_init(&mutex[i], NULL);
        if(ret != 0)
        {
            ch438err("pthread_mutex_init failed, status=%d\n", ret);
        }
    }

    /* Initialize the condition variable */
    for(i = 0; i < CH438PORTNUM; i++)
    {
        if(!g_uart_selected[i])
        {
            continue;
        }
        
        ret = pthread_cond_init(&cond[i], NULL);
        if(ret != 0)
        {
            ch438err("pthread_cond_init failed, status=%d\n", ret);
        }
    }

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

    work_queue(HPWORK, &g_ch438irqwork, getInterruptStatus, NULL, CH438_INCREMENT);
}

/****************************************************************************
 * Name: ch438_open
 ****************************************************************************/
static int ch438_open(FAR struct file *filep)
{
    FAR struct inode *inode = filep->f_inode;
    FAR struct ch438_dev_s *priv = inode->i_private;
    uint8_t port = priv->port;
    int ret = OK;

    DEBUGASSERT(port >= 0 && port < CH438PORTNUM);

    ret = nxsem_wait_uninterruptible(&priv->devsem);
    if (ret < 0)
    {
        return ret;
    }

    if(g_ch438open[port])
    {
        ch438err("ERROR: ch438 port %d is opened!\n",port);
        return -EBUSY;
    }
    g_ch438open[port] = true;

    nxsem_post(&priv->devsem);

    return ret;
}

/****************************************************************************
 * Name: ch438_close
 ****************************************************************************/
static int ch438_close(FAR struct file *filep)
{
    FAR struct inode *inode = filep->f_inode;
    FAR struct ch438_dev_s *priv = inode->i_private;
    uint8_t port = priv->port;
    int ret = OK;

    DEBUGASSERT(port >= 0 && port < CH438PORTNUM);

    ret = nxsem_wait_uninterruptible(&priv->devsem);
    if (ret < 0)
    {
        return ret;
    }

    if(!g_ch438open[port])
    {
        ch438err("ERROR: ch438 port %d is closed!\n",port);
        return -EBUSY;
    }
    g_ch438open[port] = false;

    nxsem_post(&priv->devsem);
    return ret;
}

/****************************************************************************
 * Name: ch438_read
 ****************************************************************************/
static ssize_t ch438_read(FAR struct file *filep, FAR char *buffer, size_t buflen)
{
    size_t length = 0;
    FAR struct inode *inode = filep->f_inode;
    FAR struct ch438_dev_s *priv = inode->i_private;
    uint8_t port = priv->port;

    DEBUGASSERT(port >= 0 && port < CH438PORTNUM);

    length = ImxrtCh438ReadData(port, buflen);
    memcpy(buffer, buff[port], length);

    if(length > buflen)
    {
        length = buflen;
    }

    return length;
}

/****************************************************************************
 * Name: ch438_write
 ****************************************************************************/
static ssize_t ch438_write(FAR struct file *filep, FAR const char *buffer, size_t buflen)
{
    FAR struct inode *inode = filep->f_inode;
    FAR struct ch438_dev_s *priv = inode->i_private;
    uint8_t port = priv->port;

    DEBUGASSERT(port >= 0 && port < CH438PORTNUM);

    ImxrtCh438WriteData(port, (const uint8_t *)buffer, buflen);
    
    return buflen;
}

/****************************************************************************
 * Name: ch438_ioctl
 ****************************************************************************/
static int ch438_ioctl(FAR struct file *filep, int cmd, unsigned long arg)
{
    FAR struct inode *inode = filep->f_inode;
    FAR struct ch438_dev_s *priv = inode->i_private;
    uint8_t port = priv->port;
    int ret = OK;

    DEBUGASSERT(port >= 0 && port < CH438PORTNUM);

    switch(cmd)
    {
        case OPE_INT:
        case OPE_CFG:
            CH438PortInit(port, (uint32_t)arg);
            break;

        default:
            ch438info("Unrecognized cmd: %d\n", cmd);
            ret = -ENOTTY;
            break;
    }
    return ret;
}


/****************************************************************************
 * Name: ch438_register
 *
 * Description:
 *   Register /dev/ext_uartN
 *
 ****************************************************************************/
static int ch438_register(FAR const char *devpath, uint8_t port)
{
    FAR struct ch438_dev_s *priv;
    int ret = 0;

    /* port number check */
    DEBUGASSERT(port >= 0 && port < CH438PORTNUM);

    priv = (FAR struct ch438_dev_s *)kmm_malloc(sizeof(struct ch438_dev_s));
    if(priv == NULL)
    {
        ch438err("ERROR: Failed to allocate instance\n");
        return -ENOMEM;
    }

    priv->port = port;
    nxsem_init(&priv->devsem, 0, 1);

    /* Register the character driver */
    ret = register_driver(devpath, &g_ch438fops, 0666, priv);
    if(ret < 0)
    {
        kmm_free(priv);
    }
    
    return ret;
}

/****************************************************************************
 * Name: board_ch438_initialize
 *
 * Description:
 *   ch438 initialize
 *
 ****************************************************************************/
void board_ch438_initialize(void)
{
    Ch438InitDefault();

#ifdef CONFIG_CH438_EXTUART0
    ch438_register("/dev/extuart_dev0", 0);
#endif

#ifdef CONFIG_CH438_EXTUART1
    ch438_register("/dev/extuart_dev1", 1);
#endif

#ifdef CONFIG_CH438_EXTUART2
    ch438_register("/dev/extuart_dev2", 2);
#endif

#ifdef CONFIG_CH438_EXTUART3
    ch438_register("/dev/extuart_dev3", 3);
#endif

#ifdef CONFIG_CH438_EXTUART4
    ch438_register("/dev/extuart_dev4", 4);
#endif

#ifdef CONFIG_CH438_EXTUART5
    ch438_register("/dev/extuart_dev5", 5);
#endif

#ifdef CONFIG_CH438_EXTUART6
    ch438_register("/dev/extuart_dev6", 6);
#endif

#ifdef CONFIG_CH438_EXTUART7
    ch438_register("/dev/extuart_dev7", 7);
#endif
}