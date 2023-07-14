#include <json/json.h>

namespace xpe {

    namespace core {

        AppConfig AppConfigReader::Read(const char *filepath) {
            AppConfig appConfig;

            std::ifstream file(filepath, std::ios::in | std::ios::binary);

            if (!file.is_open()) {
                FMT_ASSERT(false, "Please specify correct path to config.json file!");
                return appConfig;
            }

            file.seekg(0, std::ios::end);
            usize fileByteSize = file.tellg();
            char* str = (char*)malloc(fileByteSize);
            memset(str, 0, fileByteSize);
            file.seekg(0, std::ios::beg);
            file.read(str, fileByteSize);
            file.close();

            Parse(std::string(str), appConfig);

            return appConfig;
        }

        void AppConfigReader::Parse(const std::string &source, AppConfig &config) {
            Json::Value root;
            Json::Reader reader;
            reader.parse(source, root);

            config.WinTitle = root["win_title"].asString();
            config.WinWidth = root["win_width"].asInt();
            config.WinHeight = root["win_height"].asInt();
            config.WinX = root["win_x"].asInt();
            config.WinY = root["win_y"].asInt();
            config.Vsync = root["vsync"].asBool() ? K_TRUE : K_FALSE;

            config.LogTitle = root["log_title"].asString();
            config.LogBacktrace = root["log_backtrace"].asInt();

            std::string gpu = root["gpu"].asString();
            if (gpu == "DX11") {
                config.GPU = AppConfig::eGPU::DX11;
            }
            else if (gpu == "OPENGL") {
                config.GPU = AppConfig::eGPU::OPENGL;
            }
            else if (gpu == "VULKAN") {
                config.GPU = AppConfig::eGPU::VULKAN;
            }
            else {
                FMT_ASSERT(false, "Config GPU type is not supported!");
                return;
            }

            config.LogDebugErrors = root["debug_errors"].asBool();
            config.LogDebugWarnings = root["debug_warnings"].asBool();
            config.LogDebugInfos = root["debug_infos"].asBool();

            config.FPS = root["fps"].asFloat();

            config.LogTimeDelaySeconds = root["log_time_delay_seconds"].asFloat();

            config.LockOnFPS = root["lock_on_fps"].asBool();

        }

    }

}