#pragma once

#include <rendering/passes/instancing_pass.h>

#include <anim/storages/skelet_storage.h>

namespace xpe {

    namespace render {

        using namespace anim;

        class SkeletalAnimPass : public InstancingPass
        {

        public:
            SkeletalAnimPass(
                const vector<RenderPassBinding>& bindings,
                RenderTarget* output,
                MaterialStorage* materialStorage
            );

            virtual void Draw(Scene* scene) override final;

        };

    }

}