#pragma once

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace xpe {

    namespace res {

        struct LOADER_API AssimpConversion final
        {

            inline static glm::vec3 ToVec3(const aiVector3D& vec)
            {
                return { vec.x, vec.y, vec.z };
            }

            inline static glm::quat ToQuat(const aiQuaternion& quat)
            {
                return { quat.x, quat.y, quat.z, quat.w };
            }

            inline static glm::mat4 ToMat4(const aiMatrix4x4& mat)
            {
                return {
                    { mat.a1, mat.a2, mat.a3, mat.a4 },
                    { mat.b1, mat.b2, mat.b3, mat.b4 },
                    { mat.c1, mat.c2, mat.c3, mat.c4 },
                    { mat.d1, mat.d2, mat.d3, mat.d4 }
                };
            }

        };

    }

}