#include <anim_loader.h>

namespace xpe {

    namespace res {

        static const std::unordered_map<AnimLoader::eOption, aiPostProcessSteps> s_AnimOptions =
        {
                { AnimLoader::eOption::TRIANGULATE, aiProcess_Triangulate },
                { AnimLoader::eOption::FLIP_UV, aiProcess_FlipUVs },
                { AnimLoader::eOption::CALC_TANGENTS, aiProcess_CalcTangentSpace },
                { AnimLoader::eOption::OPTIMIZE_MESHES, aiProcess_OptimizeMeshes }
        };

        static u32 GetAnimFlags(const vector<AnimLoader::eOption>& options)
        {
            u32 flags = 0;
            for (const auto& option : options)
            {
                flags |= s_AnimOptions.at(option);
            }
            return flags;
        }

        static void ParseAnimation(AnimationNode& parent, const aiNode* node)
        {
            parent.Name = node->mName.data;
            parent.Transform = AssimpConversion::ToMat4(node->mTransformation);
            parent.Children.reserve(node->mNumChildren);

            for (int i = 0; i < node->mNumChildren; i++)
            {
                AnimationNode child;
                ParseAnimation(child, node->mChildren[i]);
                parent.Children.emplace_back(child);
            }
        }

        Ref<Animation> AnimLoader::Load(const char* filepath, const vector<AnimLoader::eOption>& options)
        {
            Animation animation;

            Assimp::Importer importer;
            u32 flags = GetAnimFlags(options);
            const aiScene* scene = importer.ReadFile(filepath, flags);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                LogError("Failed to import 3D animation file {0}", filepath);
                return {};
            }

            auto aiAnim = scene->mAnimations[0];
            animation.Duration = aiAnim->mDuration;
            animation.TicksPerSecond = aiAnim->mTicksPerSecond;

            ParseAnimation(animation.Root, scene->mRootNode);

            return m_Storage->Add(filepath, animation);
        }

    }

}