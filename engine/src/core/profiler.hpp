#pragma once

#include <chrono>

namespace xpe
{
    namespace core
    {
        class xCPUProfiler
        {
            public:
                xCPUProfiler(float* output)
                {
                    _start = std::chrono::system_clock::now();
                    _output = output;
                }

                ~xCPUProfiler()
                {
                    _end = std::chrono::system_clock::now();
                    *_output = std::chrono::duration_cast<std::chrono::duration<float>>(_end - _start).count();
                }

            private:
                float* _output;
                std::chrono::system_clock::time_point _start;
                std::chrono::system_clock::time_point _end;
        };
    }
}