#include <rendering/vertex.h>

namespace xpe {

    namespace render {

        VertexFormat Vertex2D::Format =
        {{
            Attributes::POS2,
            Attributes::UV
        }};

        VertexFormat Vertex3D::Format =
        {{
            Attributes::POS3,
            Attributes::UV,
            Attributes::NORMAL
        }};

        VertexFormat::VertexFormat(const std::vector<VertexFormat::Attribute>& attributes) : Attributes(attributes) {
            for (const auto& attribute : attributes) {
                Stride += (u32) attribute.Format;
            }
        }

    }

}