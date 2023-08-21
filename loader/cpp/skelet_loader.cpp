#include <skelet_loader.h>

namespace xpe {

    namespace res {

        static void SetKeyFrames(Bone& bone, const aiNodeAnim* channel)
        {
            for (int i = 0; i < channel->mNumPositionKeys; i++)
            {
                aiVector3D aiPosition = channel->mPositionKeys[i].mValue;
                float timestamp = channel->mPositionKeys[i].mTime;
                bone.KeyPositions.emplace_back(timestamp, AssimpConversion::ToVec3(aiPosition));
            }

            for (int i = 0; i < channel->mNumRotationKeys; i++)
            {
                aiQuaternion aiOrienation = channel->mRotationKeys[i].mValue;
                float timestamp = channel->mRotationKeys[i].mTime;
                bone.KeyRotations.emplace_back(timestamp, AssimpConversion::ToQuat(aiOrienation));
            }

            for (int i = 0; i < channel->mNumScalingKeys; i++)
            {
                aiVector3D aiScale = channel->mScalingKeys[i].mValue;
                float timestamp = channel->mScalingKeys[i].mTime;
                bone.KeyScales.emplace_back(timestamp, AssimpConversion::ToVec3(aiScale));
            }
        }

        static const std::unordered_map<SkeletLoader::eOption, aiPostProcessSteps> s_SkeletOptions =
        {
                { SkeletLoader::eOption::TRIANGULATE, aiProcess_Triangulate },
                { SkeletLoader::eOption::FLIP_UV, aiProcess_FlipUVs },
                { SkeletLoader::eOption::CALC_TANGENTS, aiProcess_CalcTangentSpace },
                { SkeletLoader::eOption::OPTIMIZE_MESHES, aiProcess_OptimizeMeshes }
        };

        static u32 GetSkeletFlags(const vector<SkeletLoader::eOption>& options)
        {
            u32 flags = 0;
            for (const auto& option : options)
            {
                flags |= s_SkeletOptions.at(option);
            }
            return flags;
        }

        static void ParseSkeletFromAnim(const aiAnimation* animation, Skelet& skelet)
        {
            int size = animation->mNumChannels;
            auto& bones = skelet.Bones;
            usize boneCount = skelet.Bones.size();

            for (int i = 0; i < size; i++)
            {
                auto channel = animation->mChannels[i];
                string boneName = channel->mNodeName.data;

                auto it = bones.find(boneName);
                if (it == bones.end()) {
                    Bone bone;
                    bone.ID = boneCount++;
                    bone.Name = boneName;
                    bones.insert({ boneName, bone });
                }

                SetKeyFrames(bones[boneName], channel);
            }
        }

        static void ParseSkeletFromMesh(aiMesh* mesh, Skelet& skelet)
        {
            s32 boneCounter = 0;
            auto& bones = skelet.Bones;

            for (int i = 0; i < mesh->mNumBones; i++)
            {
                string boneName = mesh->mBones[i]->mName.C_Str();

                if (bones.find(boneName) == bones.end())
                {
                    Bone newBone;
                    newBone.ID = boneCounter;
                    newBone.Name = boneName;
                    newBone.Offset = AssimpConversion::ToMat4(mesh->mBones[i]->mOffsetMatrix);
                    bones.insert({ boneName, newBone });
                    boneCounter++;
                }
            }
        }

        static void ParseSkeletFromScene(
                aiNode* node,
                const aiScene* scene,
                Skelet& skelet
        ) {
            for (u32 i = 0 ; i < node->mNumMeshes ; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                ParseSkeletFromMesh(mesh, skelet);
            }

            for (u32 i = 0 ; i < node->mNumChildren ; i++)
            {
                ParseSkeletFromScene(node->mChildren[i], scene, skelet);
            }
        }

        Ref<Skelet> SkeletLoader::Load(const char* filepath, const vector<SkeletLoader::eOption>& options)
        {
            Skelet skelet;

            Assimp::Importer importer;
            u32 flags = GetSkeletFlags(options);
            const aiScene* scene = importer.ReadFile(filepath, flags);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                LogError("Failed to import 3D skelet model file {0}", filepath);
                return {};
            }

            ParseSkeletFromScene(scene->mRootNode, scene, skelet);
            ParseSkeletFromAnim(scene->mAnimations[0], skelet);

            return m_Storage->Add(filepath, skelet);
        }

    }

}