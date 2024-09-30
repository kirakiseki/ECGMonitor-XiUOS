# YOLO Framework for K210 KPU

## Usage

* Include all src and header there in menuconfig, and copy codes like below in *main.c* in folder *Application*.
* In board *edu-riscv64*, if all operations are right, the LCD should light and show the image from camera and YOLO inference.

```C
#include <stdio.h>
#include <string.h>
// #include <user_api.h>
#include <transform.h>


extern int FrameworkInit();
extern void ApplicationOtaTaskInit(void);
extern void k210_detect(char *json_file_path);

int main(void)
{
	printf("Hello, world! \n");
	FrameworkInit();
#ifdef APPLICATION_OTA
	ApplicationOtaTaskInit();
#endif
	k210_detect("instrusion.json");
    return 0;
}
```

## TODO

* KPU drivers and frameworks are still not completed, because it is undefined how to reisgter KPU's bus and device name in rt-fusion system. The framework is still directly using the SDK with *ifdef* From Canaan Inc. But after the driver completed, it will be easy to adapt framework for the APIs.
* Camera and LCD drivers between RT and XIZI are not completely compatible. So there are some marcos like *ifdef* to make the framework compatible for both systems.
* After completed, all codes from kernel in knowing framework should be posix interfaces,like *PrivOpen*、*PrivIoctrl*、*PrivRead*.
