#ifdef DX11

#include <rendering/core/debugger.hpp>

#include <d3d11.h>

namespace xpe {

    namespace render {

        namespace context {

            ID3D11Debug* s_Debug = nullptr;
            ID3D11InfoQueue* s_InfoQueue = nullptr;

            static sDebugMessage ToDebugMessage(const D3D11_MESSAGE& d3D11Message)
            {
                sDebugMessage message;

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

            static sDebugMessage GetDebugMessage(int index)
            {
                SIZE_T messageSize = 0;
                s_InfoQueue->GetMessageA(index, nullptr, &messageSize);

                D3D11_MESSAGE* message = (D3D11_MESSAGE*) salloc(messageSize);
                s_InfoQueue->GetMessageA(index, message, &messageSize);

                sDebugMessage debugMessage = ToDebugMessage(*message);

                return debugMessage;
            }

            void InitDebug() {
                auto* device = reinterpret_cast<ID3D11Device*>(context::GetDevice());
                device->QueryInterface(__uuidof(ID3D11Debug), (void**)&s_Debug);
                device->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&s_InfoQueue);
            }

            void FreeDebug() {
                s_Debug->Release();
                s_InfoQueue->Release();
            }

            bool GetLastMessage(sDebugMessage& message) {
                auto messageCount = s_InfoQueue->GetNumStoredMessages();
                if (messageCount > 0) {
                    message = GetDebugMessage(0);
                    s_InfoQueue->ClearStoredMessages();
                    return true;
                }
                return false;
            }

            vector<sDebugMessage> GetMessageQueue() {
                auto messageCount = s_InfoQueue->GetNumStoredMessages();
                vector<sDebugMessage> messages;
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