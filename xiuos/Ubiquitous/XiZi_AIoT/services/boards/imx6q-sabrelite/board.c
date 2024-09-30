#include <isr.h>
extern void platform_init(void);
extern void print_version(void);
void init()
{
    SysInitIsrManager();
    platform_init();

    print_version();
}