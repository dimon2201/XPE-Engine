#include <rendering/core/debugger.h>

namespace xpe {

    namespace render {

        void DebugManager::Init(Debugger *const debugger, Context *const context) {
            m_Debugger = debugger;
            m_Debugger->Init(context);
            m_Debugger->SetCallback(this);
        }

        void DebugManager::Free() {
            m_Debugger->Free();
        }

        void DebugManager::LogMessages() {
            auto messages = m_Debugger->GetMessageQueue();
            for (const auto& message : messages) {
                ReceiveMessage(message);
            }
        }

        void DebugManager::LogLastMessage() {
            DebugMessage message;
            if (m_Debugger->GetLastMessage(message)) {
                ReceiveMessage(message);
            }
        }

        void DebugManager::ReceiveMessage(const DebugMessage& message) {
            hstringstream ss;

            int id = message.ID;
            eDebugType type = message.Type;
            eDebugCategory category = message.Category;
            eDebugErrorType errorType = message.ErrorType;
            eDebugSeverity severity = message.Severity;
            const char* description = message.Description.c_str();

            ss << "\n";
            ss << "--------------- Rendering Debugger ---------------- \n";

            ss << "ID: " << id << "\n";

            switch (type)
            {
                case eDebugType::D_NO_TYPE:               ss << "Type: None"; break;
                case eDebugType::D_ERROR:                 ss << "Type: Error"; break;
                case eDebugType::D_WARNING:               ss << "Type: Warning"; break;
                case eDebugType::D_DEPRECATED_BEHAVIOUR:  ss << "Type: Deprecated Behaviour"; break;
                case eDebugType::D_UNDEFINED_BEHAVIOUR:   ss << "Type: Undefined Behaviour"; break;
                case eDebugType::D_PORTABILITY:           ss << "Type: Portability"; break;
                case eDebugType::D_PERFORMANCE:           ss << "Type: Performance"; break;
                case eDebugType::D_MARKER:                ss << "Type: Marker"; break;
                case eDebugType::D_PUSH_GROUP:            ss << "Type: Push Group"; break;
                case eDebugType::D_POP_GROUP:             ss << "Type: Pop Group"; break;
                case eDebugType::D_OTHER:                 ss << "Type: Other"; break;
            }
            ss << "\n";

            switch (category)
            {
                case eDebugCategory::D_NO_CATEGORY:             ss << "Category: None"; break;
                case eDebugCategory::D_API:                     ss << "Category: API"; break;
                case eDebugCategory::D_WINDOW_SYSTEM:           ss << "Category: Window System"; break;
                case eDebugCategory::D_SHADER_COMPILER:         ss << "Category: Shader Compiler"; break;
                case eDebugCategory::D_THIRD_PARTY:             ss << "Category: Third Party"; break;
                case eDebugCategory::D_APPLICATION:             ss << "Category: Application"; break;
                case eDebugCategory::D_OTHER:                   ss << "Category: Other"; break;
            }
            ss << "\n";

            switch (errorType)
            {
                case eDebugErrorType::D_NO_ERROR_TYPE:                       ss << "ErrorType: None"; break;
                case eDebugErrorType::D_INVALID_ENUM:                        ss << "ErrorType: Invalid Enum"; break;
                case eDebugErrorType::D_INVALID_VALUE:                       ss << "ErrorType: Invalid Value"; break;
                case eDebugErrorType::D_INVALID_OPERATION:                   ss << "ErrorType: Invalid Operation"; break;
                case eDebugErrorType::D_INVALID_FRAMEBUFFER_OPERATION:       ss << "ErrorType: Invalid FrameBuffer Operation"; break;
                case eDebugErrorType::D_STACK_OVERFLOW:                      ss << "ErrorType: Stack Overflow"; break;
                case eDebugErrorType::D_STACK_UNDERFLOW:                     ss << "ErrorType: Stack Underflow"; break;
                case eDebugErrorType::D_OUT_OF_MEMORY:                       ss << "ErrorType: Out Of Memory"; break;
                case eDebugErrorType::D_OTHER_ERROR_TYPE:                    ss << "ErrorType: Other"; break;
            }
            ss << "\n";

            switch (severity)
            {
                case eDebugSeverity::D_NO_SEVERITY:       ss << "Severity: None"; break;
                case eDebugSeverity::D_NOTIFICATION:      ss << "Severity: Notification"; break;
                case eDebugSeverity::D_LOW:               ss << "Severity: Low"; break;
                case eDebugSeverity::D_MEDIUM:            ss << "Severity: Medium"; break;
                case eDebugSeverity::D_HIGH:              ss << "Severity: High"; break;
            }
            ss << "\n";

            ss << "Description: " << description << "\n";
            ss << "--------------------------------------------------- \n";

            auto& appConfig = AppConfig::Get();

            if (appConfig.LogDebugErrors)
            {

                switch (type) {

                    case eDebugType::D_ERROR:
                        LogError(ss.str().c_str());
                        debug_break();
                        break;

                    case eDebugType::D_UNDEFINED_BEHAVIOUR:
                        LogError(ss.str().c_str());
                        debug_break();
                        break;

                }

            }

            if (appConfig.LogDebugWarnings)
            {

                switch (type) {

                    case eDebugType::D_WARNING:
                        LogWarning(ss.str().c_str());
                        break;

                    case eDebugType::D_DEPRECATED_BEHAVIOUR:
                        LogWarning(ss.str().c_str());
                        break;

                    case eDebugType::D_PERFORMANCE:
                        LogWarning(ss.str().c_str());
                        break;

                }

            }

            if (appConfig.LogDebugInfos)
            {
                LogInfo(ss.str().c_str());
            }

        }

    }

}