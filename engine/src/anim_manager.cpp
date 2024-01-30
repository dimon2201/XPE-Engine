#include <anim_manager.hpp>

namespace xpe {

    namespace anim {

        float MAnim::s_CurrentSeconds = 0;
        float MAnim::s_DeltaSeconds = 0;

        void MAnim::Init()
        {
            Profile();
            Buffers::Skeleton = new sSkeletonBuffer();
        }

        void MAnim::Free()
        {
            Profile();
            delete Buffers::Skeleton;
        }

        void MAnim::Update(cScene* scene, float dt)
        {
            Profile();

            auto components = scene->GetComponents<CAnimation, CSkeleton, CSkeletonInfo>();
            for (auto [entity, animation, skeleton, skeletonInfo] : components.each())
            {
                AnimateSkeleton(animation, skeleton, skeletonInfo.SkeletonIndex, dt);
            }
        }

        void MAnim::AnimateSkeleton(const CAnimation &animation, CSkeleton &skeleton, u32 skeletonIndex, float dt)
        {
            s_DeltaSeconds = dt * 0.001;
            s_CurrentSeconds += animation.TicksPerSecond * s_DeltaSeconds;
            s_CurrentSeconds = fmod(s_CurrentSeconds, animation.Duration);
            UpdateSkeletonTransform(animation.Root, skeleton, *Buffers::Skeleton, skeletonIndex, glm::mat4(1.0f));
        }

        void MAnim::AnimateBone(sBone &bone, float time)
        {
            glm::mat4 translation = InterpolatePosition(bone, time);
            glm::mat4 rotation = InterpolateRotation(bone, time);
            glm::mat4 scale = InterpolateScale(bone, time);
            bone.Transform = translation * rotation * scale;
        }

        int MAnim::GetPositionIndex(sBone &bone, float time)
        {
            usize count = bone.KeyPositions.size();

            for (int i = 0; i < count; ++i)
            {
                if (time < bone.KeyPositions[i + 1].Timestamp)
                {
                    return i;
                }
            }

            return -1;
        }

        int MAnim::GetRotationIndex(sBone &bone, float time)
        {
            usize count = bone.KeyRotations.size();

            for (int i = 0; i < count; ++i)
            {
                if (time < bone.KeyRotations[i + 1].Timestamp)
                {
                    return i;
                }
            }

            return -1;
        }

        int MAnim::GetScaleIndex(sBone &bone, float time)
        {
            usize count = bone.KeyScales.size();

            for (int i = 0; i < count; ++i)
            {
                if (time < bone.KeyScales[i + 1].Timestamp)
                {
                    return i;
                }
            }

            return -1;
        }

        float MAnim::GetScaleFactor(float lastTimestamp, float nextTimestamp, float time)
        {
            float scaleFactor;
            float midWayLength = time - lastTimestamp;
            float framesDiff = nextTimestamp - lastTimestamp;
            scaleFactor = midWayLength / framesDiff;
            return scaleFactor;
        }

        glm::mat4 MAnim::InterpolatePosition(sBone &bone, float time)
        {
            auto& positions = bone.KeyPositions;

            if (positions.empty())
            {
                return glm::mat4(1.0f);
            }

            if (positions.size() == 1)
            {
                return glm::translate(glm::mat4(1.0f), positions[0].Position);
            }

            int p0Index = GetPositionIndex(bone, time);
            int p1Index = p0Index + 1;

            float scaleFactor = GetScaleFactor(
                    positions[p0Index].Timestamp,
                    positions[p1Index].Timestamp,
                    time
            );

            glm::vec3 finalPosition = glm::mix(
                    positions[p0Index].Position,
                    positions[p1Index].Position,
                    scaleFactor
            );

            return glm::translate(glm::mat4(1.0f), finalPosition);
        }

        glm::mat4 MAnim::InterpolateRotation(sBone &bone, float time)
        {
            auto& rotations = bone.KeyRotations;

            if (rotations.empty())
            {
                return glm::mat4(1.0f);
            }

            if (rotations.size() == 1)
            {
                auto rotation = glm::normalize(rotations[0].Orientation);
                return glm::toMat4(rotation);
            }

            int p0Index = GetRotationIndex(bone, time);
            int p1Index = p0Index + 1;

            float scaleFactor = GetScaleFactor(
                    rotations[p0Index].Timestamp,
                    rotations[p1Index].Timestamp,
                    time
            );

            glm::quat finalRotation = glm::slerp(
                    rotations[p0Index].Orientation,
                    rotations[p1Index].Orientation,
                    scaleFactor
            );

            finalRotation = glm::normalize(finalRotation);

            return glm::toMat4(finalRotation);
        }

        glm::mat4 MAnim::InterpolateScale(sBone &bone, float time)
        {
            auto& scales = bone.KeyScales;

            if (scales.empty())
            {
                return glm::mat4(1.0f);
            }

            if (scales.size() == 1)
            {
                return glm::scale(glm::mat4(1.0f), scales[0].Scale);
            }

            int p0Index = GetScaleIndex(bone, time);
            int p1Index = p0Index + 1;

            float scaleFactor = GetScaleFactor(scales[p0Index].Timestamp, scales[p1Index].Timestamp, time);

            glm::vec3 finalScale = glm::mix(scales[p0Index].Scale, scales[p1Index].Scale, scaleFactor);

            return glm::scale(glm::mat4(1.0f), finalScale);
        }

        void MAnim::UpdateSkeletonTransform(
                const sAnimationNode& animationNode,
                CSkeleton &skeleton,
                sSkeletonBuffer& skeletonBuffer,
                u32 skeletonIndex,
                const glm::mat4 &parentTransform
        )
        {
            const string& nodeName = animationNode.Name;
            glm::mat4 nodeTransform = animationNode.Transform;

            sBone* bone = nullptr;
            auto it = skeleton.Bones.find(nodeName);
            if (it != skeleton.Bones.end())
            {
                bone = &it->second;
            }

            if (bone != nullptr)
            {
                AnimateBone(*bone, s_CurrentSeconds);
                nodeTransform = bone->Transform;
            }

            glm::mat4 globalTransformation = parentTransform * nodeTransform;

            if (bone != nullptr)
            {
                *skeletonBuffer[bone->ID + skeletonIndex] = { globalTransformation * bone->Offset };
            }

            for (auto& node : animationNode.Children)
            {
                UpdateSkeletonTransform(node, skeleton, skeletonBuffer, skeletonIndex, globalTransformation);
            }
        }

        static void ParseAnimation(sAnimationNode& parent, const aiNode* node)
        {
            parent.Name = node->mName.data;
            parent.Transform = MAssimp::ToMat4(node->mTransformation);
            parent.Children.reserve(node->mNumChildren);

            for (int i = 0; i < node->mNumChildren; i++)
            {
                sAnimationNode child;
                ParseAnimation(child, node->mChildren[i]);
                parent.Children.emplace_back(child);
            }
        }

        CAnimation MAnim::LoadAnimation(const char* filepath, const vector<eLoadOption>& options)
        {
            Profile();

            if (MFile::NotExists(filepath))
            {
                LogError("Skeleton Animation not found in {}", filepath);
                return {};
            }

            CAnimation animation;

            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(filepath, MAssimp::GetLoadFlags(options));

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                LogError("Failed to import 3D animation file {0}", filepath);
                return {};
            }

            auto aiAnim = scene->mAnimations[0];
            animation.Duration = aiAnim->mDuration;
            animation.TicksPerSecond = aiAnim->mTicksPerSecond;

            ParseAnimation(animation.Root, scene->mRootNode);

            return animation;
        }

        static void SetKeyFrames(sBone& bone, const aiNodeAnim* channel)
        {
            for (int i = 0; i < channel->mNumPositionKeys; i++)
            {
                aiVector3D aiPosition = channel->mPositionKeys[i].mValue;
                float timestamp = channel->mPositionKeys[i].mTime;
                bone.KeyPositions.emplace_back(timestamp, MAssimp::ToVec3(aiPosition));
            }

            for (int i = 0; i < channel->mNumRotationKeys; i++)
            {
                aiQuaternion aiOrienation = channel->mRotationKeys[i].mValue;
                float timestamp = channel->mRotationKeys[i].mTime;
                bone.KeyRotations.emplace_back(timestamp, MAssimp::ToQuat(aiOrienation));
            }

            for (int i = 0; i < channel->mNumScalingKeys; i++)
            {
                aiVector3D aiScale = channel->mScalingKeys[i].mValue;
                float timestamp = channel->mScalingKeys[i].mTime;
                bone.KeyScales.emplace_back(timestamp, MAssimp::ToVec3(aiScale));
            }
        }

        static void ParseSkeletonFromAnim(const aiAnimation* animation, CSkeleton& skeleton)
        {
            int size = animation->mNumChannels;
            auto& bones = skeleton.Bones;
            usize boneCount = skeleton.Bones.size();

            for (int i = 0; i < size; i++)
            {
                auto channel = animation->mChannels[i];
                string boneName = channel->mNodeName.data;

                auto it = bones.find(boneName);
                if (it == bones.end())
                {
                    sBone bone;
                    bone.ID = boneCount++;
                    bone.Name = boneName;
                    bones.insert({ boneName, bone });
                }

                SetKeyFrames(bones[boneName], channel);
            }
        }

        static void ParseSkeletonFromMesh(aiMesh* mesh, CSkeleton& skeleton)
        {
            s32 boneCounter = 0;
            auto& bones = skeleton.Bones;

            for (int i = 0; i < mesh->mNumBones; i++)
            {
                string boneName = mesh->mBones[i]->mName.C_Str();

                if (bones.find(boneName) == bones.end())
                {
                    sBone newBone;
                    newBone.ID = boneCounter;
                    newBone.Name = boneName;
                    newBone.Offset = MAssimp::ToMat4(mesh->mBones[i]->mOffsetMatrix);
                    bones.insert({ boneName, newBone });
                    boneCounter++;
                }
            }
        }

        static void ParseSkeletonFromScene(aiNode* node, const aiScene* scene, CSkeleton& skeleton)
        {
            for (u32 i = 0 ; i < node->mNumMeshes ; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                ParseSkeletonFromMesh(mesh, skeleton);
            }

            for (u32 i = 0 ; i < node->mNumChildren ; i++)
            {
                ParseSkeletonFromScene(node->mChildren[i], scene, skeleton);
            }
        }

        CSkeleton MAnim::LoadSkeleton(const char* filepath, const vector<eLoadOption>& options)
        {
            Profile();

            if (MFile::NotExists(filepath))
            {
                LogError("Skeleton not found in {}", filepath);
                return {};
            }

            CSkeleton skeleton;

            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(filepath, MAssimp::GetLoadFlags(options));

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                LogError("Failed to import 3D skelet model file {0}", filepath);
                return {};
            }

            ParseSkeletonFromScene(scene->mRootNode, scene, skeleton);
            ParseSkeletonFromAnim(scene->mAnimations[0], skeleton);

            return skeleton;
        }

        CSkeletonInfo MAnim::AddSkeleton(const CSkeleton &skeleton)
        {
            CSkeletonInfo skeletonInfo;
            skeletonInfo.SkeletonIndex = Buffers::Skeleton->Size();
            Buffers::Skeleton->Resize(Buffers::Skeleton->Size() + skeleton.Bones.size());
            return skeletonInfo;
        }
    }
}