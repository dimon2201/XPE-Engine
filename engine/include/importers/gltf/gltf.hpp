#pragma once

namespace xpe
{
    namespace gltf
    {
        struct ENGINE_API Buffer
        {
            int ByteLength;
            const char* URI;
            void* Data;
        };

        struct ENGINE_API BufferView
        {
            int BufferId;
            int ByteLength;
            int ByteOffset;
        };

        struct ENGINE_API Vertex
        {
            core::f32 Position[3];
            core::f32 Texcoord[2];
            core::f32 Normal[3];
        };

        struct ENGINE_API Mesh
        {
            core::usize IndexByteSize;
            core::usize VertexCount;
            core::usize IndexCount;
            Vertex* Vertices;
            core::u32* Indices;
        };

        class ENGINE_API GLTFModel
        {
            public:
                static const core::usize k_vertexSize = sizeof(Vertex);
                static const core::usize k_indexSize = sizeof(core::u32);

                GLTFModel(const char* filePath);
                ~GLTFModel();

                // returns null if index is out of bounds
                Mesh* GetMesh(core::usize index);

            private:
                core::vector<Buffer> _buffs;
                core::vector<Mesh> _meshes;
        };
    }
}