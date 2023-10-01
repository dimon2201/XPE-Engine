#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/instance_buffer.h>
#include <rendering/buffers/transform_buffer.h>
#include <rendering/storages/material_storage.h>

#include <anim/storages/skelet_storage.h>
#include <anim/storages/skin_storage.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        using namespace core;
        using namespace math;
        using namespace ecs;
        using namespace anim;

        class SkeletalAnimPass : public RenderPass
        {

        public:
            SkeletalAnimPass(
                const core::vector<RenderPassBinding>& bindings,
                RenderTarget* output,
                MaterialStorage* materialStorage,
                core::Boolean useMSAA
            );
            ~SkeletalAnimPass();

            virtual void Update(Scene* scene) override final;
            virtual void Draw(Scene* scene) override final;

        private:
            void DrawSkin(const Ref<Skin>& skin, const Ref<Skelet>& skelet, const Transform& transform);
            void DrawSkinList(const Ref<Skin>& skin, const Ref<Skelet>& skelet, const vector<Transform>& transforms);

            void DrawSkinModel(const Ref<SkinModel>& model, const Ref<Skelet>& skelet, const Transform& transform);
            void DrawSkinModelList(const Ref<SkinModel>& model, const Ref<Skelet>& skelet, const vector<Transform>& transforms);

        protected:
            InstanceBuffer m_InstanceBuffer;
            TransformBuffer m_TransformBuffer;
        };

    }

}