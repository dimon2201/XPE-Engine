struct MonitorData
{
    float Exposure;
    float Gamma;
};

StructuredBuffer<MonitorData> Monitors : K_SLOT_MONITOR;

float GetGamma()
{
    return Monitors[0].Gamma;
}

float GetExposure()
{
    return Monitors[0].Exposure;
}