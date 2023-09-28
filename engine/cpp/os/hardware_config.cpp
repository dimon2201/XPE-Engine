namespace xpe {

    namespace os {

        MemoryStats Hardware::s_MemoryStats;
        VideoStats Hardware::s_VideoStats;

        MemoryStats Hardware::GetMemoryStats() {
            return s_MemoryStats;
        }

        VideoStats Hardware::GetVideoStats() {
            return s_VideoStats;
        }

    }

}