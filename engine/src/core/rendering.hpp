#pragma once

#include <map>
#include <string>
#include <glm.hpp>

#include "windowing.hpp"

namespace xpe
{
    namespace core
    {
        struct GPUResource
        {
            void* Instance;
            void* ViewInstance;
        };

        struct xRenderTarget
        {
            GPUResource* ColorTexture;
            GPUResource* DepthTexture;
            void* ColorTargetInstance;
            void* DepthTargetInstance;
            glm::ivec2 Dimensions;
        };

        struct xShader
        {
            enum class Type
            {
                VERTEX = 0,
                PIXEL = 1
            };

            enum class PrimitiveTopology
            {
                TRIANGLE_LIST = 0,
                TRIANGLE_STRIP = 1
            };

            PrimitiveTopology Topology;
            GPUResource VertexShader;
            GPUResource PixelShader;
            void* VertexShaderByteCode;
            usize VertexShaderByteCodeSize;
        };

        struct xBuffer
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

        struct xInputLayout
        {
            struct xEntry
            {
                enum class eFormat
                {
                    VEC3 = 0
                };

                const char* Name;
                eFormat Format;
                usize ByteSize;
            };

            GPUResource InputLayout;
            void* VertexShaderByteCode;
            usize VertexShaderByteCodeSize;
            usize EntryCount;
            xEntry Entries[16];
        };

        struct xPipeline
        {
            xInputLayout InputLayout;
            xBuffer* InputVertexBuffer;
            xBuffer* InputIndexBuffer;
            xBuffer* InputInstanceBuffer;
            xBuffer* InputConstantBuffer;
            xShader* Shaders;
            xRenderTarget* RenderTarget;
        };

        class RenderingContext_Interface
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
                virtual void CreateShaderFromString(xShader& shader, const xShader::PrimitiveTopology& topology, const xShader::Type& type, const char* str, const char* funcName, const char* profile, const uword flags) = 0;
                virtual void BindShader(const xShader* shader) = 0;
                virtual void FreeShader(const xShader& shader) = 0;
                virtual GPUResource CreateTexture(const void* texture, const glm::ivec2& dimensions) = 0;
                virtual void BindTexture(const GPUResource* texture, const xShader::Type& shaderType, const u32 slot) = 0;
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
                virtual void FreeInputLayout(const xInputLayout& inputLayout) = 0;
                virtual void BindViewport(const glm::vec4& coords, f32 minDepth, f32 maxDepth) = 0;
                virtual void BindRenderPipeline(const xPipeline* pipeline) = 0;
                virtual void DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount) = 0;
                virtual void DrawQuad() = 0;
                virtual void OutputErrors() = 0;
        };

        struct xGeometryInfo
        {
            xGeometryInfo(usize vertexBufferOffset, usize indexBufferOffset, usize vertexCount, usize indexCount) :
                VertexBufferOffset(vertexBufferOffset), IndexBufferOffset(indexBufferOffset), VertexCount(vertexCount), IndexCount(indexCount)
            {}

            usize VertexBufferOffset;
            usize IndexBufferOffset;
            usize VertexCount;
            usize IndexCount;
        };

        struct xBatch
        {
            std::string GeometryUSID;
            xGeometryInfo* GeometryInfo;
            usize InstanceCount;
        };

        struct xRenderInstance
        {
            glm::vec4 Position;
        };

        struct xConstantBuffer
        {
            glm::mat4 ViewerViewProjection;
        };

        class BatchManager
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