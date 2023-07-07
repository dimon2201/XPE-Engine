#pragma once

#include <core/windowing.hpp>

#include <rendering/core.h>

namespace xpe {

    namespace render {

        class RenderTarget;

        class Shader;
        enum class eShaderType;

        class InputLayout;

        class Texture;
        class TextureSampler;

        class Pipeline;

        class DepthStencilState;

        class ENGINE_API Context {

            public:
                Context() {}
                ~Context() {}

                virtual void Init(Window& window) = 0;
                virtual void Free() = 0;
                
                virtual RenderTarget CreateRenderTarget(const glm::ivec2& dimensions, const GPUResource* colorTexture, const GPUResource* colorView, const GPUResource* depthTexture, const GPUResource* depthView) = 0;
                virtual void BindRenderTarget(const RenderTarget* renderTarget) = 0;
                virtual void ClearRenderTarget(const glm::vec4& color, const f32 depth) = 0;
                virtual glm::ivec2 GetSwapChainDimensions() = 0;
                virtual void FreeRenderTarget(const RenderTarget& renderTarget) = 0;
                virtual void Present() = 0;
                
                virtual void CreateShader(Shader& shader) = 0;
                virtual void BindShader(const Shader* shader) = 0;
                virtual void FreeShader(Shader& shader) = 0;
                
                virtual void CreateTexture(Texture& texture, const void* instance) = 0;
                virtual void BindTexture(const Texture* texture, const eShaderType& shaderType) = 0;
                virtual void BindTexture(const eShaderType& shaderType, u32 slot) = 0;
                virtual void FreeTexture(const Texture* texture) = 0;
                virtual void WriteTexture(const Texture& texture, const void* pixels, usize pixelsSize) = 0;

                virtual void CreateSampler(TextureSampler& sampler) = 0;
                virtual void BindSampler(const TextureSampler* sampler) = 0;
                virtual void FreeSampler(const TextureSampler* sampler) = 0;
                
                virtual void CreateBuffer(Buffer& buffer, boolean duplicate) = 0;
                virtual void BindVertexBuffer(const Buffer* buffer) = 0;
                virtual void BindIndexBuffer(const Buffer* buffer) = 0;
                virtual void BindVSBuffer(const Buffer* buffer) = 0;
                virtual void BindPSBuffer(const Buffer* buffer) = 0;
                virtual void WriteBuffer(const Buffer& buffer, const void* data, usize dataByteSize) = 0;
                virtual void WriteBufferOffset(const Buffer& buffer, usize offset, const void* data, usize dataByteSize) = 0;
                virtual void WriteBufferAppend(Buffer& buffer, const void* data, usize dataByteSize) = 0;
                virtual void FreeBuffer(const Buffer& buffer) = 0;

                virtual void CreateInputLayout(InputLayout& inputLayout) = 0;
                virtual void BindInputLayout(const InputLayout* inputLayout) = 0;
                virtual void FreeInputLayout(const InputLayout& inputLayout) = 0;
                
                virtual void BindViewport(const glm::vec4& coords, f32 minDepth, f32 maxDepth) = 0;

                virtual void CreateRenderPipeline(Pipeline& pipeline) = 0;
                virtual void BindRenderPipeline(const Pipeline* pipeline) = 0;
                virtual void FreeRenderPipeline(Pipeline& pipeline) = 0;

                virtual void CreateDepthStencilState(DepthStencilState& state) = 0;
                virtual void BindDepthStencilState(const DepthStencilState* state) = 0;
                virtual void FreeDepthStencilState(DepthStencilState& state) = 0;

                virtual void DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount) = 0;
                virtual void DrawQuad() = 0;

                virtual void* GetDevice() = 0;

        };

    }

}
