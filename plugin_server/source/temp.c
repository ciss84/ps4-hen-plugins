#include <stdint.h>
#include <stdio.h>
#include "../../common/plugin_common.h"
#include "../../common/notify.h"

void* pthread_temp(void* args)
{
    (void)args;
    int (*sceKernelGetCpuTemperature)(int *);
    int (*sceKernelGetSocSensorTemperature)(int, int *);
    while (1)
    {
        int CPU_Temp, SOC_Temp;
        sceKernelGetCpuTemperature(&CPU_Temp);
        sceKernelGetSocSensorTemperature(0, &SOC_Temp); 
        Notify(TEX_ICON_SYSTEM,"*Cpu: %d*C\n*Soc: %i*C", CPU_Temp, SOC_Temp);}
        sleep(100);
    }
    return 0;
}