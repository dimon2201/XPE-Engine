namespace xpe {

    namespace core {

        MemoryStats Hardware::s_MemoryStats;
        VideoStats Hardware::s_VideoStats;
        CpuStats Hardware::s_CpuStats;

        MemoryStats Hardware::GetMemoryStats() {
            return s_MemoryStats;
        }

        VideoStats Hardware::GetVideoStats() {
            return s_VideoStats;
        }

        CpuStats Hardware::GetCpuStats() {
            return s_CpuStats;
        }

    }

}