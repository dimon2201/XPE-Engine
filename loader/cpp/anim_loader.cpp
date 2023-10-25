#include <anim_loader.h>

namespace xpe {

    namespace res {

        static void ParseAnimation(AnimationNode& parent, const aiNode* node)
        {
            parent.Name = node->mName.data;
            parent.Transform = AssimpManager::ToMat4(node->mTransformation);
            parent.Children.reserve(node->mNumChildren);

            for (int i = 0; i < node->mNumChildren; i++)
            {
                AnimationNode child;
                ParseAnimation(child, node->mChildren[i]);
                parent.Children.emplace_back(child);
            }
        }

        Ref<Animation> AnimLoader::Load(const char* filepath, const vector<eLoadOption>& options)
        {
            if (m_Map.find(filepath) != m_Map.end()) {
                Ref<Animation> animRef;
                animRef.Create(*m_Map[filepath]);
                return animRef;
            }

            Ref<Animation> animRef;
            animRef.Create();

            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(filepath, AssimpManager::GetLoadFlags(options));

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                LogError("Failed to import 3D animation file {0}", filepath);
                return {};
            }

            auto aiAnim = scene->mAnimations[0];
            animRef->Duration = aiAnim->mDuration;
            animRef->TicksPerSecond = aiAnim->mTicksPerSecond;

            ParseAnimation(animRef->Root, scene->mRootNode);

            m_Map.insert({ filepath, animRef });
            return animRef;
        }

    }

}