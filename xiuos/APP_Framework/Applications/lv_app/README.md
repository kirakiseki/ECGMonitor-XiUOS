## lv_sensor_update_info
用于在触摸屏上显示传感器的各项数据

更新显示数据可在线程中使用`bool sensor_update_val(double val, enum sensor_type st)`（保障线程安全）  
    - `val`: 传感器数值；  
    - `st`：检测值类型（如氧气、臭氧）；  
        - `st`类型为`enum sensor_type`，该枚举类型在lv_sensor_info.h中给出；   
    - 该函数的使用方法可参考lv_sensor_info_update_demo.c；   
    - 请勿直接修改lvgl table；  
``` C
enum sensor_type {
    O3 = 0, // 臭氧
    CO2,  // 二氧化碳
    SO2,  // 二氧化硫
    NO2,  // 二氧化氮
    N2,  // 氨气
    TVOC,  
    FORMALDEHYDE,  // 甲醛
    ALCOHOL,  // 乙醇
    METHANE,  // 甲烷
    O2,  // 氧气
    AQS,
    PM,  // PM1.0/2.5
    TEMPERATURE,  // 温度
    HUMIDITY,  // 湿度 
    WIND_SPEED,  // 风速
    WIND_DIRECTION,  //风向
    PRESURE,  // 压力
    NOISE  // 噪音
};
```
