namespace xpe {

    namespace core {

        cTime cTimeManager::Now() {
            auto time = system_clock::now();
            return Cast(time);
        }

        cTime cTimeManager::Cast(const system_clock::time_point& time) {
            return static_cast<float>(duration_cast<duration<float>>(time.time_since_epoch()).count() * 1000);
        }

    }

}