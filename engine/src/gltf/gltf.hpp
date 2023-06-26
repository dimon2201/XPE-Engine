#pragma once

#include <vector>

#include "../core/types.hpp"

namespace xpe
{
    namespace gltf
    {
        struct xBuffer
        {
            int ByteLength;
            const char* URI;
            void* Data;
        };

        struct xBufferView
        {
            int BufferId;
            int ByteLength;
            int ByteOffset;
        };

        struct xVertex
        {
            core::f32 Position[3];
            core::f32 Texcoord[2];
            core::f32 Normal[3];
        };

        struct xMesh
        {
            core::usize IndexByteSize;
            core::usize VertexCount;
            core::usize IndexCount;
            xVertex* Vertices;
            core::u32* Indices;
        };

        class cGLTFModel
        {
            public:
                static const core::usize k_vertexSize = sizeof(xVertex);
                static const core::usize k_indexSize = sizeof(core::u32);

                cGLTFModel(const char* filePath);
                ~cGLTFModel();

                inline xMesh* GetMesh(core::usize index) { return &_meshes[index]; }

            private:
                std::vector<xBuffer> _buffs;
                std::vector<xMesh> _meshes;
        };
    }
}