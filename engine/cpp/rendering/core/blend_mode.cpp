#include <rendering/core/blend_mode.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

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