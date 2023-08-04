#include <rendering/buffers/instance_buffer.h>

namespace xpe {

    namespace render {

        void InstanceBuffer::Bind() {
            m_Context->BindVSBuffer(this);
        }

        void InstanceBuffer2d::Bind() {
            m_Context->BindVSBuffer(this);
        }

        void TextGlyphInstanceBuffer::Bind() {
            m_Context->BindVSBuffer(this);
        }

    }

}