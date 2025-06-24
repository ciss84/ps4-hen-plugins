#include <stdio.h>
#include <string.h>
#include <sstream>

#include <orbis/libkernel.h>

#include "../../common/plugin_common.h"
#include "../../common/notify.h"

attr_public const char* g_pluginName = "plugin_temp";
attr_public const char* g_pluginDesc = "Demonstrate usage of CXX in module. Based from OpenOrbis `library_example`";
attr_public const char* g_pluginAuth = "illusiony";
attr_public uint32_t g_pluginVersion = 0x00000100;  // 1.00

int temp;
uint32_t (*sceKernelGetCpuTemperature)(uint32_t *);
uint32_t (*sceKernelGetSocSensorTemperature)(uint32_t, uint32_t *);

extern "C" int plugin_load(int* argc, const char** argv)
{
  temp= 1;
  while (temp) {
  uint32_t CPU_Temp;
  uint32_t SOC_Temp;
  uint32_t ret = sceKernelGetCpuTemperature(&CPU_Temp);
  uint32_t retb = sceKernelGetSocSensorTemperature(0, &SOC_Temp);
  sleep(100);
  printf("returned %d\n%i", ret, retb);
  Notify(TEX_ICON_SYSTEM, "Temperature\n*Cpu: %d*C\n*Soc: %i*C", CPU_Temp, SOC_Temp);
  }
  return 0;      
}
