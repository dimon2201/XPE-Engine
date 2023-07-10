#pragma once

namespace xpe
{
    namespace core
    {

        using namespace std::chrono;

        class ENGINE_API Timer
        {
            public:
                Timer(Time* output)
                {
                    _start = system_clock::now();
                    _output = output;
                }

                ~Timer()
                {
                    _end = system_clock::now();
                    *_output = static_cast<float>(duration_cast<duration<float>>(_end - _start).count() * 1000);
                }

            public:
                Time GetStartTime() {
                    return static_cast<float>(duration_cast<duration<float>>(_start.time_since_epoch()).count() * 1000);
                }

                Time GetEndTime() {
                    return static_cast<float>(duration_cast<duration<float>>(_end.time_since_epoch()).count() * 1000);
                }

            private:
                Time* _output;
                system_clock::time_point _start;
                system_clock::time_point _end;
        };
    }
}