namespace xpe {

    namespace core {

        static std::shared_ptr<spdlog::logger> s_Logger;
        static std::shared_ptr<spdlog::logger> s_Tracer;
        static std::shared_ptr<spdlog::logger> s_MemLogger;
        // [DateTime][Hours:Minutes:Seconds:Milliseconds]: Message
        static const char* s_LogPattern = "[%l][%D][%H:%M:%S.%e]: %^%v%$";
        // [DateTime][Hours:Minutes:Seconds:Milliseconds] FunctionName(FileName:CodeLine) Message
        static const char* s_TracePattern = "[%l][%D][%H:%M:%S.%e] %^%!(%s:%#) %v%$";

        using LogSinkColor = spdlog::sinks::stdout_color_sink_mt;
        using ErrorSinkColor = spdlog::sinks::stderr_color_sink_mt;
        using FileSink = spdlog::sinks::basic_file_sink_mt;
        using Sink = spdlog::sink_ptr;

        static std::shared_ptr<spdlog::logger> CreateLogger(const char* logName, const char* filepath, const char* pattern, int backtrace) {

            // setup console color sink
            std::shared_ptr<LogSinkColor> consoleSink = std::make_shared<LogSinkColor>();
            consoleSink->set_pattern(pattern);

            // setup file sink
            std::shared_ptr<FileSink> fileSink = std::make_shared<FileSink>(filepath, true);
            fileSink->set_pattern(pattern);

            core::vector<Sink> sinks = { consoleSink, fileSink };

            // setup logger
            std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>(logName, std::begin(sinks), std::end(sinks));
            spdlog::register_logger(logger);
            logger->enable_backtrace(backtrace);

            return logger;
        }

        void cLogManager::Init(const sLoggerDescriptor& descriptor) {
            const char* name = descriptor.Name;
            const int backtrace = descriptor.Backtrace;
            hstringstream ss;

            try {
                ss << name << "_Logger";
                hstring logName = ss.str();
                ss = {};
                ss << "logs/" << name << ".logs";
                hstring filepath = ss.str();
                ss = {};
                s_Logger = CreateLogger(logName.c_str(), filepath.c_str(), s_LogPattern, backtrace);

                ss << name << "_Tracer";
                logName = ss.str();
                ss = {};
                ss << "logs/" << name << ".logs";
                filepath = ss.str();
                ss = {};
                s_Tracer = CreateLogger(logName.c_str(), filepath.c_str(), s_TracePattern, backtrace);

                ss << name << "_Memory";
                logName = ss.str();
                ss = {};
                ss << "logs/" << name << ".logs";
                filepath = ss.str();
                ss = {};
                s_MemLogger = CreateLogger(logName.c_str(), filepath.c_str(), s_LogPattern, backtrace);
            }

            catch (const spdlog::spdlog_ex &ex) {
                SPDLOG_ERROR("Failed to initialize log {}. Error: {}", name, ex.what());
            }
        }

        void cLogManager::Free() {
            s_Tracer->flush();
            s_Logger->flush();
            s_MemLogger->flush();
        }

        void cLogManager::DumpBacktrace() {
            s_Tracer->dump_backtrace();
        }

        spdlog::logger* cLogManager::GetLogger() {
            return s_Logger.get();
        }

        spdlog::logger* cLogManager::GetTracer() {
            return s_Tracer.get();
        }

        spdlog::logger* cLogManager::GetMemLogger() {
            return s_MemLogger.get();
        }

    }

}