#pragma once

#include <anim/anim.h>

#include <rendering/buffers/skeleton_buffer.h>
#include <rendering/geometry/geometry.h>

namespace xpe {

    namespace anim {

        struct ENGINE_API sBone final
        {
            s32 ID;
            string Name;
            glm::mat4 Offset;
            glm::mat4 Transform = glm::mat4(1.0);
            vector<sKeyPosition> KeyPositions;
            vector<sKeyRotate> KeyRotations;
            vector<sKeyScale> KeyScales;
        };
        JSON(sBone, ID, Name, Offset, Transform, KeyPositions, KeyRotations, KeyScales)

        struct ENGINE_API sSkeleton
        {
            unordered_map<string, sBone> Bones;
        };
        JSON(sSkeleton, Bones)

        struct ENGINE_API sSkeletonInfo
        {
            render::sGeometryInfo GeometryInfo;
            u32 SkeletonIndex = 0;
        };

    }

}