#include <rendering/context.hpp>
#include <rendering/materials/material.h>

namespace xpe {

    namespace render {

        void MaterialBuffer::Init(Context *context) {
            m_Context = context;
            m_Context->CreateBuffer(*this, eBufferType::CONSTANT, sizeof(MaterialBufferData), K_FALSE);
        }

        void MaterialBuffer::Free() {
            m_Context->FreeBuffer(*this);
        }

        void MaterialBuffer::Flush() {
            m_Context->WriteBuffer(*this, &m_Data, sizeof(m_Data));
        }

    }

}