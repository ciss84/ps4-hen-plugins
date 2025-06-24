void* pthread_temp(void* args)
{
    (void)args;
    uint32_t (*sceKernelGetCpuTemperature)(uint32_t *);
    uint32_t (*sceKernelGetSocSensorTemperature)(uint32_t, uint32_t *);
    while (1)
    {
        uint32_t CPU_Temp, SOC_Temp;
        sceKernelGetCpuTemperature(&CPU_Temp);
        sceKernelGetSocSensorTemperature(0, &SOC_Temp); 
        Notify(TEX_ICON_SYSTEM,"*Cpu: %d*C\n*Soc: %i*C", CPU_Temp, SOC_Temp);}
        sleep(100);
    }
    return 0;
}