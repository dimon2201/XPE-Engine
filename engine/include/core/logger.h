#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>

namespace xpe {

    namespace core {

        struct ENGINE_API LoggerDescriptor final {
            const char* Name;
            int Backtrace;
        };

        class ENGINE_API Logger final {

        public:
            static void Init(const LoggerDescriptor& descriptor);
            static void Free();

            static spdlog::logger* GetLogger();
            static spdlog::logger* GetTracer();

            static void DumpBacktrace();

        };

    }

}

#ifdef DEBUG

#define InitLogger(desc) xpe::core::Logger::Init(desc)
#define FreeLogger() xpe::core::Logger::Free()

#define LogTrace(...) SPDLOG_LOGGER_TRACE(xpe::core::Logger::GetTracer(), __VA_ARGS__)
#define LogVerbose(...) SPDLOG_LOGGER_TRACE(xpe::core::Logger::GetLogger(), __VA_ARGS__)
#define LogInfo(...) SPDLOG_LOGGER_INFO(xpe::core::Logger::GetLogger(), __VA_ARGS__)
#define LogWarning(...) SPDLOG_LOGGER_WARN(xpe::core::Logger::GetTracer(), __VA_ARGS__)
#define LogError(...) SPDLOG_LOGGER_ERROR(xpe::core::Logger::GetTracer(), __VA_ARGS__)

#define DumpTraceError(...) \
LogError(__VA_ARGS__); \
Logger::DumpBacktrace()

#define LogGLM(name, v) \
LogInfo("{}: {}", name, glm::to_string(v))

#else

#define InitLogger(desc)
#define FreeLogger()

#define LogTrace(...)
#define LogVerbose(...)
#define LogInfo(...)
#define LogWarning(...)
#define LogError(...)

#define DumpTraceError(...)

#define LogTime(time)

#define LogGLM(name, v)

#endif