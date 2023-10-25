#pragma once

#include <rendering/core/context.hpp>

#ifdef DEBUG

#define InitDebugger() xpe::render::Debugger::Init()
#define FreeDebugger() xpe::render::Debugger::Free()

#define LogDebugMessages() xpe::render::DebugManager::LogMessages()
#define LogDebugMessage() xpe::render::Debugger::LogLastMessage()

#else

#define InitDebugger()
#define FreeDebugger()

#define LogDebugMessages()
#define LogDebugMessage()

#endif

namespace xpe {

    namespace render {

        using namespace core;

        enum class eDebugErrorType {
            D_NO_ERROR_TYPE,
            D_INVALID_ENUM,
            D_INVALID_OPERATION,
            D_INVALID_VALUE,
            D_STACK_OVERFLOW,
            D_STACK_UNDERFLOW,
            D_OUT_OF_MEMORY,
            D_INVALID_FRAMEBUFFER_OPERATION,
            D_OTHER_ERROR_TYPE
        };

        enum class eDebugType {
            D_NO_TYPE,
            D_ERROR,
            D_WARNING,
            D_DEPRECATED_BEHAVIOUR,
            D_UNDEFINED_BEHAVIOUR,
            D_PORTABILITY,
            D_PERFORMANCE,
            D_MARKER,
            D_PUSH_GROUP,
            D_POP_GROUP,
            D_OTHER
        };

        enum class eDebugCategory {
            D_NO_CATEGORY,
            D_API,
            D_WINDOW_SYSTEM,
            D_SHADER_COMPILER,
            D_THIRD_PARTY,
            D_APPLICATION,
            D_OTHER
        };

        enum class eDebugSeverity {
            D_NO_SEVERITY,
            D_LOW,
            D_MEDIUM,
            D_HIGH,
            D_NOTIFICATION
        };

        struct ENGINE_API DebugMessage final {
            int ID = 0;
            hstring Description = "";
            eDebugType Type = eDebugType::D_NO_TYPE;
            eDebugCategory Category = eDebugCategory::D_NO_CATEGORY;
            eDebugSeverity Severity = eDebugSeverity::D_NO_SEVERITY;
            eDebugErrorType ErrorType = eDebugErrorType::D_NO_ERROR_TYPE;
        };

        typedef void (*DebuggerCallback)(const DebugMessage&);

        namespace context {

            extern DebuggerCallback Callback;
            extern bool EnableInfoLog;
            extern bool EnableWarnLog;
            extern bool EnableErrorLog;

            ENGINE_API void InitDebug();
            ENGINE_API void FreeDebug();

            ENGINE_API bool GetLastMessage(DebugMessage& message);

            ENGINE_API vector<DebugMessage> GetMessageQueue();

            ENGINE_API void ClearMessageQueue();

        }

        class ENGINE_API Debugger final {

        public:
            static void Init();
            static void Free();

            static void LogMessages();
            static void LogLastMessage();

        };

    }

}