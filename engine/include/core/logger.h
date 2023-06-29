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

#define log_trace(...) SPDLOG_LOGGER_TRACE(xpe::core::Logger::GetTracer(), __VA_ARGS__)
#define log_verbose(...) SPDLOG_LOGGER_TRACE(xpe::core::Logger::GetLogger(), __VA_ARGS__)
#define log_info(...) SPDLOG_LOGGER_INFO(xpe::core::Logger::GetLogger(), __VA_ARGS__)
#define log_warning(...) SPDLOG_LOGGER_WARN(xpe::core::Logger::GetTracer(), __VA_ARGS__)
#define log_error(...) SPDLOG_LOGGER_ERROR(xpe::core::Logger::GetTracer(), __VA_ARGS__)

#define dump_trace_error(...) \
log_err(__VA_ARGS__); \
Logger::DumpBacktrace()

#define log_delta(dt) \
log_info("Delta time: {} seconds", dt); \
log_info("FPS: {}", 1 / dt)

#define log_glm(name, v) \
log_info("{}:{}", name, glm::to_string(v))

#else

#define log_trace(...)
#define log_verbose(...)
#define log_info(...)
#define log_warning(...)
#define log_error(...)

#define dump_trace_error(...)

#define log_delta(dt)

#define log_glm(name, v)

#endif