#ifdef DX11

#include <rendering/core/debugger.h>
#include <d3d11.h>

namespace xpe {

    namespace render {

        namespace debugger {

            ID3D11Device* s_Device = nullptr;
            ID3D11Debug* s_Debug = nullptr;
            ID3D11InfoQueue* s_InfoQueue = nullptr;

            static DebugMessage ToDebugMessage(const D3D11_MESSAGE& d3D11Message)
            {
                DebugMessage message;

                message.ID = d3D11Message.ID;
                auto severity = d3D11Message.Severity;
                auto category = d3D11Message.Category;

                message.Description.reserve(d3D11Message.DescriptionByteLength);
                memmove((char*) message.Description.data(),
                        d3D11Message.pDescription,
                        d3D11Message.DescriptionByteLength);

                switch (severity) {

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

                switch (category) {

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

            static DebugMessage GetDebugMessage(int index)
            {
                SIZE_T messageSize = 0;
                s_InfoQueue->GetMessage(index, nullptr, &messageSize);

                D3D11_MESSAGE* message = (D3D11_MESSAGE*) salloc(messageSize);
                s_InfoQueue->GetMessage(index, message, &messageSize);

                DebugMessage debugMessage = ToDebugMessage(*message);

                return debugMessage;
            }

            void Init() {
                s_Device = reinterpret_cast<ID3D11Device*>(context::GetDevice());
                s_Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&s_Debug);
                s_Device->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&s_InfoQueue);
            }

            void Free() {
                s_Debug->Release();
                s_InfoQueue->Release();
            }

            bool GetLastMessage(DebugMessage& message) {
                auto messageCount = s_InfoQueue->GetNumStoredMessages();
                if (messageCount > 0) {
                    message = GetDebugMessage(messageCount - 1);
                    s_InfoQueue->ClearStoredMessages();
                    return true;
                }
                return false;
            }

            vector<DebugMessage> GetMessageQueue() {
                auto messageCount = s_InfoQueue->GetNumStoredMessages();
                vector<DebugMessage> messages;
                messages.resize(messageCount);

                for (int i = 0; i < messageCount; i++) {
                    messages.emplace_back(GetDebugMessage(i));
                }

                s_InfoQueue->ClearStoredMessages();

                return messages;
            }

            void ClearMessageQueue() {
                s_InfoQueue->ClearStoredMessages();
            }

        }

    }

}

#endif // DX11