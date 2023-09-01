#pragma once

#include <rendering/draw/drawer.h>
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

        class DirectLightBuffer;
        class PointLightBuffer;
        class SpotLightBuffer;

        class ENGINE_API SkeletalAnimDrawer : public Drawer
        {

        public:
            SkeletalAnimDrawer(
                CameraBuffer* cameraBuffer,
                Shader* shader,
                MaterialStorage* materialStorage,
                DirectLightBuffer* directLightBuffer,
                PointLightBuffer* pointLightBuffer,
                SpotLightBuffer* spotLightBuffer,
                SkeletStorage* skeletStorage,
                SkinStorage* skinStorage,
                RenderTarget* renderTarget,
                Viewport* viewport
            );

            ~SkeletalAnimDrawer() override;

            void Draw(Scene* scene) override;

        private:
            void DrawSkin(const Ref<Skin>& skin, const Ref<Skelet>& skelet, const Transform& transform);
            void DrawSkinList(const Ref<Skin>& skin, const Ref<Skelet>& skelet, const vector<Transform>& transforms);

            void DrawSkinModel(const Ref<SkinModel>& model, const Ref<Skelet>& skelet, const Transform& transform);
            void DrawSkinModelList(const Ref<SkinModel>& model, const Ref<Skelet>& skelet, const vector<Transform>& transforms);

        protected:
            MaterialStorage* m_MaterialStorage;
            SkinStorage* m_SkinStorage;
            SkeletStorage* m_SkeletStorage;

            InstanceBuffer m_InstanceBuffer;
            TransformBuffer m_TransformBuffer;
        };

    }

}