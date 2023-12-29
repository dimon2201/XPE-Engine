namespace xpe {

    namespace core {

        cThread::cThread(u32 id, std::thread &thread) : m_ID(id), m_Thread(thread) {}

        u32 cThread::CurrentID()
        {
            return std::hash<std::thread::id>{}(std::this_thread::get_id());
        }

        void cThread::CurrentSleep(u32 millis)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(millis));
        }

    }

}