#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>

#ifdef DEBUG

#define InitLogger(desc) xpe::core::cLogManager::Init(desc)
#define FreeLogger() xpe::core::cLogManager::Free()

#define LogTrace(...) SPDLOG_LOGGER_TRACE(xpe::core::cLogManager::GetTracer(), __VA_ARGS__)
#define LogVerbose(...) SPDLOG_LOGGER_TRACE(xpe::core::cLogManager::GetLogger(), __VA_ARGS__)
#define LogInfo(...) SPDLOG_LOGGER_INFO(xpe::core::cLogManager::GetLogger(), __VA_ARGS__)
#define LogWarning(...) SPDLOG_LOGGER_WARN(xpe::core::cLogManager::GetLogger(), __VA_ARGS__)
#define LogError(...) SPDLOG_LOGGER_ERROR(xpe::core::cLogManager::GetTracer(), __VA_ARGS__)
#define LogMemory(...) SPDLOG_LOGGER_INFO(xpe::core::cLogManager::GetMemLogger(), __VA_ARGS__)

#define DumpTraceError(...) \
LogError(__VA_ARGS__); \
cLogManager::DumpBacktrace()

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
#define LogMemory(...)

#define DumpTraceError(...)

#define LogTime(time)

#define LogGLM(name, v)

#endif

namespace xpe {

    namespace core {

        struct ENGINE_API sLoggerDescriptor final {
            const char* Name;
            int Backtrace;
        };

        class ENGINE_API cLogManager final {

        public:
            static void Init(const sLoggerDescriptor& descriptor);
            static void Free();

            static spdlog::logger* GetLogger();
            static spdlog::logger* GetTracer();
            static spdlog::logger* GetMemLogger();

            static void DumpBacktrace();

        };

    }

}