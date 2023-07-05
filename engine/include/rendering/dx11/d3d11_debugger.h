#pragma once

#include <d3d11.h>
#include <rendering/debugger.h>

namespace xpe {

    namespace render {

        class ENGINE_API D3D11Debugger : public Debugger {

        public:
            void Init(Context* const context) override;
            void Free() override;
            DebugMessage GetLastMessage() override;
            std::vector<DebugMessage> GetMessageQueue() override;

        private:
            DebugMessage GetDebugMessage(int index);
            DebugMessage ToDebugMessage(const D3D11_MESSAGE& message);

        private:
            ID3D11Device* m_Device = nullptr;
            ID3D11Debug* m_Debug = nullptr;
            ID3D11InfoQueue* m_InfoQueue = nullptr;
        };

    }

}