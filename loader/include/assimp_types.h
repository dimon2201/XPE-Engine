#pragma once

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace xpe {

    namespace res {

        using namespace core;

        enum class eLoadOption
        {
            TRIANGULATE,
            FLIP_UV,
            CALC_TANGENTS,
            OPTIMIZE_MESHES,
            GEN_NORMAL,
            JOIN_VERTICES
        };

        struct LOADER_API AssimpConversion final
        {
            static inline AssimpConversion& Get() {
                static AssimpConversion instance;
                return instance;
            }

            const std::unordered_map<eLoadOption, aiPostProcessSteps> LoadOptions =
            {
                    { eLoadOption::TRIANGULATE, aiProcess_Triangulate },
                    { eLoadOption::FLIP_UV, aiProcess_FlipUVs },
                    { eLoadOption::CALC_TANGENTS, aiProcess_CalcTangentSpace },
                    { eLoadOption::OPTIMIZE_MESHES, aiProcess_OptimizeMeshes },
                    { eLoadOption::GEN_NORMAL, aiProcess_GenNormals },
                    { eLoadOption::JOIN_VERTICES, aiProcess_JoinIdenticalVertices }
            };

            inline static glm::vec3 ToVec3(const aiVector3D& vec)
            {
                return { vec.x, vec.y, vec.z };
            }

            inline static glm::quat ToQuat(const aiQuaternion& quat)
            {
                return { quat.w, quat.x, quat.y, quat.z };
            }

            inline static glm::mat4 ToMat4(const aiMatrix4x4& mat)
            {
                return {
                        { mat.a1, mat.b1, mat.c1, mat.d1 },
                        { mat.a2, mat.b2, mat.c2, mat.d2 },
                        { mat.a3, mat.b3, mat.c3, mat.d3 },
                        { mat.a4, mat.b4, mat.c4, mat.d4 }
                };
            }

            inline static u32 GetLoadFlags(const vector<eLoadOption>& options) {
                u32 flags = 0;

                for (const auto& option : options)
                {
                    flags |= Get().LoadOptions.at(option);
                }

                return flags;
            }

        };

    }

}