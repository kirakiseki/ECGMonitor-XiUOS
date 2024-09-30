## Tiny TTF font engine
用于在触摸屏上渲染矢量字体，若要使用该引擎，需开启LVGL。
### Usage
在`lv_conf.h`中开启`LV_USE_TINY_TTF`,可以使用`lv_tiny_ttf_create_data(data, data_size, line_height)`创建字体，创建字体后像LVGL正常字体一样使用。
默认情况下，TTF或者OTF文件必须作为数组嵌入到程序，见`ubuntu_font`，如果要从sd卡中读取文件，需开启`LV_TINY_TTF_FILE_SUPPORT`和`LV_USE_FS_POSIX`。  

> 注意：从sd卡中读取字库文件会比较慢，请谨慎使用！   

默认情况下，字体使用4KB缓存来加速渲染，可以使用`lv_tiny_ttf_create_data_ex(data, data_size, line_height, cache_size)`或者`lv_tiny_ttf_create_file_ex(path, line_height, cache_size)`更改此大小
。
### API
见 xiuos/APP_Framework/lib/lvgl/src/extra/libs/tiny_ttf/lv_tiny_ttf.h