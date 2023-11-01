#include <rendering/core/vertex.h>

namespace xpe {

    namespace render {

        sVertexFormat::sVertexFormat(const std::vector<sVertexFormat::sAttribute>& attributes) : Attributes(attributes)
        {
            for (const auto& attribute : attributes)
            {
                Stride += attribute.ByteSize;
            }
        }

        const sVertexFormat sVertex::k_Format =
        {{
             sAttributes::k_Pos,
             sAttributes::k_UV,
             sAttributes::k_Normal,
             sAttributes::k_Tangent,
             sAttributes::k_BoneIds,
             sAttributes::k_BoneWeights
         }};

    }

}