#pragma once

#include <anim/skeleton.hpp>

namespace xpe {

    namespace physics
    {
        struct sRagdoll;
    }

    namespace anim {

        using namespace core;
        using namespace ecs;
        using namespace render;
        using namespace physics;

        class ENGINE_API cAnimSystem : public cSystem
        {

        public:
            void Update(cScene* scene, const cTime& dt) override final;

        private:
            void AnimateSkeleton(const sAnimation& animation, sSkeleton& skeleton, u32 skeletonIndex, const cTime& dt);
            void AnimateBone(sBone& bone, float time);

            glm::mat4 InterpolatePosition(sBone& bone, float time);
            glm::mat4 InterpolateRotation(sBone& bone, float time);
            glm::mat4 InterpolateScale(sBone& bone, float time);

            int GetPositionIndex(sBone& bone, float time);
            int GetRotationIndex(sBone& bone, float time);
            int GetScaleIndex(sBone& bone, float time);

            float GetScaleFactor(float lastTimestamp, float nextTimestamp, float time);

            void UpdateSkeletonTransform(
                    const sAnimationNode& animationNode,
                    sSkeleton& skeleton,
                    cSkeletonBuffer& skeletonBuffer,
                    u32 skeletonIndex,
                    const glm::mat4& parentTransform
            );
            
            void ConstructRagdollJoints(sSkeleton& skeleton, sRagdoll& ragdoll);

            float m_CurrentSeconds = 0;
            float m_DeltaSeconds = 0;
        };

    }

}