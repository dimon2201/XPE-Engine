#pragma once

namespace xpe
{
    namespace core
    {
        class ENGINE_API Timer
        {
            public:
                Timer(Time* output)
                {
                    _start = std::chrono::system_clock::now();
                    _output = output;
                }

                ~Timer()
                {
                    _end = std::chrono::system_clock::now();
                    *_output = std::chrono::duration_cast<std::chrono::duration<float>>(_end - _start).count() * 1000;
                }

            private:
                Time* _output;
                std::chrono::system_clock::time_point _start;
                std::chrono::system_clock::time_point _end;
        };
    }
}