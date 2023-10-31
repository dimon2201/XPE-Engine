#include <skeleton_loader.h>

#include <anim/skeleton_manager.h>

namespace xpe {

    namespace res {

        static void SetKeyFrames(sBone& bone, const aiNodeAnim* channel)
        {
            for (int i = 0; i < channel->mNumPositionKeys; i++)
            {
                aiVector3D aiPosition = channel->mPositionKeys[i].mValue;
                float timestamp = channel->mPositionKeys[i].mTime;
                bone.KeyPositions.emplace_back(timestamp, AssimpManager::ToVec3(aiPosition));
            }

            for (int i = 0; i < channel->mNumRotationKeys; i++)
            {
                aiQuaternion aiOrienation = channel->mRotationKeys[i].mValue;
                float timestamp = channel->mRotationKeys[i].mTime;
                bone.KeyRotations.emplace_back(timestamp, AssimpManager::ToQuat(aiOrienation));
            }

            for (int i = 0; i < channel->mNumScalingKeys; i++)
            {
                aiVector3D aiScale = channel->mScalingKeys[i].mValue;
                float timestamp = channel->mScalingKeys[i].mTime;
                bone.KeyScales.emplace_back(timestamp, AssimpManager::ToVec3(aiScale));
            }
        }

        static void ParseSkeletonFromAnim(const aiAnimation* animation, sSkeleton& skelet)
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
                    sBone bone;
                    bone.ID = boneCount++;
                    bone.Name = boneName;
                    bones.insert({ boneName, bone });
                }

                SetKeyFrames(bones[boneName], channel);
            }
        }

        static void ParseSkeletFromMesh(aiMesh* mesh, sSkeleton& skelet)
        {
            s32 boneCounter = 0;
            auto& bones = skelet.Bones;

            for (int i = 0; i < mesh->mNumBones; i++)
            {
                string boneName = mesh->mBones[i]->mName.C_Str();

                if (bones.find(boneName) == bones.end())
                {
                    sBone newBone;
                    newBone.ID = boneCounter;
                    newBone.Name = boneName;
                    newBone.Offset = AssimpManager::ToMat4(mesh->mBones[i]->mOffsetMatrix);
                    bones.insert({ boneName, newBone });
                    boneCounter++;
                }
            }
        }

        static void ParseSkeletonFromScene(
                aiNode* node,
                const aiScene* scene,
                sSkeleton& skelet
        ) {
            for (u32 i = 0 ; i < node->mNumMeshes ; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                ParseSkeletFromMesh(mesh, skelet);
            }

            for (u32 i = 0 ; i < node->mNumChildren ; i++)
            {
                ParseSkeletonFromScene(node->mChildren[i], scene, skelet);
            }
        }

        sSkeleton SkeletonLoader::Load(const char* filepath, const vector<eLoadOption>& options)
        {
            sSkeleton skeleton;

            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(filepath, AssimpManager::GetLoadFlags(options));

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                LogError("Failed to import 3D skelet model file {0}", filepath);
                return {};
            }

            ParseSkeletonFromScene(scene->mRootNode, scene, skeleton);
            ParseSkeletonFromAnim(scene->mAnimations[0], skeleton);

            return mSkeletonManager::AddSkeleton(skeleton);
        }

    }

}