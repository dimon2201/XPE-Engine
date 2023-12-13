#include <rendering/core/core.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        sVertexFormat::sVertexFormat(const std::vector<sVertexFormat::sAttribute>& attributes)
        : Attributes(attributes)
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

        sBlendMode::sBlendMode(const vector<sBlendTarget>& targets)
        {
            Targets = targets;
            Init();
        }

        sBlendMode::~sBlendMode()
        {
            context::FreeBlendMode(*this);
        }

        void sBlendMode::Bind()
        {
            context::BindBlendMode(State);
        }

        void sBlendMode::Init()
        {
            context::CreateBlendMode(*this);
        }

    }

}