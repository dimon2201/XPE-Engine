#pragma once

#include <core/windowing.hpp>

#include <rendering/core/viewport.h>
#include <rendering/core/pipeline.h>

namespace xpe {

    namespace render {

        class RenderTarget;

        class Shader;
        class ShaderStage;
        enum class eShaderType;

        class InputLayout;

        class Texture;
        class TextureSampler;

        class Pipeline;

        class DepthStencilState;

        class BlendState;

        class Rasterizer;

        namespace context {

            extern void* SwapchainTargetView;
            extern Viewport* BoundViewport;
            extern void* BoundColorTargetView;
            extern void* BoundDepthTargetView;
            extern Shader* BoundShader;
            extern Pipeline* BoundPipeline;

            ENGINE_API void Init();
            ENGINE_API void Free();

            ENGINE_API void CreateSwapchain(int width, int height);
            ENGINE_API void FreeSwapchain();
            ENGINE_API void ResizeSwapchain(RenderTarget& presentTarget, int width, int height);
            ENGINE_API void CreateSwapchainTargetView();

            ENGINE_API void CreateRenderTarget(RenderTarget& renderTarget);
            ENGINE_API void BindRenderTarget(void* colorTargetView, void* depthTargetView);
            ENGINE_API void UnbindRenderTarget();
            ENGINE_API void ClearColorTarget(const glm::vec4& color);
            ENGINE_API void ClearDepthTarget(const f32 depth);
            ENGINE_API void FreeRenderTarget(RenderTarget& renderTarget);
            ENGINE_API void ResizeRenderTarget(RenderTarget& renderTarget, int width, int height);

            ENGINE_API void Present();

            ENGINE_API void CreateShader(Shader& shader);
            ENGINE_API void CompileShader(Shader& shader);
            ENGINE_API void BindShader(const Shader* shader);
            ENGINE_API void FreeShader(Shader& shader);

            ENGINE_API void CompileShaderStage(ShaderStage& stage);
            ENGINE_API void CreateShaderStage(ShaderStage& stage);
            ENGINE_API void BindShaderStage(const ShaderStage& stage);
            ENGINE_API void FreeShaderStage(ShaderStage& stage);

            ENGINE_API void CreateTexture(Texture& texture);

            ENGINE_API void CreateTexture1D(Texture& texture);
            ENGINE_API void CreateTexture2D(Texture& texture);
            ENGINE_API void CreateTexture2DArray(Texture& texture);
            ENGINE_API void CreateTexture3D(Texture& texture);
            ENGINE_API void CreateTextureCube(Texture& texture);
            ENGINE_API void CreateTextureDepthStencil(Texture& texture);

            ENGINE_API void BindTexture(const Texture* texture);
            ENGINE_API void BindTextureSlot(u32 slot);
            ENGINE_API void FreeTexture(Texture& texture);
            ENGINE_API void WriteTexture(const Texture& texture, const void* pixels, usize pixelsSize, u32 index = 0);

            ENGINE_API void GenerateMips(const Texture& texture);

            ENGINE_API void CreateSampler(TextureSampler& sampler);
            ENGINE_API void BindSampler(const TextureSampler* sampler);
            ENGINE_API void FreeSampler(const TextureSampler* sampler);

            ENGINE_API void CreateBuffer(Buffer& buffer);
            ENGINE_API void BindVertexBuffer(const Buffer* buffer);
            ENGINE_API void BindIndexBuffer(const Buffer* buffer);
            ENGINE_API void BindVSBuffer(const Buffer* buffer);
            ENGINE_API void BindPSBuffer(const Buffer* buffer);
            ENGINE_API void WriteBuffer(const Buffer& buffer, const void* data, usize dataByteSize);
            ENGINE_API void WriteBufferOffset(const Buffer& buffer, usize offset, const void* data, usize dataByteSize);
            ENGINE_API void WriteBufferAppend(Buffer& buffer, const void* data, usize dataByteSize);
            ENGINE_API void FreeBuffer(const Buffer& buffer);

            ENGINE_API void CreateInputLayout(InputLayout& inputLayout);
            ENGINE_API void BindInputLayout(const InputLayout* inputLayout);
            ENGINE_API void FreeInputLayout(InputLayout& inputLayout);

            ENGINE_API void BindPrimitiveTopology(const ePrimitiveTopology& primitiveTopology);

            ENGINE_API void BindViewport(Viewport* viewport);

            ENGINE_API void CreatePipeline(Pipeline& pipeline);
            ENGINE_API void BindPipeline(const Pipeline* pipeline);
            ENGINE_API void FreePipeline(Pipeline& pipeline);

            ENGINE_API void CreateDepthStencilState(DepthStencilState& state);
            ENGINE_API void BindDepthStencilState(const DepthStencilState* state);
            ENGINE_API void FreeDepthStencilState(DepthStencilState& state);

            ENGINE_API void CreateRasterizer(Rasterizer& rasterizer);
            ENGINE_API void BindRasterizer(const Rasterizer* rasterizer);
            ENGINE_API void FreeRasterizer(Rasterizer& rasterizer);

            ENGINE_API void CreateBlendState(BlendState& state);
            ENGINE_API void BindBlendState(const BlendState* state);
            ENGINE_API void FreeBlendState(BlendState& state);

            ENGINE_API void DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount);
            ENGINE_API void DrawBatch(usize vertexOffset, usize vertexCount, usize instanceCount);

            ENGINE_API void DrawQuad();

            ENGINE_API void* GetDevice();

            ENGINE_API void CreateHardwareConfig();

        }

    }

}
