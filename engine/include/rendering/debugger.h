#pragma once

#include <rendering/context.hpp>

#ifdef DEBUG

#define InitDebugger(debugger, context) xpe::core::DebugManager::Get().Init(debugger, context)
#define FreeDebugger() xpe::core::DebugManager::Get().Free()

#define LogDebugMessages() xpe::core::DebugManager::Get().LogMessages()
#define LogDebugMessage() xpe::core::DebugManager::Get().LogLastMessage()

#else

#define InitDebugger(debugger, context)
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
            const char* Description = "";
            eDebugType Type = eDebugType::D_NO_TYPE;
            eDebugCategory Category = eDebugCategory::D_NO_CATEGORY;
            eDebugSeverity Severity = eDebugSeverity::D_NO_SEVERITY;
            eDebugErrorType ErrorType = eDebugErrorType::D_NO_ERROR_TYPE;
        };

        class ENGINE_API DebuggerCallback {

        public:
            virtual void ReceiveMessage(const DebugMessage& message) = 0;

        };

        class ENGINE_API Debugger {

        public:
            virtual void Init(Context* const context) = 0;
            virtual void Free() = 0;

            virtual DebugMessage GetLastMessage() = 0;

            virtual std::vector<DebugMessage> GetMessageQueue() = 0;

            inline void SetCallback(DebuggerCallback* const callback) {
                m_Callback = callback;
            }

        protected:
            DebuggerCallback* m_Callback;
        };

        class ENGINE_API DebugManager : public DebuggerCallback {

        public:
            static DebugManager& Get() {
                static DebugManager instance;
                return instance;
            }

            void Init(Debugger* const debugger, Context* const context);
            void Free();

            void LogMessages();
            void LogLastMessage();

            void ReceiveMessage(const DebugMessage &message) override;

        private:
            Debugger* m_Debugger;
        };

    }

}