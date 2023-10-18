#include <rendering/passes/skeletal_anim_pass.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        SkeletalAnimPass::SkeletalAnimPass(
                const vector <RenderPassBinding> &bindings,
                RenderTarget *output,
                MaterialStorage *materialStorage
        ) : InstancingPass(bindings, output)
        {
            materialStorage->BindPipeline(*m_Pipeline);
        }

        void SkeletalAnimPass::Draw(Scene* scene)
        {
            scene->EachComponent<SkinComponent>([this](SkinComponent* component)
            {
                if (component->Skin.Get() != nullptr) {
                    auto& skin = *component->Skin;
                    auto& skelet = component->Skelet;

                    if (skelet.Get() != nullptr) {
                        skelet->BoneBuffer.Flush();
                        context::BindVSBuffer(skelet->BoneBuffer);
                    }

                    DrawInstanced(
                            skin.PrimitiveTopology,
                            skin.VertexOffset,
                            skin.Vertices.size(),
                            skin.IndexOffset,
                            skin.Indices.size(),
                            component->Entity,
                            component->Entities
                    );

                    if (skelet.Get() != nullptr) {
                        context::UnbindVSBuffer(skelet->BoneBuffer);
                    }
                }
            });

            scene->EachComponent<SkinModelComponent>([this](SkinModelComponent* component)
            {
                if (component->Model.Get() != nullptr) {
                    auto& model = *component->Model;
                    auto& skelet = component->Skelet;

                    if (skelet.Get() != nullptr) {
                        skelet->BoneBuffer.Flush();
                        context::BindVSBuffer(skelet->BoneBuffer);
                    }

                    DrawInstanced(
                            model.PrimitiveTopology,
                            model.VertexOffset,
                            model.VertexCount,
                            model.IndexOffset,
                            model.Indices.size(),
                            component->Entity,
                            component->Entities
                    );

                    if (skelet.Get() != nullptr) {
                        context::UnbindVSBuffer(skelet->BoneBuffer);
                    }
                }
            });
        }

    }

}