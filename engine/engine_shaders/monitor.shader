struct MonitorData
{
    float Exposure;
    float Gamma;
};

StructuredBuffer<MonitorData> Monitors : K_SLOT_MONITOR;

MonitorData Monitor()
{
    return Monitors[0];
}