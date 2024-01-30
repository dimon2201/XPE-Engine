#pragma once

#include <components.hpp>

namespace xpe
{
    namespace render
    {
        class ENGINE_API MGeometry final
        {

        public:
            static void Init();
            static void Free();
            static CGeometryInfo Add(const CGeometry& geometry);
            static CGeometry Load(const char* filepath, const vector<eLoadOption>& options =
            {
                    eLoadOption::TRIANGULATE,
                    eLoadOption::FLIP_UV,
                    eLoadOption::CALC_TANGENTS,
                    eLoadOption::OPTIMIZE_MESHES,
                    eLoadOption::GEN_NORMAL,
                    eLoadOption::JOIN_VERTICES
            });
            static void Merge(CGeometry& geometry);
        };
    }
}