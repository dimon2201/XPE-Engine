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

        DebugMessage D3D11Debugger::ToDebugMessage(const D3D11_MESSAGE &message) {
            return {};
        }

    }

}