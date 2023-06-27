#pragma once

namespace xpe
{
    namespace gltf
    {
        struct ENGINE_API xBuffer
        {
            int ByteLength;
            const char* URI;
            void* Data;
        };

        struct ENGINE_API xBufferView
        {
            int BufferId;
            int ByteLength;
            int ByteOffset;
        };

        struct ENGINE_API xVertex
        {
            core::f32 Position[3];
            core::f32 Texcoord[2];
            core::f32 Normal[3];
        };

        struct ENGINE_API xMesh
        {
            core::usize IndexByteSize;
            core::usize VertexCount;
            core::usize IndexCount;
            xVertex* Vertices;
            core::u32* Indices;
        };

        class ENGINE_API cGLTFModel
        {
            public:
                static const core::usize k_vertexSize = sizeof(xVertex);
                static const core::usize k_indexSize = sizeof(core::u32);

                cGLTFModel(const char* filePath);
                ~cGLTFModel();

                // returns null if index is out of bounds
                xMesh* GetMesh(core::usize index);

            private:
                std::vector<xBuffer> _buffs;
                std::vector<xMesh> _meshes;
        };
    }
}