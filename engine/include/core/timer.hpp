#pragma once

using namespace std::chrono;

namespace xpe
{
    namespace core
    {

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
                    *_output = GetDeltaTime();
                }

            public:
                static Time Now();
                static Time Cast(const system_clock::time_point& time);

            public:
                inline Time GetStartTime() {
                    return Cast(_start);
                }

                inline Time GetEndTime() {
                    return Cast(_end);
                }

                inline Time GetDeltaTime() {
                    return static_cast<float>(duration_cast<duration<float>>(_end - _start).count() * 1000);
                }

            private:
                Time* _output;
                system_clock::time_point _start;
                system_clock::time_point _end;
        };
    }
}