#pragma once

#include <core/rendering.hpp>

#ifdef DEBUG

#define log_debug_messages() xpe::core::DebugManager::Get().LogMessages()
#define log_debug_message() xpe::core::DebugManager::Get().LogLastMessage()

#else

#define log_debug_messages()
#define log_debug_message()

#endif

namespace xpe {

    namespace core {

        enum class eDebugErrorCode {
            D_NO_CODE,
            D_INVALID_ENUM,
            D_INVALID_OPERATION,
            D_INVALID_VALUE,
            D_STACK_OVERFLOW,
            D_STACK_UNDERFLOW,
            D_OUT_OF_MEMORY,
            D_INVALID_FRAMEBUFFER_OPERATION,
        };

        enum class eDebugType {
            D_NO_TYPE,
            D_ERROR,
            D_DEPRECATED_BEHAVIOUR,
            D_UNDEFINED_BEHAVIOUR,
            D_PORTABILITY,
            D_PERFORMANCE,
            D_MARKER,
            D_PUSH_GROUP,
            D_POP_GROUP,
            D_OTHER
        };

        enum class eDebugSource {
            D_NO_SOURCE,
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
            const char* Description = "";
            eDebugErrorCode ErrorCode = eDebugErrorCode::D_NO_CODE;
            eDebugType Type = eDebugType::D_NO_TYPE;
            eDebugSource Source = eDebugSource ::D_NO_SOURCE;
            eDebugSeverity Severity = eDebugSeverity::D_NO_SEVERITY;
        };

        class ENGINE_API DebuggerCallback {

        public:
            virtual void ReceiveMessage(const DebugMessage& message) = 0;

        };

        class RenderingContext_Interface;

        class ENGINE_API Debugger {

        public:
            virtual void Init(RenderingContext_Interface* const context) = 0;
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

            void SetDebugger(Debugger* const debugger);

            void LogMessages();
            void LogLastMessage();

            void ReceiveMessage(const DebugMessage &message) override;

        private:
            Debugger* m_Debugger;
        };

    }

}