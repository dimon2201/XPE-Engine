#include <assimp_manager.hpp>

namespace xpe
{
    namespace res
    {
        const std::unordered_map<eLoadOption, aiPostProcessSteps> MAssimp::k_LoadOptions =
        {
                { eLoadOption::TRIANGULATE, aiProcess_Triangulate },
                { eLoadOption::FLIP_UV, aiProcess_FlipUVs },
                { eLoadOption::CALC_TANGENTS, aiProcess_CalcTangentSpace },
                { eLoadOption::OPTIMIZE_MESHES, aiProcess_OptimizeMeshes },
                { eLoadOption::GEN_NORMAL, aiProcess_GenNormals },
                { eLoadOption::JOIN_VERTICES, aiProcess_JoinIdenticalVertices }
        };
    }
}