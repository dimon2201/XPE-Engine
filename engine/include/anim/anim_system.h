#pragma once

#include <anim/skeleton.h>

namespace xpe {

    namespace anim {

        using namespace core;
        using namespace ecs;
        using namespace render;

        class ENGINE_API AnimSystem : public System
        {

        public:
            void Update(Scene* scene, const Time& dt) override final;

        private:
            void AnimateSkeleton(Skeleton& skeleton, const Animation& animation, const Time& dt);
            void AnimateBone(Bone& bone, float time);

            glm::mat4 InterpolatePosition(Bone& bone, float time);
            glm::mat4 InterpolateRotation(Bone& bone, float time);
            glm::mat4 InterpolateScale(Bone& bone, float time);

            int GetPositionIndex(Bone& bone, float time);
            int GetRotationIndex(Bone& bone, float time);
            int GetScaleIndex(Bone& bone, float time);

            float GetScaleFactor(float lastTimestamp, float nextTimestamp, float time);

            void UpdateSkeletonTransform(
                    Skeleton& skeleton,
                    BoneBuffer& boneBuffer,
                    const AnimationNode& animationNode,
                    const glm::mat4& parentTransform
            );

            float m_CurrentSeconds = 0;
            float m_DeltaSeconds = 0;
        };

    }

}