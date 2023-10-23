#include <rendering/passes/skeletal_anim_pass.h>
#include <rendering/material/material_manager.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        SkeletalAnimPass::SkeletalAnimPass(
                const vector <RenderPassBinding> &bindings,
                RenderTarget *output
        ) : InstancingPass(bindings, output)
        {
            MaterialManager::Bind(*m_Pipeline);
        }

        void SkeletalAnimPass::Draw(Scene* scene)
        {
            scene->EachComponent<SkinModelComponent>([this](SkinModelComponent* component)
            {
                if (component->Model.Get() != nullptr) {
                    auto& model = *component->Model;
                    auto& skeleton = component->Skeleton;

                    if (skeleton.Get() != nullptr) {
                        skeleton->BoneBuffer.Flush();
                        context::BindVSBuffer(skeleton->BoneBuffer);
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

                    if (skeleton.Get() != nullptr) {
                        context::UnbindVSBuffer(skeleton->BoneBuffer);
                    }
                }
            });
        }

    }

}