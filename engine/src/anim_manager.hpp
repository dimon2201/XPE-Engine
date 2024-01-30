#pragma once

#include <components.hpp>

namespace xpe
{
    namespace anim
    {
        class ENGINE_API MAnim final
        {

        public:
            static void Init();
            static void Free();
            static void Update(cScene* scene, float dt);

            static CAnimation LoadAnimation(
                    const char* filepath,
                    const vector<eLoadOption>& options = { eLoadOption::TRIANGULATE }
            );

            static CSkeleton LoadSkeleton(
                    const char* filepath,
                    const vector<eLoadOption>& options = { eLoadOption::TRIANGULATE }
            );

            static CSkeletonInfo AddSkeleton(const CSkeleton& skeleton);

        private:
            static void AnimateSkeleton(const CAnimation& animation, CSkeleton& skeleton, u32 skeletonIndex, float dt);
            static void AnimateBone(sBone& bone, float time);

            static glm::mat4 InterpolatePosition(sBone& bone, float time);
            static glm::mat4 InterpolateRotation(sBone& bone, float time);
            static glm::mat4 InterpolateScale(sBone& bone, float time);

            static int GetPositionIndex(sBone& bone, float time);
            static int GetRotationIndex(sBone& bone, float time);
            static int GetScaleIndex(sBone& bone, float time);

            static float GetScaleFactor(float lastTimestamp, float nextTimestamp, float time);

            static void UpdateSkeletonTransform(
                    const sAnimationNode& animationNode,
                    CSkeleton& skeleton,
                    sSkeletonBuffer& skeletonBuffer,
                    u32 skeletonIndex,
                    const glm::mat4& parentTransform
            );

            static float s_CurrentSeconds;
            static float s_DeltaSeconds;
        };
    }
}