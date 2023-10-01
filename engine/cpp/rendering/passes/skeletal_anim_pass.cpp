#include <rendering/passes/skeletal_anim_pass.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        SkeletalAnimPass::SkeletalAnimPass(
            const core::vector<RenderPassBinding>& bindings,
            RenderTarget* output,
            MaterialStorage* materialStorage,
            core::Boolean useMSAA
        ) : RenderPass(bindings, output)
        {
            m_InstanceBuffer.Reserve(1000);
            m_TransformBuffer.Reserve(1000);

            m_Pipeline->Rasterizer.MultisampleEnable = useMSAA;
            m_Pipeline->InputLayout.Format = SkeletalVertex::Format;

            materialStorage->BindPipeline(*m_Pipeline);

            context::CreatePipeline(*m_Pipeline);
        }

        SkeletalAnimPass::~SkeletalAnimPass()
        {
        }

        void SkeletalAnimPass::Update(Scene* scene)
        {
        }

        void SkeletalAnimPass::Draw(Scene* scene)
        {
            scene->EachComponent<SkinComponent>([this](SkinComponent* component)
            {
                DrawSkin(component->Skin, component->Skelet, component->Transform);
            });

            scene->EachComponent<SkinListComponent>([this](SkinListComponent* component)
            {
                DrawSkinList(component->Skin, component->Skelet, component->Transforms);
            });

            scene->EachComponent<SkinModelComponent>([this](SkinModelComponent* component)
            {
                DrawSkinModel(component->Model, component->Skelet, component->Transform);
            });

            scene->EachComponent<SkinModelListComponent>([this](SkinModelListComponent* component)
            {
                DrawSkinModelList(component->Model, component->Skelet, component->Transforms);
            });
        }

        void SkeletalAnimPass::DrawSkin(const Ref<Skin> &skin, const Ref<Skelet>& skelet, const Transform &transform)
        {
            if (skin.Get() == nullptr) return;

            RenderInstance instance;
            instance.TransformIndex = 0;
            instance.CameraIndex = 0;
            instance.MaterialIndex = skin->Material->Index;

            m_InstanceBuffer.Clear();
            m_InstanceBuffer.Add(instance);
            m_InstanceBuffer.Flush();

            m_TransformBuffer.Clear();
            m_TransformBuffer.AddTransform(transform);
            m_TransformBuffer.Flush();

            if (skelet.Get() != nullptr) {
                context::BindVSBuffer(skelet->BoneBuffer);
            }

            context::BindPrimitiveTopology(skin->PrimitiveTopology);
            context::BindVertexBuffer(skin->Vertices);
            context::BindIndexBuffer(skin->Indices);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);
            context::DrawIndexed(0, 0, skin->Indices.NumElements, 1);
            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);

            if (skelet.Get() != nullptr) {
                context::UnbindVSBuffer(skelet->BoneBuffer);
            }
        }

        void SkeletalAnimPass::DrawSkinList(const Ref<Skin> &skin, const Ref<Skelet>& skelet, const vector<Transform> &transforms)
        {
            if (skin.Get() == nullptr) return;

            usize instanceCount = transforms.size();
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < instanceCount ; i++)
            {
                RenderInstance instance;
                instance.TransformIndex = i;
                instance.CameraIndex = 0;
                instance.MaterialIndex = skin->Material->Index;

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(transforms[i]);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            if (skelet.Get() != nullptr) {
                context::BindVSBuffer(skelet->BoneBuffer);
            }

            context::BindPrimitiveTopology(skin->PrimitiveTopology);
            context::BindVertexBuffer(skin->Vertices);
            context::BindIndexBuffer(skin->Indices);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);
            context::DrawIndexed(0, 0, skin->Indices.NumElements, instanceCount);
            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);

            if (skelet.Get() != nullptr) {
                context::UnbindVSBuffer(skelet->BoneBuffer);
            }
        }

        void SkeletalAnimPass::DrawSkinModel(const Ref<SkinModel> &model, const Ref<Skelet>& skelet, const Transform &transform)
        {
            if (model.Get() == nullptr) return;

            u32 instanceCount = model->Skins.size();
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < instanceCount ; i++)
            {
                RenderInstance instance;
                instance.TransformIndex = i;
                instance.CameraIndex = 0;
                instance.MaterialIndex = model->Skins[i].Material->Index;

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(transform);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            if (skelet.Get() != nullptr) {
                context::BindVSBuffer(skelet->BoneBuffer);
            }

            context::BindPrimitiveTopology(model->PrimitiveTopology);
            context::BindVertexBuffer(model->Vertices);
            context::BindIndexBuffer(model->Indices);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);
            context::DrawIndexed(0, 0, model->Indices.NumElements, instanceCount);
            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);

            if (skelet.Get() != nullptr) {
                context::UnbindVSBuffer(skelet->BoneBuffer);
            }
        }

        void SkeletalAnimPass::DrawSkinModelList(const Ref<SkinModel> &model, const Ref<Skelet>& skelet, const vector<Transform> &transforms)
        {
            if (model.Get() == nullptr) return;

            u32 skinCount = model->Skins.size();
            u32 transformCount = transforms.size();
            u32 instanceCount = skinCount * transformCount;
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < transformCount ; i++)
            {
                RenderInstance instance;
                instance.TransformIndex = i;

                for (usize j = 0 ; j < skinCount ; j++)
                {
                    instance.CameraIndex = 0;
                    instance.MaterialIndex = model->Skins[j].Material->Index;
                    m_InstanceBuffer.Add(instance);
                }

                m_TransformBuffer.AddTransform(transforms[i]);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            if (skelet.Get() != nullptr) {
                context::BindVSBuffer(skelet->BoneBuffer);
            }

            context::BindPrimitiveTopology(model->PrimitiveTopology);
            context::BindVertexBuffer(model->Vertices);
            context::BindIndexBuffer(model->Indices);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);
            context::DrawIndexed(0, 0, model->Indices.NumElements, instanceCount);
            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);

            if (skelet.Get() != nullptr) {
                context::UnbindVSBuffer(skelet->BoneBuffer);
            }
        }
    }

}