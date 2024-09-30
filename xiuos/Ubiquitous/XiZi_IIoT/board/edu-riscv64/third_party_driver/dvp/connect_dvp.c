#include <connect_dvp.h>
#include <dvp.h>
#include <board.h>
#include "sysctl.h"
#include "bsp.h"
#include "plic.h"
#include <ov2640.h>

#define CONTINOUS_SHOOTS 1
#define ONLY_ONE_SHOOT 2
#define STOP_SHOOT 0

static int shoot_flag = 0; // shoot will close when shoot_flag == 0

// irq interrupt function
static int on_irq_dvp(int irq, void *arg)
{
    if (dvp_get_interrupt(DVP_STS_FRAME_FINISH))
    {
        dvp_clear_interrupt(DVP_STS_FRAME_FINISH);
    }
    else{
        if(shoot_flag>0){
            dvp_start_convert();
            if(ONLY_ONE_SHOOT==shoot_flag){
                shoot_flag=STOP_SHOOT;
            }            
        }
        dvp_clear_interrupt(DVP_STS_FRAME_START);
    }

    return 0;
}

struct DvpRegConfigureInfo
{
    uint8_t device_addr;
    uint16_t reg_addr;
    uint8_t reg_value;
};

static struct CameraCfg sensor_config = {
    .output_h = IMAGE_HEIGHT,//will be resize from window below in ov2640
    .output_w = IMAGE_WIDTH,
    .window_h = 600,        //register configure in ov2640.h
    .window_w = 800,        //to make window as large as cmos selected size
    .window_xoffset = 0,
    .window_yoffset = 0,
    .gain_manu_enable = 0,
    .gain = 0x00
    };

static uint32 DvpDrvInit(void)
{
    x_err_t ret = EOK;
    dvp_init(SCCB_REG_LENGTH);
    dvp_set_xclk_rate(DVP_XCLK_RATE);
    dvp_set_image_format(DVP_CFG_RGB_FORMAT);
    dvp_set_image_size(IMAGE_WIDTH, IMAGE_HEIGHT);
    dvp_set_output_enable(DVP_OUTPUT_DISPLAY, 0);
    dvp_set_output_enable(DVP_OUTPUT_AI, 0);
    ov2640_init();
    SensorConfigure(&sensor_config);

    sysctl_set_spi0_dvp_data(1);
#ifdef DVP_BURST_ENABLE
    dvp_enable_burst();
#endif
#ifdef DVP_AUTO_ENABLE
    dvp_disable_auto();
#endif
#ifdef DVP_AI_OUTPUT
    dvp_set_output_enable(DVP_OUTPUT_AI, 1);
    // dvp_set_ai_addr((uint32_t)DVP_AI_RED_ADRESS, (uint32_t)DVP_AI_GREEN_ADRESS, (uint32_t)DVP_AI_BLUE_ADRESS);
#endif
#ifdef DVP_INTERRUPT_ENABLE
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
    isrManager.done->registerIrq(IRQN_DVP_INTERRUPT, (IsrHandlerType)on_irq_dvp, NULL);
    isrManager.done->enableIrq(IRQN_DVP_INTERRUPT);
    dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
    KPrintf("camera interrupt has open!\n");
#endif
    return ret;
}

static uint32 ReadDvpReg(void *drv, struct DvpRegConfigureInfo *reg_info)
{
    x_err_t ret = EOK;
    reg_info->reg_value = dvp_sccb_receive_data(reg_info->device_addr, reg_info->reg_addr);
    return ret;
}

static uint32 WriteDvpReg(void *drv, struct DvpRegConfigureInfo *reg_info)
{
    x_err_t ret = EOK;
    dvp_sccb_send_data(reg_info->device_addr, reg_info->reg_addr, reg_info->reg_value);
    return ret;
}

static uint32 DvpOpen(void *dev)
{
    x_err_t ret = EOK;
    DvpDrvInit();
    return ret;
}

static uint32 DvpClose(void *dev)
{
    x_err_t ret = EOK;
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
    dvp_set_output_enable(DVP_OUTPUT_AI, 0);
    dvp_set_output_enable(DVP_OUTPUT_DISPLAY, 0);
    return ret;
}

static uint32 DvpRead(void *dev, struct BusBlockReadParam *read_param)
{
    x_err_t ret = EOK;

    // change the output buff address by read
    dvp_set_output_enable(DVP_OUTPUT_DISPLAY, 0);
    dvp_set_display_addr((uintptr_t)read_param->buffer);
    dvp_set_output_enable(DVP_OUTPUT_DISPLAY, 1);
    shoot_flag=CONTINOUS_SHOOTS;
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);

    return ret;
}

/**
 * @brief configure api for dvp device
 * TODO: unified APIs to keep consistent with RT-thread 
 */
static uint32 DvpDrvConfigure(void *drv, struct BusConfigureInfo *args)
{
    x_err_t ret = EOK;
    
    int cmd_type = args->configure_cmd;
    struct CameraCfg* tmp_cfg;
    RgbAddress* kpu_rgb_address;
    _ioctl_shoot_para* pixel_cfg;
    int value = ((int*)args->private_data)[0];
    switch (cmd_type)
    {
    case OPE_INT:
        break;
    case OPE_CFG:
        tmp_cfg = (struct CameraCfg *)args->private_data;
        memcpy(&sensor_config,tmp_cfg,sizeof(struct CameraCfg));
        SensorConfigure(&sensor_config);
        dvp_set_image_size(tmp_cfg->output_w, tmp_cfg->output_h);
        break;
    case IOCTRL_CAMERA_START_SHOT:
        pixel_cfg = (_ioctl_shoot_para*)args->private_data;
        dvp_set_display_addr(pixel_cfg->pdata);
        dvp_set_output_enable(DVP_OUTPUT_DISPLAY, 1); 
        dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);
        shoot_flag=ONLY_ONE_SHOOT;
        break;
    case IOCTRL_CAMERA_OUT_SIZE_RESO:
        dvp_set_image_size(((uint32_t*)args->private_data)[0], ((uint32_t*)args->private_data)[1]);
        break;
    case FLAG_CHECK:
        *((int*)args->private_data) = shoot_flag;
        break;        
    case SET_AI_ADDR:
        kpu_rgb_address = (RgbAddress*)args->private_data;
        dvp_set_output_enable(DVP_OUTPUT_AI, 1);
        dvp_set_ai_addr(kpu_rgb_address->r_addr,kpu_rgb_address->g_addr,kpu_rgb_address->b_addr);
        break;

    // make compatible for rt-fusion xizi
    case IOCTRL_CAMERA_SET_LIGHT:
        ov2640_set_light_mode(value);
        break;
    case IOCTRL_CAMERA_SET_COLOR:
        ov2640_set_color_saturation(value);
        break;
    case IOCTRL_CAMERA_SET_BRIGHTNESS:
        ov2640_set_brightness(value);
        break;
    case IOCTRL_CAMERA_SET_CONTRAST:
        ov2640_set_contrast(value);
        break;
    case IOCTRL_CAMERA_SET_EFFECT:
        ov2640_set_special_effects(value);
        break;
    case IOCTRL_CAMERA_SET_EXPOSURE:
        ov2640_set_auto_exposure(value);
        break;

    case REG_SCCB_READ:
        ReadDvpReg(drv, (struct DvpRegConfigureInfo *)args->private_data);
        break;
    case REG_SCCB_WRITE:
        //for ov2640,write reg 0x04 to Horizontal mirror or Vertical flip
        WriteDvpReg(drv, (struct DvpRegConfigureInfo *)args->private_data);
        break;
    default:
        break;
    }
    return ret;
}

/*manage the camera device operations*/
static const struct CameraDevDone camera_dev_done =
    {
        .dev_open = DvpOpen,
        .dev_close = DvpClose,
        .dev_write = NONE,
        .dev_read = DvpRead,
};

/*Init camera bus*/
static int BoardCameraBusInit(struct CameraBus *camera_bus, struct CameraDriver *camera_driver)
{
    x_err_t ret = EOK;

    /*Init the camera bus */
    camera_bus->private_data = (void *)&camera_dev_done;
    ret = CameraBusInit(camera_bus, CAMERA_BUS_NAME);
    if (EOK != ret)
    {
        KPrintf("board_camera_init CameraBusInit error %d\n", ret);
        return ERROR;
    }

    /*Init the camera driver*/
    camera_driver->private_data = (void *)&camera_dev_done;
    ret = CameraDriverInit(camera_driver, CAMERA_DRV_NAME);
    if (EOK != ret)
    {
        KPrintf("board_camera_init CameraDriverInit error %d\n", ret);
        return ERROR;
    }

    /*Attach the camera driver to the camera bus*/
    ret = CameraDriverAttachToBus(CAMERA_DRV_NAME, CAMERA_BUS_NAME);
    if (EOK != ret)
    {
        KPrintf("board_camera_init CameraDriverAttachToBus error %d\n", ret);
        return ERROR;
    }

    return ret;
}

/*Attach the camera device to the camera bus*/
static int BoardCameraDevBend(void)
{
    x_err_t ret = EOK;
    static struct CameraHardwareDevice camera_device;
    memset(&camera_device, 0, sizeof(struct CameraHardwareDevice));

    camera_device.camera_dev_done = &camera_dev_done;

    ret = CameraDeviceRegister(&camera_device, NONE, CAMERA_DEVICE_NAME);
    if (EOK != ret)
    {
        KPrintf("board_camera_init CameraDeviceInit device %s error %d\n", CAMERA_DEVICE_NAME, ret);
        return ERROR;
    }

    ret = CameraDeviceAttachToBus(CAMERA_DEVICE_NAME, CAMERA_BUS_NAME);
    if (EOK != ret)
    {
        KPrintf("board_camera_init CameraDeviceAttachToBus device %s error %d\n", CAMERA_DEVICE_NAME, ret);
        return ERROR;
    }

    return ret;
}

int HwDvpInit(void)
{
    x_err_t ret = EOK;
    static struct CameraBus camera_bus;
    memset(&camera_bus, 0, sizeof(struct CameraBus));

    static struct CameraDriver camera_driver;
    memset(&camera_driver, 0, sizeof(struct CameraDriver));

    camera_driver.configure = DvpDrvConfigure;
    ret = BoardCameraBusInit(&camera_bus, &camera_driver);
    if (EOK != ret)
    {
        KPrintf("board_camera_Init error ret %u\n", ret);
        return ERROR;
    }

    ret = BoardCameraDevBend();
    if (EOK != ret)
    {
        KPrintf("board_camera_Init error ret %u\n", ret);
        return ERROR;
    }
    return ret;
}