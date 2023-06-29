namespace xpe {

    namespace core {

        static Ref<spdlog::logger> s_logger;
        static Ref<spdlog::logger> s_tracer;
        // [DateTime][Hours:Minutes:Seconds:Milliseconds]: Message
        static const char* s_logPattern = "[%D][%H:%M:%S.%e]: %^%v%$";
        // [DateTime][Hours:Minutes:Seconds:Milliseconds] FunctionName(FileName:CodeLine) Message
        static const char* s_tracePattern = "[%D][%H:%M:%S.%e] %^%!(%s:%#) %v%$";

        using LogSinkColor = spdlog::sinks::stdout_color_sink_mt;
        using ErrorSinkColor = spdlog::sinks::stdout_color_sink_mt;
        using FileSink = spdlog::sinks::basic_file_sink_mt;
        using Sink = spdlog::sink_ptr;

        static Ref<spdlog::logger> createLogger(const char* logName, const char* filepath, const char* pattern, int backtrace) {

            // setup console color sink
            Ref<LogSinkColor> consoleSink = createRef<LogSinkColor>();
            consoleSink->set_pattern(pattern);

            // setup file sink
            Ref<FileSink> fileSink = createRef<FileSink>(filepath, true);
            fileSink->set_pattern(pattern);

            std::vector<Sink> sinks = { consoleSink, fileSink };

            // setup logger
            Ref<spdlog::logger> logger = createRef<spdlog::logger>(logName, std::begin(sinks), std::end(sinks));
            spdlog::register_logger(logger);
            logger->enable_backtrace(backtrace);

            return logger;
        }

        void Logger::Init(const LoggerDescriptor& descriptor) {
            const char* name = descriptor.Name;
            const int backtrace = descriptor.Backtrace;
            std::stringstream ss;

            try {
                ss << name << "_Logger";
                std::string logName = ss.str();
                ss = {};
                ss << "logs/" << name << ".log";
                std::string filepath = ss.str();
                ss = {};
                s_logger = createLogger(logName.c_str(), filepath.c_str(), s_logPattern, backtrace);

                ss << name << "_Tracer";
                logName = ss.str();
                ss = {};
                ss << "logs/" << name << ".trace";
                filepath = ss.str();
                ss = {};
                s_tracer = createLogger(logName.c_str(), filepath.c_str(), s_tracePattern, backtrace);
            }
            catch (const spdlog::spdlog_ex &ex) {
                SPDLOG_ERROR("Failed to initialize log {}. Error: {}", name, ex.what());
            }
        }

        void Logger::Free() {
            s_logger.reset();
            s_tracer.reset();
        }

        void Logger::DumpBacktrace() {
            s_tracer->dump_backtrace();
        }

        spdlog::logger* Logger::GetLogger() {
            return s_logger.get();
        }

        spdlog::logger* Logger::GetTracer() {
            return s_tracer.get();
        }

    }

}