#include <anim/animator.h>
#include <anim/storages/skelet_storage.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace anim {

        void Animator::Animate(ecs::Scene *scene, float dt)
        {
            scene->EachComponent<SkeletalAnimationComponent>([this, dt](SkeletalAnimationComponent* component)
            {
                if (component->Play) {
                    AnimateSkelet(component->Skelet, component->Animation, dt);
                }
            });
        }

        void Animator::AnimateSkelet(const Ref<Skelet> &skelet, const Ref<Animation> &animation, float dt)
        {
            m_DeltaTime = dt;
            m_CurrentTime += animation->TicksPerSecond * dt;
            m_CurrentTime = fmod(m_CurrentTime, animation->Duration);
            auto& boneBuffer = skelet->BoneBuffer;

            boneBuffer.Resize(skelet->Bones.size());

            UpdateSkeletTransform(skelet, boneBuffer, animation->Root, glm::mat4(1.0f));

            boneBuffer.Flush();
        }

        void Animator::AnimateBone(Bone &bone, float time)
        {
            glm::mat4 translation = InterpolatePosition(bone, time);
            glm::mat4 rotation = InterpolateRotation(bone, time);
            glm::mat4 scale = InterpolateScale(bone, time);
            bone.Transform = translation * rotation * scale;
        }

        int Animator::GetPositionIndex(Bone &bone, float time)
        {
            usize count = bone.KeyPositions.size();

            for (int i = 0; i < count; i++)
            {
                if (time < bone.KeyPositions[i + 1].Timestamp) {
                    return i;
                }
            }

            return -1;
        }

        int Animator::GetRotationIndex(Bone &bone, float time)
        {
            usize count = bone.KeyRotations.size();

            for (int i = 0; i < count; i++)
            {
                if (time < bone.KeyRotations[i + 1].Timestamp) {
                    return i;
                }
            }

            return -1;
        }

        int Animator::GetScaleIndex(Bone &bone, float time)
        {
            usize count = bone.KeyScales.size();

            for (int i = 0; i < count; i++)
            {
                if (time < bone.KeyScales[i + 1].Timestamp) {
                    return i;
                }
            }

            return -1;
        }

        float Animator::GetScaleFactor(float lastTimestamp, float nextTimestamp, float time)
        {
            float scaleFactor;
            float midWayLength = time - lastTimestamp;
            float framesDiff = nextTimestamp - lastTimestamp;
            scaleFactor = midWayLength / framesDiff;
            return scaleFactor;
        }

        glm::mat4 Animator::InterpolatePosition(Bone &bone, float time)
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

        glm::mat4 Animator::InterpolateRotation(Bone &bone, float time)
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

        glm::mat4 Animator::InterpolateScale(Bone &bone, float time)
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

        void Animator::UpdateSkeletTransform(
            const Ref<Skelet>& skelet,
            BoneBuffer& boneBuffer,
            const AnimationNode& animationNode,
            const glm::mat4 &parentTransform
        ) {
            string nodeName = animationNode.Name;
            glm::mat4 nodeTransform = animationNode.Transform;

            Bone* bone = nullptr;
            auto it = skelet->Bones.find(nodeName);
            if (it != skelet->Bones.end()) {
                bone = &it->second;
            }

            if (bone != nullptr) {
                AnimateBone(*bone, m_CurrentTime);
                nodeTransform = bone->Transform;
            }

            glm::mat4 globalTransformation = parentTransform * nodeTransform;

            if (bone != nullptr) {
                *boneBuffer[bone->ID] = { globalTransformation * bone->Offset };
            }

            for (auto& node : animationNode.Children)
            {
                UpdateSkeletTransform(skelet, boneBuffer, node, globalTransformation);
            }
        }

    }

}