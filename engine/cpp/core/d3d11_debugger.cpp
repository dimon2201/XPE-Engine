#include <core/d3d11_debugger.h>

namespace xpe {

    namespace core {

        void D3D11Debugger::Init(RenderingContext_Interface* const context) {
            m_Device = reinterpret_cast<ID3D11Device*>(context->GetDevice());
            m_Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&m_Debug);
            m_Debug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&m_InfoQueue);
        }

        void D3D11Debugger::Free() {
            m_Debug->Release();
            m_InfoQueue->Release();
        }

        DebugMessage D3D11Debugger::GetLastMessage() {
            auto messageCount = m_InfoQueue->GetNumStoredMessages();
            if (messageCount > 0) {
                return GetDebugMessage(messageCount - 1);
            }
            return {};
        }

        DebugMessage D3D11Debugger::GetDebugMessage(int index) {
            SIZE_T messageByteLength = 0;
            m_InfoQueue->GetMessageA(index, nullptr, &messageByteLength);

            D3D11_MESSAGE message = {};
            m_InfoQueue->GetMessageA(index, &message, &messageByteLength);

            return ToDebugMessage(message);
        }

        std::vector<DebugMessage> D3D11Debugger::GetMessageQueue() {
            std::vector<DebugMessage> messages;

            auto messageCount = m_InfoQueue->GetNumStoredMessages();
            for (int i = 0; i < messageCount; i++) {
                messages.emplace_back(GetDebugMessage(i));
            }

            return messages;
        }

        DebugMessage D3D11Debugger::ToDebugMessage(const D3D11_MESSAGE& d3D11Message) {
            DebugMessage message;

            message.Description = d3D11Message.pDescription;
            message.ID = d3D11Message.ID;

            switch (d3D11Message.Severity) {

                case D3D11_MESSAGE_SEVERITY::D3D11_MESSAGE_SEVERITY_CORRUPTION:
                    message.Severity = eDebugSeverity::D_HIGH;
                    message.Type = eDebugType::D_ERROR;
                    message.ErrorType = eDebugErrorType::D_OTHER_ERROR_TYPE;
                    break;

                case D3D11_MESSAGE_SEVERITY::D3D11_MESSAGE_SEVERITY_MESSAGE:
                    message.Severity = eDebugSeverity::D_NOTIFICATION;
                    break;

                case D3D11_MESSAGE_SEVERITY::D3D11_MESSAGE_SEVERITY_INFO:
                    message.Severity = eDebugSeverity::D_LOW;
                    break;

                case D3D11_MESSAGE_SEVERITY::D3D11_MESSAGE_SEVERITY_WARNING:
                    message.Severity = eDebugSeverity::D_MEDIUM;
                    message.Type = eDebugType::D_WARNING;
                    break;

                case D3D11_MESSAGE_SEVERITY::D3D11_MESSAGE_SEVERITY_ERROR:
                    message.Severity = eDebugSeverity::D_HIGH;
                    message.Type = eDebugType::D_ERROR;
                    message.ErrorType = eDebugErrorType::D_OTHER_ERROR_TYPE;
                    break;

            }

            switch (d3D11Message.Category) {

                case D3D11_MESSAGE_CATEGORY::D3D11_MESSAGE_CATEGORY_APPLICATION_DEFINED:
                    message.Category = eDebugCategory::D_APPLICATION;
                    break;

                case D3D11_MESSAGE_CATEGORY::D3D11_MESSAGE_CATEGORY_CLEANUP:
                    message.Category = eDebugCategory::D_API;
                    break;

                case D3D11_MESSAGE_CATEGORY::D3D11_MESSAGE_CATEGORY_COMPILATION:
                    message.Category = eDebugCategory::D_SHADER_COMPILER;
                    break;

                case D3D11_MESSAGE_CATEGORY::D3D11_MESSAGE_CATEGORY_EXECUTION:
                    message.Category = eDebugCategory::D_API;
                    break;

                case D3D11_MESSAGE_CATEGORY::D3D11_MESSAGE_CATEGORY_INITIALIZATION:
                    message.Category = eDebugCategory::D_API;
                    break;

                case D3D11_MESSAGE_CATEGORY::D3D11_MESSAGE_CATEGORY_MISCELLANEOUS:
                    message.Category = eDebugCategory::D_API;
                    break;

                case D3D11_MESSAGE_CATEGORY::D3D11_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
                    message.Category = eDebugCategory::D_API;
                    break;

                case D3D11_MESSAGE_CATEGORY::D3D11_MESSAGE_CATEGORY_SHADER:
                    message.Category = eDebugCategory::D_SHADER_COMPILER;
                    break;

                case D3D11_MESSAGE_CATEGORY::D3D11_MESSAGE_CATEGORY_STATE_CREATION:
                    message.Category = eDebugCategory::D_API;
                    break;

                case D3D11_MESSAGE_CATEGORY::D3D11_MESSAGE_CATEGORY_STATE_GETTING:
                    message.Category = eDebugCategory::D_API;
                    break;

                case D3D11_MESSAGE_CATEGORY::D3D11_MESSAGE_CATEGORY_STATE_SETTING:
                    message.Category = eDebugCategory::D_API;
                    break;
            }

            return message;
        }

    }

}