namespace xpe {

    namespace core {

        static Ref<spdlog::logger> s_Logger;
        static Ref<spdlog::logger> s_Tracer;
        static Ref<spdlog::logger> s_MemLogger;
        // [DateTime][Hours:Minutes:Seconds:Milliseconds]: Message
        static const char* s_LogPattern = "[%D][%H:%M:%S.%e]: %^%v%$";
        // [DateTime][Hours:Minutes:Seconds:Milliseconds] FunctionName(FileName:CodeLine) Message
        static const char* s_TracePattern = "[%D][%H:%M:%S.%e] %^%!(%s:%#) %v%$";

        using LogSinkColor = spdlog::sinks::stdout_color_sink_mt;
        using ErrorSinkColor = spdlog::sinks::stderr_color_sink_mt;
        using FileSink = spdlog::sinks::basic_file_sink_mt;
        using Sink = spdlog::sink_ptr;

        static Ref<spdlog::logger> CreateLogger(const char* logName, const char* filepath, const char* pattern, int backtrace) {

            // setup console color sink
            Ref<LogSinkColor> consoleSink;
            consoleSink.Create();
            consoleSink->set_pattern(pattern);

            // setup file sink
            Ref<FileSink> fileSink;
            fileSink.Create(filepath, true);
            fileSink->set_pattern(pattern);

            core::vector<Sink> sinks = { consoleSink.GetPtr(), fileSink.GetPtr() };

            // setup logger
            Ref<spdlog::logger> logger;
            logger.Create(logName, std::begin(sinks), std::end(sinks));
            spdlog::register_logger(logger.GetPtr());
            logger->enable_backtrace(backtrace);

            return logger;
        }

        void Logger::Init(const LoggerDescriptor& descriptor) {
            const char* name = descriptor.Name;
            const int backtrace = descriptor.Backtrace;
            hstringstream ss;

            try {
                ss << name << "_Logger";
                hstring logName = ss.str();
                ss = {};
                ss << "logs/" << name << ".log";
                hstring filepath = ss.str();
                ss = {};
                s_Logger = CreateLogger(logName.c_str(), filepath.c_str(), s_LogPattern, backtrace);

                ss << name << "_Tracer";
                logName = ss.str();
                ss = {};
                ss << "logs/" << name << ".trace";
                filepath = ss.str();
                ss = {};
                s_Tracer = CreateLogger(logName.c_str(), filepath.c_str(), s_TracePattern, backtrace);

                ss << name << "_Memory";
                logName = ss.str();
                ss = {};
                ss << "logs/" << name << ".memory";
                filepath = ss.str();
                ss = {};
                s_MemLogger = CreateLogger(logName.c_str(), filepath.c_str(), s_LogPattern, backtrace);
            }

            catch (const spdlog::spdlog_ex &ex) {
                SPDLOG_ERROR("Failed to initialize log {}. Error: {}", name, ex.what());
            }
        }

        void Logger::Free() {
            s_Tracer->flush();
            s_Logger->flush();
            s_MemLogger->flush();
        }

        void Logger::DumpBacktrace() {
            s_Tracer->dump_backtrace();
        }

        spdlog::logger* Logger::GetLogger() {
            return s_Logger.Get();
        }

        spdlog::logger* Logger::GetTracer() {
            return s_Tracer.Get();
        }

        spdlog::logger* Logger::GetMemLogger() {
            return s_MemLogger.Get();
        }

    }

}