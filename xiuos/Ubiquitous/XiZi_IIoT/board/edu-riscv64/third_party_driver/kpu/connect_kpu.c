#include <connect_kpu.h>
#include <kpu.h>
#include <board.h>
#include "bsp.h"

#define LOAD_KMODEL 0xA0
#define RUN_KMODEL 0xA1
#define GET_OUTPUT 0xA2
#define WAIT_FLAG 0xA3

static kpu_model_context_t kpu_task;
static int g_ai_done_flag = 0;

// irq interrupt function
static void ai_done(void *ctx)
{
    g_ai_done_flag = 1;
}

struct KpuRegConfigureInfo
{
    uint8_t device_addr;
    uint16_t reg_addr;
    uint8_t reg_value;
};

static uint32 KpuDrvInit(void)
{
    x_err_t ret = EOK;
    return ret;
}

static uint32 KpuOpen(void *dev)
{
    x_err_t ret = EOK;
    KpuDrvInit();
    return ret;
}

static uint32 KpuClose(void *dev)
{
    x_err_t ret = EOK;
    return ret;
}

static uint32 KpuDrvConfigure(void *drv, struct BusConfigureInfo *args)
{
    x_err_t ret = EOK;
    int cmd_type = args->configure_cmd;
    KpuOutputBuffer* output_data;
    switch (cmd_type)
    {
    case OPE_INT:
        break;
    case OPE_CFG:
        break;
    case LOAD_KMODEL:
        kpu_load_kmodel(&kpu_task, args->private_data);
        break;
    case RUN_KMODEL:
        g_ai_done_flag=0;
        kpu_run_kmodel(&kpu_task, args->private_data, KPU_DMA_CH, ai_done, NULL);
        break;
    case GET_OUTPUT:
        output_data = (KpuOutputBuffer*)args->private_data;
        kpu_get_output(&kpu_task, 0, (uint8_t **)&(output_data->buffer), &(output_data->length));
        break;
    case WAIT_FLAG:
        *((uint8_t*)(args->private_data)) = g_ai_done_flag;
        break;
    default:
        break;
    }
    return ret;
}

/*manage the kpu device operations*/
static const struct KpuDevDone kpu_dev_done =
{
        .dev_open = KpuOpen,
        .dev_close = KpuClose,
        .dev_write = NONE,
        .dev_read = NONE,
};

/*Init kpu bus*/
static int BoardKpuBusInit(struct KpuBus *kpu_bus, struct KpuDriver *kpu_driver)
{
    x_err_t ret = EOK;

    /*Init the kpu bus */
    kpu_bus->private_data = (void *)&kpu_dev_done;
    ret = KpuBusInit(kpu_bus, KPU_BUS_NAME);
    if (EOK != ret)
    {
        KPrintf("board_kpu_init KpuBusInit error %d\n", ret);
        return ERROR;
    }

    /*Init the kpu driver*/
    kpu_driver->private_data = (void *)&kpu_dev_done;
    ret = KpuDriverInit(kpu_driver, KPU_DRV_NAME);
    if (EOK != ret)
    {
        KPrintf("board_kpu_init KpuDriverInit error %d\n", ret);
        return ERROR;
    }

    /*Attach the kpu driver to the kpu bus*/
    ret = KpuDriverAttachToBus(KPU_DRV_NAME, KPU_BUS_NAME);
    if (EOK != ret)
    {
        KPrintf("board_kpu_init KpuDriverAttachToBus error %d\n", ret);
        return ERROR;
    }

    return ret;
}

/*Attach the kpu device to the kpu bus*/
static int BoardKpuDevBend(void)
{
    x_err_t ret = EOK;
    static struct KpuHardwareDevice kpu_device;
    memset(&kpu_device, 0, sizeof(struct KpuHardwareDevice));

    kpu_device.kpu_dev_done = &kpu_dev_done;

    ret = KpuDeviceRegister(&kpu_device, NONE, KPU_DEVICE_NAME);
    if (EOK != ret)
    {
        KPrintf("board_kpu_init KpuDeviceInit device %s error %d\n", KPU_DEVICE_NAME, ret);
        return ERROR;
    }

    ret = KpuDeviceAttachToBus(KPU_DEVICE_NAME, KPU_BUS_NAME);
    if (EOK != ret)
    {
        KPrintf("board_kpu_init KpuDeviceAttachToBus device %s error %d\n", KPU_DEVICE_NAME, ret);
        return ERROR;
    }

    return ret;
}

int HwKpuInit(void)
{
    x_err_t ret = EOK;
    static struct KpuBus kpu_bus;
    memset(&kpu_bus, 0, sizeof(struct KpuBus));

    static struct KpuDriver kpu_driver;
    memset(&kpu_driver, 0, sizeof(struct KpuDriver));

    kpu_driver.configure = KpuDrvConfigure;
    ret = BoardKpuBusInit(&kpu_bus, &kpu_driver);
    if (EOK != ret)
    {
        KPrintf("board_kpu_Init error ret %u\n", ret);
        return ERROR;
    }

    ret = BoardKpuDevBend();
    if (EOK != ret)
    {
        KPrintf("board_kpu_Init error ret %u\n", ret);
        return ERROR;
    }
    return ret;
}