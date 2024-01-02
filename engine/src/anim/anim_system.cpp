#include <anim/anim_system.hpp>
#include <anim/skeleton_manager.hpp>

#include <ecs/components.hpp>

namespace xpe {

    namespace anim {

        void cAnimSystem::Update(cScene* scene, const cTime& dt)
        {
            auto components = scene->GetComponents<CAnimation, CSkeleton, CSkeletonInfo>();
            for (auto [entity, animation, skeleton, skeletonInfo] : components.each())
            {
                for (auto& subAnimation : animation.Animations) {
                    if (subAnimation.Play) {
                        AnimateSkeleton(subAnimation, skeleton, skeletonInfo.SkeletonIndex, dt);
                    }
                }
            }
        }

        void cAnimSystem::AnimateSkeleton(const sAnimation &animation, sSkeleton &skeleton, u32 skeletonIndex, const cTime& dt)
        {
            m_DeltaSeconds = dt.Seconds();
            m_CurrentSeconds += animation.TicksPerSecond * m_DeltaSeconds;
            m_CurrentSeconds = fmod(m_CurrentSeconds, animation.Duration);
            UpdateSkeletonTransform(animation.Root, skeleton, *Buffers::Skeleton, skeletonIndex, glm::mat4(1.0f));
        }

        void cAnimSystem::AnimateBone(sBone &bone, float time)
        {
            glm::mat4 translation = InterpolatePosition(bone, time);
            glm::mat4 rotation = InterpolateRotation(bone, time);
            glm::mat4 scale = InterpolateScale(bone, time);
            bone.Transform = translation * rotation * scale;
        }

        int cAnimSystem::GetPositionIndex(sBone &bone, float time)
        {
            usize count = bone.KeyPositions.size();

            for (int i = 0; i < count; ++i)
            {
                if (time < bone.KeyPositions[i + 1].Timestamp) {
                    return i;
                }
            }

            return -1;
        }

        int cAnimSystem::GetRotationIndex(sBone &bone, float time)
        {
            usize count = bone.KeyRotations.size();

            for (int i = 0; i < count; ++i)
            {
                if (time < bone.KeyRotations[i + 1].Timestamp) {
                    return i;
                }
            }

            return -1;
        }

        int cAnimSystem::GetScaleIndex(sBone &bone, float time)
        {
            usize count = bone.KeyScales.size();

            for (int i = 0; i < count; ++i)
            {
                if (time < bone.KeyScales[i + 1].Timestamp) {
                    return i;
                }
            }

            return -1;
        }

        float cAnimSystem::GetScaleFactor(float lastTimestamp, float nextTimestamp, float time)
        {
            float scaleFactor;
            float midWayLength = time - lastTimestamp;
            float framesDiff = nextTimestamp - lastTimestamp;
            scaleFactor = midWayLength / framesDiff;
            return scaleFactor;
        }

        glm::mat4 cAnimSystem::InterpolatePosition(sBone &bone, float time)
        {
            auto& positions = bone.KeyPositions;

            if (positions.empty()) {
                return glm::mat4(1.0f);
            }

            if (positions.size() == 1) {
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

        glm::mat4 cAnimSystem::InterpolateRotation(sBone &bone, float time)
        {
            auto& rotations = bone.KeyRotations;

            if (rotations.empty()) {
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

        glm::mat4 cAnimSystem::InterpolateScale(sBone &bone, float time)
        {
            auto& scales = bone.KeyScales;

            if (scales.empty()) {
                return glm::mat4(1.0f);
            }

            if (scales.size() == 1) {
                return glm::scale(glm::mat4(1.0f), scales[0].Scale);
            }

            int p0Index = GetScaleIndex(bone, time);
            int p1Index = p0Index + 1;

            float scaleFactor = GetScaleFactor(
                    scales[p0Index].Timestamp,
                    scales[p1Index].Timestamp,
                    time
            );

            glm::vec3 finalScale = glm::mix(
                    scales[p0Index].Scale,
                    scales[p1Index].Scale,
                    scaleFactor
            );

            return glm::scale(glm::mat4(1.0f), finalScale);
        }

        void cAnimSystem::UpdateSkeletonTransform(
                const sAnimationNode& animationNode,
                sSkeleton &skeleton,
                cSkeletonBuffer& skeletonBuffer,
                u32 skeletonIndex,
                const glm::mat4 &parentTransform
        ) {
            const string& nodeName = animationNode.Name;
            glm::mat4 nodeTransform = animationNode.Transform;

            sBone* bone = nullptr;
            auto it = skeleton.Bones.find(nodeName);
            if (it != skeleton.Bones.end()) {
                bone = &it->second;
            }

            if (bone != nullptr) {
                AnimateBone(*bone, m_CurrentSeconds);
                nodeTransform = bone->Transform;

                //nodeTransform = cPhysicsManager::GetRagdollBodypartMatrix(ragdoll, nodeName);
            }

            glm::mat4 globalTransformation = parentTransform * nodeTransform;

            if (bone != nullptr) {
                *skeletonBuffer[bone->ID + skeletonIndex] = { globalTransformation * bone->Offset };
            }

            for (auto& node : animationNode.Children)
            {
                UpdateSkeletonTransform(node, skeleton, skeletonBuffer, skeletonIndex, globalTransformation);
            }
        }

    }

}