#include "sysctl.h"
#include <sensor.h>

#define SAMPLES_COUNT 5000

void ECGADC_ADS1115(void)
{
    static uint16_t dat[SAMPLES_COUNT];
    int32_t value;
    struct SensorQuantity *adc = SensorQuantityFind(SENSOR_QUANTITY_ADS1115_ECGADC, SENSOR_QUANTITY_ADC);
    SensorQuantityOpen(adc);
    
    int cnt = 0;
    uint64_t start = read_cycle();

    while (cnt<SAMPLES_COUNT)
    {
        value = SensorQuantityReadValue(adc);
        dat[cnt++] = value;
    }

    uint64_t elapsed = read_cycle() - start;
    double seconds = (double)elapsed/(sysctl_cpu_get_freq());

    for(int i = 0; i < SAMPLES_COUNT; i++)
    {
        printf("%d,", dat[i]);
    }
    printf("\n");

    printf("Sensor Framework:\n");
    printf("Time used: %f\n", seconds);
    printf("SPS: %f\n", SAMPLES_COUNT / seconds);
    
    SensorQuantityClose(adc);
}