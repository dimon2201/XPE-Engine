#pragma once

namespace xpe {

    namespace ecs {
        class Scene;
    }

    namespace render {
        class BoneBuffer;
    }

    namespace anim {

        class Bone;

        class Animation;
        class AnimationNode;
        class Skelet;

        class SkeletStorage;

        using namespace core;
        using namespace ecs;
        using namespace render;

        class ENGINE_API Animator : public Object
        {

        public:
            Animator(SkeletStorage* skeletStorage) : m_SkeletStorage(skeletStorage) {}

            void Animate(ecs::Scene* scene, const Time& dt);

        private:
            void AnimateSkelet(const Ref<Skelet>& skelet, const Ref<Animation>& animation, const Time& dt);

            void AnimateBone(Bone& bone, float time);

            glm::mat4 InterpolatePosition(Bone& bone, float time);
            glm::mat4 InterpolateRotation(Bone& bone, float time);
            glm::mat4 InterpolateScale(Bone& bone, float time);

            int GetPositionIndex(Bone& bone, float time);
            int GetRotationIndex(Bone& bone, float time);
            int GetScaleIndex(Bone& bone, float time);

            float GetScaleFactor(float lastTimestamp, float nextTimestamp, float time);

            void UpdateSkeletTransform(
                    const Ref<Skelet>& skelet,
                    BoneBuffer& boneBuffer,
                    const AnimationNode& animationNode,
                    const glm::mat4& parentTransform
            );

            SkeletStorage* m_SkeletStorage;
            float m_CurrentSeconds = 0;
            float m_DeltaSeconds = 0;
        };

    }

}