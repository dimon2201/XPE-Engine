namespace xpe {

    namespace os {

        MemoryStats HardwareConfig::s_MemoryStats;
        VideoStats HardwareConfig::s_VideoStats;

        MemoryStats HardwareConfig::GetMemoryStats() {
            return s_MemoryStats;
        }

        VideoStats HardwareConfig::GetVideoStats() {
            return s_VideoStats;
        }

    }

}