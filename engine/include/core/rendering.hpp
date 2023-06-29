#pragma once

#include <core/windowing.hpp>

namespace xpe
{
    namespace core
    {
        struct ENGINE_API GPUResource
        {
            void* Instance;
            void* ViewInstance;
        };

        struct ENGINE_API xRenderTarget
        {
            GPUResource* ColorTexture;
            GPUResource* DepthTexture;
            void* ColorTargetInstance;
            void* DepthTargetInstance;
            glm::ivec2 Dimensions;
        };

        enum class ePrimitiveTopology
        {
            TRIANGLE_LIST = 0,
            TRIANGLE_STRIP = 1
        };

        struct ENGINE_API xShader
        {
            enum class eType
            {
                VERTEX = 0,
                PIXEL = 1,
                VERTEX_PIXEL = 2
            };

            ePrimitiveTopology PrimitiveTopology;
            eType Type;
            char* Sources[3];
            char* SourceEntryPoints[3];
            char* SourceProfiles[3];
            uword SourceFlags[3];
            GPUResource _VertexShader;
            GPUResource _PixelShader;
            void* _VertexShaderByteCode;
            usize _VertexShaderByteCodeSize;
        };

        struct ENGINE_API xBuffer
        {
            enum class xType
            {
                VERTEX = 0,
                INDEX = 1,
                INSTANCE = 2,
                CONSTANT = 3
            };

            xType Type;
            GPUResource BufferResource;
            void* CPUMemory;
            usize AppendOffset;
        };

        struct ENGINE_API xInputLayout
        {
            struct ENGINE_API xEntry
            {
                enum class eFormat
                {
                    VEC2 = 0,
                    VEC3 = 1,
                    VEC4 = 2
                };

                const char* Name;
                eFormat Format;
                usize ByteSize;
            };

            ePrimitiveTopology PrimitiveTopology;
            GPUResource _InputLayout;
            void* _VertexShaderByteCode;
            usize _VertexShaderByteCodeSize;
            usize EntryCount;
            xEntry Entries[16];
            usize StrideByteSize;
        };

        struct ENGINE_API xDepthStencilState
        {
            GPUResource Instance;
            boolean UseDepthTest;
        };

        struct ENGINE_API xPipeline
        {
            xInputLayout InputLayout;
            xBuffer* VertexBuffer;
            xBuffer* IndexBuffer;
            xBuffer* InstanceBuffer;
            xBuffer* ConstantBuffer;
            xShader* Shaders;
            xRenderTarget* RenderTarget;
            xDepthStencilState DepthStencilState;
        };

        class ENGINE_API RenderingContext_Interface
        {
            public:
                RenderingContext_Interface() {}
                ~RenderingContext_Interface() {}

                virtual void Init(Window& window) = 0;
                virtual void Free() = 0;
                
                virtual xRenderTarget CreateRenderTarget(const glm::ivec2& dimensions, const GPUResource* colorTexture, const GPUResource* colorView, const GPUResource* depthTexture, const GPUResource* depthView) = 0;
                virtual void BindRenderTarget(const xRenderTarget* renderTarget) = 0;
                virtual void ClearRenderTarget(const glm::vec4& color, const f32 depth) = 0;
                virtual glm::ivec2 GetSwapChainDimensions() = 0;
                virtual void FreeRenderTarget(const xRenderTarget& renderTarget) = 0;
                virtual void Present() = 0;
                
                virtual void CreateShaderFromString(xShader& shader) = 0;
                virtual void BindShader(const xShader* shader) = 0;
                virtual void FreeShader(const xShader& shader) = 0;
                
                virtual GPUResource CreateTexture(const void* texture, const glm::ivec2& dimensions) = 0;
                virtual void BindTexture(const GPUResource* texture, const xShader::eType& shaderType, const u32 slot) = 0;
                virtual void FreeTexture(const GPUResource* texture) = 0;
                
                virtual GPUResource CreateSampler() = 0;
                virtual void BindSampler(const GPUResource* sampler) = 0;
                virtual void FreeSampler(const GPUResource* sampler) = 0;
                
                virtual xBuffer CreateBuffer(const xBuffer::xType& bufferType, usize byteSize, boolean duplicate) = 0;
                virtual void BindBuffer(const xBuffer* buffer) = 0;
                virtual void WriteBuffer(const xBuffer& buffer, const void* data, usize dataByteSize) = 0;
                virtual void WriteBufferOffset(const xBuffer& buffer, usize offset, const void* data, usize dataByteSize) = 0;
                virtual void WriteBufferAppend(xBuffer& buffer, const void* data, usize dataByteSize) = 0;
                virtual void FreeBuffer(const xBuffer& buffer) = 0;

                virtual void CreateInputLayout(xInputLayout& inputLayout) = 0;
                virtual void BindInputLayout(const xInputLayout* inputLayout) = 0;
                virtual void FreeInputLayout(const xInputLayout& inputLayout) = 0;
                
                virtual void BindViewport(const glm::vec4& coords, f32 minDepth, f32 maxDepth) = 0;

                virtual void CreateRenderPipeline(xPipeline& pipeline) = 0;
                virtual void BindRenderPipeline(const xPipeline* pipeline) = 0;
                virtual void FreeRenderPipeline(xPipeline& pipeline) = 0;

                virtual void CreateDepthStencilState(xDepthStencilState& state) = 0;
                virtual void BindDepthStencilState(const xDepthStencilState* state) = 0;
                virtual void FreeDepthStencilState(xDepthStencilState& state) = 0;

                virtual void DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount) = 0;
                virtual void DrawQuad() = 0;

                virtual void* GetDevice() = 0;
        };

        struct ENGINE_API xGeometryInfo
        {
            xGeometryInfo(usize vertexBufferOffset, usize indexBufferOffset, usize vertexCount, usize indexCount) :
                VertexBufferOffset(vertexBufferOffset), IndexBufferOffset(indexBufferOffset), VertexCount(vertexCount), IndexCount(indexCount)
            {}

            usize VertexBufferOffset;
            usize IndexBufferOffset;
            usize VertexCount;
            usize IndexCount;
        };

        struct ENGINE_API xBatch
        {
            std::string GeometryUSID;
            xGeometryInfo* GeometryInfo;
            usize InstanceCount;
        };

        struct ENGINE_API xRenderInstance
        {
            glm::vec4 Position;
        };

        struct ENGINE_API xConstantBuffer
        {
            glm::mat4 ViewerViewProjection;
        };

        class ENGINE_API BatchManager
        {
            public:
                static const usize k_vertexBufferByteSize = K_MEMORY_MIB;
                static const usize k_indexBufferByteSize = K_MEMORY_MIB;
                static const usize k_instanceBufferInstanceCount = 1000000;
                static const usize k_instanceBufferByteSize = sizeof(xRenderInstance) * k_instanceBufferInstanceCount;
                static const usize k_constantBufferByteSize = K_MEMORY_KIB;

                BatchManager(RenderingContext_Interface* context);
                ~BatchManager();

                void StoreGlobalGeometryData(const std::string& usid, const void* vertices, usize verticesByteSize, usize vertexCount, const void* indices, usize indicesByteSize, usize indexCount);
                void BeginBatch(const std::string& geometryUSID);
                void RecordConstantBuffer(const xConstantBuffer* buffer);
                void RecordInstance(const xRenderInstance& instance);
                void EndBatch();
                void DrawBatch();

                inline xBuffer* GetVertexBuffer() { return &_vertex; }
                inline xBuffer* GetIndexBuffer() { return &_index; }
                inline xBuffer* GetInstanceBuffer() { return &_instance; }
                inline xBuffer* GetConstantBuffer() { return &_constant; }

            private:
                RenderingContext_Interface* _context;
                xBuffer _vertex;
                xBuffer _index;
                xBuffer _instance;
                xBuffer _constant;
                std::map<std::string, xGeometryInfo> _geometries;
                xBatch _batch;
        };
    }
}