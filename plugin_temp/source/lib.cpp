#include <stdio.h>
#include <string.h>
#include <sstream>

#include <orbis/libkernel.h>

#include "../../common/plugin_common.h"
#include "../../common/notify.h"

attr_public const char* g_pluginName = "plugin_example";
attr_public const char* g_pluginDesc = "Demonstrate usage of CXX in module. Based from OpenOrbis `library_example`";
attr_public const char* g_pluginAuth = "illusiony";
attr_public uint32_t g_pluginVersion = 0x00000100;  // 1.00

uint32_t (*sceKernelGetCpuTemperature)(uint32_t *);
uint32_t (*sceKernelGetSocSensorTemperature)(uint32_t, uint32_t *);

extern "C" int plugin_load(int* argc, const char** argv)
{
    while (1)
        {
            uint32_t CPU_Temp, SOC_Temp;
            sceKernelGetCpuTemperature(&CPU_Temp);
            sceKernelGetSocSensorTemperature(0, &SOC_Temp);
            printf_notification3("*Cpu: %d*C\n*Soc: %i*C", CPU_Temp, SOC_Temp);
            sceKernelSleep(100);
        }
    return 0;
}
