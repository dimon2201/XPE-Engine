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
             sAttributes::POS,
             sAttributes::UV,
             sAttributes::NORMAL,
             sAttributes::TANGENT,
             sAttributes::BONE_IDS,
             sAttributes::BONE_WEIGHTS
         }};

    }

}