#include <rendering/core/blend_mode.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        BlendMode::BlendMode(const vector<BlendTarget>& targets)
        {
            Targets = targets;
            Init();
        }

        BlendMode::~BlendMode()
        {
            context::FreeBlendMode(*this);
        }

        void BlendMode::Bind()
        {
            context::BindBlendMode(State);
        }

        void BlendMode::Init()
        {
            context::CreateBlendMode(*this);
        }

    }

}