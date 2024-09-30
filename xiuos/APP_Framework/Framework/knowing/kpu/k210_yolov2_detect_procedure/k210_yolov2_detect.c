#include "k210_yolov2_detect.h"

#include "cJSON.h"
#include "dvp.h"
#ifdef USING_YOLOV2_JSONPARSER
#include <json_parser.h>
#endif
#include "region_layer.h"
#define STACK_SIZE (128 * 1024)
#define THREAD_PRIORITY_D (11)

static dmac_channel_number_t dma_ch = DMAC_CHANNEL_MAX - 1;
static _ioctl_shoot_para shoot_para_t = {0};

static pthread_t tid = 0;
static void *thread_detect_entry(void *parameter);
static int camera_fd = 0;
static int kmodel_fd = 0;
static int kpu_fd = 0;
static int if_exit = 0;
static unsigned char *showbuffer = NULL;
static unsigned char *kpurgbbuffer = NULL;
static int image_width = IMAGE_WIDTH;
static int image_height = IMAGE_HEIGHT;

unsigned char *model_data = NULL; // kpu data  load memory
unsigned char *model_data_align = NULL;

kpu_model_context_t detect_task;
static region_layer_t detect_rl;
static obj_info_t detect_info;
volatile uint32_t g_ai_done_flag;

static void ai_done(void *ctx) { g_ai_done_flag = 1; }


void k210_detect(char *json_file_path)
{
    int ret = 0;
    int result = 0;
    int size = 0;
    char kmodel_path[127] = {};

    // open and parse from json file
    yolov2_params_t detect_params = param_parse(json_file_path);
    if (!detect_params.is_valid)
    {
        return;
    }

#ifdef ADD_XIZI_FEATURES
    kpu_fd = PrivOpen(KPU_DEV_DRIVER, O_RDONLY);
    if (camera_fd < 0)
    {
        printf("open %s fail !!", KPU_DEV_DRIVER);
        return;
    }    
#endif

    printf("select camera device name:%s\n", CAMERA_DEV_DRIVER);
    camera_fd = PrivOpen(CAMERA_DEV_DRIVER, O_RDONLY);
    if (camera_fd < 0)
    {
        printf("open %s fail !!", CAMERA_DEV_DRIVER);
        return;
    }

    // configure the resolution of camera
    _ioctl_set_reso set_dvp_reso = {detect_params.sensor_output_size[1], detect_params.sensor_output_size[0]};
    struct PrivIoctlCfg camera_cfg;
    camera_cfg.args = &set_dvp_reso;
    camera_cfg.ioctl_driver_type = CAMERA_TYPE;
    PrivIoctl(camera_fd, IOCTRL_CAMERA_OUT_SIZE_RESO, &camera_cfg);
    image_height = set_dvp_reso.height;
    image_width = set_dvp_reso.width;

    // alloc the memory for camera and kpu running
    model_data = (unsigned char *)malloc(detect_params.kmodel_size + 255);
    if (NULL == model_data)
    {
        free(showbuffer);
        free(kpurgbbuffer);
        PrivClose(camera_fd);
        printf("model_data apply memory fail !!");
        return;
    }
    showbuffer = (unsigned char *)malloc(detect_params.sensor_output_size[0] * detect_params.sensor_output_size[1] * 2);
    if (NULL == showbuffer)
    {
        PrivClose(camera_fd);
        printf("showbuffer apply memory fail !!");
        return;
    }
    kpurgbbuffer = (unsigned char *)malloc(detect_params.net_input_size[0] * detect_params.net_input_size[1] * 3);
    if (NULL == kpurgbbuffer)
    {
        PrivClose(camera_fd);
        free(showbuffer);
        printf("kpurgbbuffer apply memory fail !!");
        return;
    }


    memset(model_data, 0, detect_params.kmodel_size + 255);
    memset(showbuffer, 0, detect_params.sensor_output_size[0] * detect_params.sensor_output_size[1] * 2);
    memset(kpurgbbuffer, 0, detect_params.net_input_size[0] * detect_params.net_input_size[1] * 3);
    shoot_para_t.pdata = (uintptr_t)(showbuffer);
    shoot_para_t.length = (size_t)(detect_params.sensor_output_size[0] * detect_params.sensor_output_size[1] * 2);

    /*
        load memory
    */
    // kmodel path generate from json file path, *.json -> *.kmodel
    memcpy(kmodel_path, json_file_path, strlen(json_file_path));
    int idx_suffix_start = strlen(json_file_path) - 4;
    const char kmodel_suffix[7] = "kmodel";
    int kmodel_suffix_len = 6;
    while (kmodel_suffix_len--)
    {
        kmodel_path[idx_suffix_start + 5 - kmodel_suffix_len] = kmodel_suffix[5 - kmodel_suffix_len];
    }
    printf("kmodel path: %s\n", kmodel_path);
    unsigned char *model_data_align = (unsigned char *)(((uintptr_t)model_data + 255) & (~255));
    printf("model address:%x->%x\n", model_data_align, model_data_align + detect_params.kmodel_size);

    kmodel_fd = PrivOpen(kmodel_path, O_RDONLY);

    if (kmodel_fd < 0)
    {
        printf("open kmodel fail");
        PrivClose(camera_fd);
        free(showbuffer);
        free(kpurgbbuffer);
        free(model_data);
        return;
    }
    else
    {
        size = PrivRead(kmodel_fd, model_data_align, detect_params.kmodel_size);
        if (size != detect_params.kmodel_size)
        {
            printf("read kmodel error size %d\n", size);
            PrivClose(camera_fd);
            PrivClose(kmodel_fd);
            free(showbuffer);
            free(kpurgbbuffer);
            free(model_data);
            return;
        }
        else
        {
            PrivClose(kmodel_fd);
            printf("read kmodel success \n");
        }
    }

#ifdef ADD_RTTHREAD_FEATURES
    dvp_set_ai_addr(
        (uintptr_t)(kpurgbbuffer +
                    detect_params.net_input_size[1] * (detect_params.net_input_size[0] - detect_params.sensor_output_size[0])),
        (uintptr_t)(kpurgbbuffer +
                    detect_params.net_input_size[1] * (detect_params.net_input_size[0] - detect_params.sensor_output_size[0]) +
                    detect_params.net_input_size[0] * detect_params.net_input_size[1]),
        (uintptr_t)(kpurgbbuffer +
                    detect_params.net_input_size[1] * (detect_params.net_input_size[0] - detect_params.sensor_output_size[0]) +
                    detect_params.net_input_size[0] * detect_params.net_input_size[1] * 2));
#else
    // Set AI buff address of Camera
    RgbAddress ai_address_preset;
    ai_address_preset.r_addr = (uintptr_t)kpurgbbuffer + detect_params.net_input_size[1];
    ai_address_preset.g_addr = ai_address_preset.r_addr + detect_params.net_input_size[0] * detect_params.net_input_size[1];
    ai_address_preset.b_addr = ai_address_preset.g_addr + detect_params.net_input_size[0] * detect_params.net_input_size[1];
    camera_cfg.args = &ai_address_preset;
    PrivIoctl(camera_fd, SET_AI_ADDR, &camera_cfg);
#endif

    // Load kmodel into kpu task
#ifdef ADD_RTTHREAD_FEATURES
    if (kpu_load_kmodel(&detect_task, model_data_align) != 0)
    {
        printf("\nmodel init error\n");
        PrivClose(camera_fd);
        PrivClose(kmodel_fd);
        free(showbuffer);
        free(kpurgbbuffer);
        free(model_data);
        return;
    }
#else
    struct PrivIoctlCfg kpu_cfg;
    kpu_cfg.args = model_data_align;
    kpu_cfg.ioctl_driver_type = KPU_TYPE;
    if (PrivIoctl(kpu_fd,LOAD_KMODEL,&kpu_cfg) != 0)
    {
        printf("\nmodel init error\n");
        PrivClose(camera_fd);
        PrivClose(kmodel_fd);
        free(showbuffer);
        free(kpurgbbuffer);
        free(model_data);
        return;
    }
#endif

    detect_rl.anchor_number = ANCHOR_NUM;
    detect_rl.anchor = detect_params.anchor;
    detect_rl.nms_value = detect_params.nms_thresh;
    detect_rl.classes = detect_params.class_num;
    result = region_layer_init(&detect_rl, detect_params.net_output_shape[0], detect_params.net_output_shape[1],
                               detect_params.net_output_shape[2], detect_params.net_input_size[1], detect_params.net_input_size[0]);
    printf("region_layer_init result %d \n\r", result);
    for (int idx = 0; idx < detect_params.class_num; idx++)
    {
        detect_rl.threshold[idx] = detect_params.obj_thresh[idx];
    }

    size_t stack_size = STACK_SIZE;
    pthread_attr_t attr;                      /* 线程属性 */
    struct sched_param prio;                  /* 线程优先级 */
    prio.sched_priority = THREAD_PRIORITY_D;  /* 优先级设置为 11 */
    pthread_attr_init(&attr);                 /* 先使用默认值初始化属性 */
    pthread_attr_setschedparam(&attr, &prio); /* 修改属性对应的优先级 */
    pthread_attr_setstacksize(&attr, stack_size);

    /* 创建线程 1, 属性为 attr，入口函数是 thread_entry，入口函数参数是 1 */
    result = pthread_create(&tid, &attr, thread_detect_entry, &detect_params);
    if (0 == result)
    {
        printf("thread_detect_entry successfully!\n");
    }
    else
    {
        printf("thread_detect_entry failed! error code is %d\n", result);
        PrivClose(camera_fd);
    }
}
// #ifdef __RT_THREAD_H__
// MSH_CMD_EXPORT(detect, detect task);
// #endif

static void *thread_detect_entry(void *parameter)
{
#ifdef BSP_USING_LCD
    int lcd_fd = PrivOpen(KPU_LCD_DEV_DRIVER, O_RDWR);
    if (lcd_fd < 0)
    {
        printf("open lcd fd error:%d\n", lcd_fd);
    }
    LcdWriteParam graph_param;
    graph_param.type = LCD_DOT_TYPE;
    for (int i = 0; i < LCD_SIZE; i++)
    {
        graph_param.pixel_info.pixel_color = (uint16_t *)showbuffer;
        graph_param.pixel_info.x_startpos = 0;
        graph_param.pixel_info.y_startpos = i;
        graph_param.pixel_info.x_endpos = LCD_SIZE - 1;
        graph_param.pixel_info.y_endpos = graph_param.pixel_info.y_startpos;
        PrivWrite(lcd_fd, &graph_param, NULL_PARAMETER);
    }
#endif

    yolov2_params_t detect_params = *(yolov2_params_t *)parameter;
    struct PrivIoctlCfg camera_cfg;
    camera_cfg.ioctl_driver_type = CAMERA_TYPE;

    printf("thread_detect_entry start!\n");
    int ret = 0;
    while (1)
    {
        g_ai_done_flag = 0;

        // get a graph map from camera
        camera_cfg.args = &shoot_para_t;
        ret = PrivIoctl(camera_fd, IOCTRL_CAMERA_START_SHOT, &camera_cfg);
        if (EOF == ret)
        {
            printf("ov2640 can't wait event flag");
            free(showbuffer);
            PrivClose(camera_fd);
            pthread_exit(NULL);
            return NULL;
        }

        int shoot_flag = 0;
        camera_cfg.args = (int *)&shoot_flag;
        PrivIoctl(camera_fd, FLAG_CHECK, &camera_cfg);
        while (shoot_flag == 2)
            PrivIoctl(camera_fd, FLAG_CHECK, &camera_cfg);

#ifdef ADD_RTTHREAD_FEATURES
        if (dmalock_sync_take(&dma_ch, 2000))
        {
            printf("Fail to take DMA channel");
        }
        kpu_run_kmodel(&detect_task, kpurgbbuffer, DMAC_CHANNEL5, ai_done, NULL);
        while (!g_ai_done_flag)
            ;
        dmalock_release(dma_ch);
#elif defined ADD_XIZI_FEATURES
        struct PrivIoctlCfg kpu_cfg;
        kpu_cfg.args = kpurgbbuffer;
        kpu_cfg.ioctl_driver_type = KPU_TYPE;
        PrivIoctl(kpu_fd,RUN_KMODEL,&kpu_cfg);

        int wait_flag=0;
        kpu_cfg.args = &wait_flag;
        while (0==wait_flag){
            PrivIoctl(kpu_fd,WAIT_FLAG,&kpu_cfg);
        }
#endif

#ifdef ADD_RTTHREAD_FEATURES
        float *output;
        size_t output_size;
        kpu_get_output(&detect_task, 0, (uint8_t **)&output, &output_size);
        detect_rl.input = output;
#else
        KpuOutputBuffer output_buffer;
        kpu_cfg.args = &output_buffer;
        PrivIoctl(kpu_fd,GET_OUTPUT,&kpu_cfg);
        detect_rl.input = (float*)(output_buffer.buffer);
#endif
        region_layer_run(&detect_rl, &detect_info);
        printf("detect_info.obj_number:%d\n", detect_info.obj_number);
        /* display result */
        for (int cnt = 0; cnt < detect_info.obj_number; cnt++)
        {
            detect_info.obj[cnt].y1 += (detect_params.sensor_output_size[0] - detect_params.net_input_size[0]) / 2;
            detect_info.obj[cnt].y2 += (detect_params.sensor_output_size[0] - detect_params.net_input_size[0]) / 2;
            draw_edge((uint32_t *)showbuffer, &detect_info, cnt, 0xF800, (uint16_t)detect_params.sensor_output_size[1],
                      (uint16_t)detect_params.sensor_output_size[0]);
            printf("%d: (%d, %d, %d, %d) cls: %s conf: %f\t", cnt, detect_info.obj[cnt].x1, detect_info.obj[cnt].y1,
                   detect_info.obj[cnt].x2, detect_info.obj[cnt].y2, detect_params.labels[detect_info.obj[cnt].class_id],
                   detect_info.obj[cnt].prob);
        }
#ifdef BSP_USING_LCD

#ifdef ADD_RTTHREAD_FEATURES
        extern void lcd_draw_picture(uint16_t x1, uint16_t y1, uint16_t width, uint16_t height, uint32_t * ptr);
        lcd_draw_picture(0, 0, (uint16_t)detect_params.sensor_output_size[1] - 1,
                         (uint16_t)detect_params.sensor_output_size[0] - 1, (uint32_t *)showbuffer);
        // lcd_show_image(0, 0, (uint16_t)detect_params.sensor_output_size[1], (uint16_t)detect_params.sensor_output_size[0],
        // (unsigned int *)showbuffer);
#else
        // refresh the LCD using photo of camera
        for (int i = 0; i < image_height; i++)
        {
            graph_param.pixel_info.pixel_color = (uint16_t *)showbuffer + (image_height-i-1) * image_width;
            graph_param.pixel_info.x_startpos = (LCD_SIZE - image_width) / 2;
            graph_param.pixel_info.y_startpos = i + (LCD_SIZE - image_height) / 2;
            graph_param.pixel_info.x_endpos = LCD_SIZE - 1 - (LCD_SIZE - image_width) / 2;
            graph_param.pixel_info.y_endpos = graph_param.pixel_info.y_startpos;
            PrivWrite(lcd_fd, &graph_param, NULL_PARAMETER);
        }
#endif

#endif
        if (1 == if_exit)
        {
            if_exit = 0;
            printf("thread_detect_entry exit");
            pthread_exit(NULL);
        }
    }
}

void detect_delete()
{
    if (showbuffer != NULL)
    {
        int ret = 0;
        PrivClose(camera_fd);
        PrivClose(kmodel_fd);
        free(showbuffer);
        free(kpurgbbuffer);
        free(model_data);
        printf("detect task cancel!!! ret %d ", ret);
        if_exit = 1;
    }
}
