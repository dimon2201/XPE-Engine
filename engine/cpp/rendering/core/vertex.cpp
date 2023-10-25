#include <rendering/core/vertex.h>

namespace xpe {

    namespace render {

        VertexFormat::VertexFormat(const std::vector<VertexFormat::Attribute>& attributes) : Attributes(attributes)
        {
            for (const auto& attribute : attributes)
            {
                Stride += attribute.ByteSize;
            }
        }

        VertexFormat Vertex::Format =
        {{
             Attributes::POS,
             Attributes::UV,
             Attributes::NORMAL,
             Attributes::TANGENT,
             Attributes::BONE_IDS,
             Attributes::BONE_WEIGHTS
         }};

    }

}