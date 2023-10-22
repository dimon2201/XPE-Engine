#pragma once

#include <anim/skeleton.h>

namespace xpe {

    namespace anim {

        using namespace core;
        using namespace ecs;
        using namespace render;

        class ENGINE_API AnimManager : public Manager
        {

        public:
            void Update(ecs::Scene* scene, const Time& dt) override final;

        private:
            void AnimateSkelet(const Ref<Skeleton>& skelet, const Ref<Animation>& animation, const Time& dt);

            void AnimateBone(Bone& bone, float time);

            glm::mat4 InterpolatePosition(Bone& bone, float time);
            glm::mat4 InterpolateRotation(Bone& bone, float time);
            glm::mat4 InterpolateScale(Bone& bone, float time);

            int GetPositionIndex(Bone& bone, float time);
            int GetRotationIndex(Bone& bone, float time);
            int GetScaleIndex(Bone& bone, float time);

            float GetScaleFactor(float lastTimestamp, float nextTimestamp, float time);

            void UpdateSkeletTransform(
                    const Ref<Skeleton>& skelet,
                    BoneBuffer& boneBuffer,
                    const AnimationNode& animationNode,
                    const glm::mat4& parentTransform
            );

            float m_CurrentSeconds = 0;
            float m_DeltaSeconds = 0;
        };

    }

}