#include <rendering/buffers/instance_buffer.h>

namespace xpe {

    namespace render {

        void InstanceBuffer::Bind() {
            context::BindVSBuffer(this);
        }

        void InstanceBuffer2d::Bind() {
            context::BindVSBuffer(this);
        }

        void TextGlyphInstanceBuffer::Bind() {
            context::BindVSBuffer(this);
        }

    }

}