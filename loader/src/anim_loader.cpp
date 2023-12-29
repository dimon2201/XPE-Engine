#include <anim_loader.hpp>

namespace xpe {

    namespace res {

        static void ParseAnimation(sAnimationNode& parent, const aiNode* node)
        {
            parent.Name = node->mName.data;
            parent.Transform = cAssimpManager::ToMat4(node->mTransformation);
            parent.Children.reserve(node->mNumChildren);

            for (int i = 0; i < node->mNumChildren; i++)
            {
                sAnimationNode child;
                ParseAnimation(child, node->mChildren[i]);
                parent.Children.emplace_back(child);
            }
        }

        sAnimation cAnimLoader::Load(const char* filepath, const vector<eLoadOption>& options)
        {
            sAnimation animation;

            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(filepath, cAssimpManager::GetLoadFlags(options));

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                LogError("Failed to import 3D animation file {0}", filepath);
                return {};
            }

            auto aiAnim = scene->mAnimations[0];
            animation.Duration = aiAnim->mDuration;
            animation.TicksPerSecond = aiAnim->mTicksPerSecond;

            ParseAnimation(animation.Root, scene->mRootNode);

            return animation;
        }

    }

}