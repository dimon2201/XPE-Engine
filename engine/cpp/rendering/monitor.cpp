#include <rendering/monitor.h>

namespace xpe {

    namespace render {

        Monitor* Monitor::s_Instance = nullptr;

        Monitor& Monitor::Get() {
            if (s_Instance == nullptr) {
                Init();
            }
            return *s_Instance;
        }

        void Monitor::Init() {
            s_Instance = new Monitor();
        }

        void Monitor::Free() {
            delete s_Instance;
        }

        void Monitor::Flush() {
            m_Buffer.FlushItem(*this);
        }

    }

}