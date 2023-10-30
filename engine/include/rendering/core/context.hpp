#pragma once

#include <rendering/core/viewport.h>
#include <rendering/core/pipeline.h>

namespace xpe {

    namespace render {

        namespace context {

            extern void* SwapchainTargetView;
            extern void* SwapchainTextureInstance;

            ENGINE_API void Init();
            ENGINE_API void Free();

            ENGINE_API void CreateSwapchain(int width, int height);
            ENGINE_API void FreeSwapchain();
            ENGINE_API void ResizeSwapchain(RenderTarget& presentTarget, int width, int height);
            ENGINE_API void CreateSwapchainTargetView();

            ENGINE_API void CreateRenderTarget(RenderTarget& renderTarget);
            ENGINE_API void BindRenderTarget(const vector<void*>& colorViews, void* depthView);
            ENGINE_API void BindRenderTarget(const vector<void*>& colorViews, void* depthView, const vector<Viewport>& viewports);
            ENGINE_API void UnbindRenderTarget();
            ENGINE_API void ClearColorTarget(void* colorView, const glm::vec4& color);
            ENGINE_API void ClearDepthTarget(void* depthView, const f32 depth);
            ENGINE_API void ClearStencilTarget(void* depthView, const u8 stencil);
            ENGINE_API void ClearDepthStencilTarget(void* depthView, const f32 depth, const u8 stencil);
            ENGINE_API void FreeRenderTarget(RenderTarget& renderTarget);
            ENGINE_API void FreeRenderTargetColors(vector<Texture*>& colors);
            ENGINE_API void FreeRenderTargetColorViews(vector<void*>& colorViews);
            ENGINE_API void FreeRenderTargetDepth(Texture* depth);
            ENGINE_API void FreeRenderTargetDepthView(void** depthView);
            ENGINE_API void ResizeRenderTarget(RenderTarget& renderTarget, int width, int height);

            ENGINE_API void Present();

            ENGINE_API void CreateShader(Shader& shader);
            ENGINE_API void CompileShader(Shader& shader);
            ENGINE_API void BindShader(const Shader& shader);
            ENGINE_API void FreeShader(Shader& shader);

            ENGINE_API void CompileShaderStage(ShaderStage& stage);
            ENGINE_API void CreateShaderStage(ShaderStage& stage);
            ENGINE_API void BindShaderStage(const ShaderStage& stage);
            ENGINE_API void FreeShaderStage(ShaderStage& stage);

            ENGINE_API void FreeShaderResourceView(void* viewInstance);

            ENGINE_API void CreateTexture(Texture& texture);

            ENGINE_API void CreateTexture1D(Texture& texture);
            ENGINE_API void CreateTexture2D(Texture& texture);
            ENGINE_API void CreateTexture2DArray(Texture& texture);
            ENGINE_API void CreateTexture3D(Texture& texture);
            ENGINE_API void CreateTextureCube(Texture& texture);
            ENGINE_API void CreateTextureDepthStencil(Texture& texture);

            ENGINE_API void BindTexture(const Texture& texture);
            ENGINE_API void BindTexture(const Texture& texture, u32 slot);
            ENGINE_API void BindTextureSlot(u32 slot);
            ENGINE_API void UnbindTexture(const Texture& texture);

            ENGINE_API void FreeTexture(Texture& texture);
            ENGINE_API void FreeTexture1D(void* instance);
            ENGINE_API void FreeTexture2D(void* instance);
            ENGINE_API void FreeTexture2DArray(void* instance);
            ENGINE_API void FreeTexture3D(void* instance);
            ENGINE_API void FreeTextureCube(void* instance);
            ENGINE_API void FreeTextureDepthStencil(void* instance);

            ENGINE_API void CopyTexture(const Texture& texture, const void* data, usize dataByteSize, u32 layerIndex = 0);
            ENGINE_API void CopyTextureOffset(const Texture& texture, usize offset, const void* data, usize dataByteSize, u32 layerIndex = 0);
            ENGINE_API void MoveTexture(const Texture& texture, const void* data, usize dataByteSize, u32 layerIndex = 0);
            ENGINE_API void MoveTextureOffset(const Texture& texture, usize offset, const void* data, usize dataByteSize, u32 layerIndex = 0);

            ENGINE_API void GenerateMips(const Texture& texture);

            ENGINE_API void CreateSampler(TextureSampler& sampler);
            ENGINE_API void BindSampler(const TextureSampler& sampler);
            ENGINE_API void UnbindSampler(const TextureSampler& sampler);
            ENGINE_API void FreeSampler(TextureSampler& sampler);

            ENGINE_API void CreateBuffer(Buffer& buffer);
            ENGINE_API void FreeBuffer(Buffer& buffer);
            ENGINE_API void BindVertexBuffer(const Buffer& buffer);
            ENGINE_API void BindIndexBuffer(const Buffer& buffer);
            ENGINE_API void BindVSBuffer(const Buffer& buffer);
            ENGINE_API void UnbindVSBuffer(const Buffer& buffer);
            ENGINE_API void BindPSBuffer(const Buffer& buffer);
            ENGINE_API void UnbindPSBuffer(const Buffer& buffer);

            ENGINE_API void CopyBuffer(const Buffer& buffer, const void* data, usize dataByteSize);
            ENGINE_API void CopyBufferOffset(const Buffer& buffer, usize offset, const void* data, usize dataByteSize);
            ENGINE_API void MoveBuffer(const Buffer& buffer, const void* data, usize dataByteSize);
            ENGINE_API void MoveBufferOffset(const Buffer& buffer, usize offset, const void* data, usize dataByteSize);

            ENGINE_API void* Map(const GPUResource& resource, u32 subresourceIndex, eMapType mapType);
            ENGINE_API void Unmap(const GPUResource& resource);
            ENGINE_API void UpdateSubData(const GPUResource& resource, u32 subresourceIndex, const void* data, u32 rowPitch, u32 depthPitch);

            ENGINE_API void CreateInputLayout(InputLayout& inputLayout);
            ENGINE_API void BindInputLayout(const InputLayout& inputLayout);
            ENGINE_API void FreeInputLayout(InputLayout& inputLayout);

            ENGINE_API void BindPrimitiveTopology(const ePrimitiveTopology& primitiveTopology);

            ENGINE_API void BindViewports(const vector<Viewport>& viewports);
            ENGINE_API void UnbindViewports();

            ENGINE_API void CreatePipeline(Pipeline& pipeline);
            ENGINE_API void BindPipeline(const Pipeline& pipeline);
            ENGINE_API void UnbindPipeline(const Pipeline& pipeline);
            ENGINE_API void FreePipeline(Pipeline& pipeline);

            ENGINE_API void CreateDepthStencilMode(DepthStencilMode& depthStencilMode);
            ENGINE_API void BindDepthStencilMode(void* state);
            ENGINE_API void FreeDepthStencilMode(DepthStencilMode& depthStencilMode);

            ENGINE_API void CreateBlendMode(BlendMode& blendMode);
            ENGINE_API void BindBlendMode(void* state);
            ENGINE_API void FreeBlendMode(BlendMode& blendMode);

            ENGINE_API void CreateRasterizer(Rasterizer& rasterizer);
            ENGINE_API void BindRasterizer(void* state);
            ENGINE_API void FreeRasterizer(Rasterizer& rasterizer);

            ENGINE_API void DrawIndexed(usize indexCount, usize instanceCount = 1, usize vertexOffset = 0, usize indexOffset = 0, usize instanceOffset = 0);
            ENGINE_API void DrawVertexed(usize vertexCount, usize instanceCount = 1, usize vertexOffset = 0, usize instanceOffset = 0);

            ENGINE_API void DrawQuad();

            ENGINE_API void* GetDevice();

        }

    }

}
