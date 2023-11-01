#pragma once

#include <anim/skeleton.h>

namespace xpe {

    namespace anim {

        using namespace core;
        using namespace ecs;
        using namespace render;

        class ENGINE_API cAnimSystem : public cSystem
        {

        public:
            void Update(cScene* scene, const cTime& dt) override final;

        private:
            void AnimateSkeleton(sSkeleton& skeleton, const sAnimation& animation, const cTime& dt);
            void AnimateBone(sBone& bone, float time);

            glm::mat4 InterpolatePosition(sBone& bone, float time);
            glm::mat4 InterpolateRotation(sBone& bone, float time);
            glm::mat4 InterpolateScale(sBone& bone, float time);

            int GetPositionIndex(sBone& bone, float time);
            int GetRotationIndex(sBone& bone, float time);
            int GetScaleIndex(sBone& bone, float time);

            float GetScaleFactor(float lastTimestamp, float nextTimestamp, float time);

            void UpdateSkeletonTransform(
                    sSkeleton& skeleton,
                    sSkeletonBuffer& skeletonBuffer,
                    const sAnimationNode& animationNode,
                    const glm::mat4& parentTransform
            );

            float m_CurrentSeconds = 0;
            float m_DeltaSeconds = 0;
        };

    }

}