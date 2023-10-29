#pragma once

#include <anim/anim.h>

#include <rendering/buffers/bone_buffer.h>

namespace xpe {

    namespace anim {

        struct ENGINE_API Bone final
        {
            s32 ID;
            string Name;
            glm::mat4 Offset;
            glm::mat4 Transform = glm::mat4(1.0);
            vector<KeyPosition> KeyPositions;
            vector<KeyRotate> KeyRotations;
            vector<KeyScale> KeyScales;
        };

        Json(Bone, ID, Name, Offset, Transform, KeyPositions, KeyRotations, KeyScales)

        struct ENGINE_API Skeleton
        {
            u32 Index = 0;
            unordered_map<string, Bone> Bones;
        };

        Json(Skeleton, Bones)

    }

}