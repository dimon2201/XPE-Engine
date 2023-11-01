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
            ENGINE_API void ResizeSwapchain(sRenderTarget& presentTarget, int width, int height);
            ENGINE_API void CreateSwapchainTargetView();

            ENGINE_API void CreateRenderTarget(sRenderTarget& renderTarget);
            ENGINE_API void BindRenderTarget(const vector<void*>& colorViews, void* depthView);
            ENGINE_API void BindRenderTarget(const vector<void*>& colorViews, void* depthView, sViewport* viewport);
            ENGINE_API void UnbindRenderTarget();
            ENGINE_API void ClearColorTarget(void* colorView, const glm::vec4& color);
            ENGINE_API void ClearDepthTarget(void* depthView, const f32 depth);
            ENGINE_API void ClearStencilTarget(void* depthView, const u8 stencil);
            ENGINE_API void ClearDepthStencilTarget(void* depthView, const f32 depth, const u8 stencil);
            ENGINE_API void FreeRenderTarget(sRenderTarget& renderTarget);
            ENGINE_API void FreeRenderTargetColors(vector<sTexture*>& colors);
            ENGINE_API void FreeRenderTargetColorViews(vector<void*>& colorViews);
            ENGINE_API void FreeRenderTargetDepth(sTexture* depth);
            ENGINE_API void FreeRenderTargetDepthView(void** depthView);
            ENGINE_API void ResizeRenderTarget(sRenderTarget& renderTarget, int width, int height);

            ENGINE_API void Present();

            ENGINE_API void CreateShader(sShader& shader);
            ENGINE_API void CompileShader(sShader& shader);
            ENGINE_API void BindShader(const sShader& shader);
            ENGINE_API void FreeShader(sShader& shader);

            ENGINE_API void CompileShaderStage(sShaderStage& stage);
            ENGINE_API void CreateShaderStage(sShaderStage& stage);
            ENGINE_API void BindShaderStage(const sShaderStage& stage);
            ENGINE_API void FreeShaderStage(sShaderStage& stage);

            ENGINE_API void FreeShaderResourceView(void* viewInstance);

            ENGINE_API void CreateTexture(sTexture& texture);

            ENGINE_API void CreateTexture1D(sTexture& texture);
            ENGINE_API void CreateTexture2D(sTexture& texture);
            ENGINE_API void CreateTexture2DArray(sTexture& texture);
            ENGINE_API void CreateTexture3D(sTexture& texture);
            ENGINE_API void CreateTextureCube(sTexture& texture);
            ENGINE_API void CreateTextureDepthStencil(sTexture& texture);

            ENGINE_API void BindTexture(const sTexture& texture);
            ENGINE_API void BindTexture(const sTexture& texture, u32 slot);
            ENGINE_API void BindTextureSlot(u32 slot);
            ENGINE_API void UnbindTexture(const sTexture& texture);

            ENGINE_API void FreeTexture(sTexture& texture);
            ENGINE_API void FreeTexture1D(void* instance);
            ENGINE_API void FreeTexture2D(void* instance);
            ENGINE_API void FreeTexture2DArray(void* instance);
            ENGINE_API void FreeTexture3D(void* instance);
            ENGINE_API void FreeTextureCube(void* instance);
            ENGINE_API void FreeTextureDepthStencil(void* instance);

            ENGINE_API void CopyTexture(const sTexture& texture, const void* data, usize dataByteSize, u32 layerIndex = 0);
            ENGINE_API void CopyTextureOffset(const sTexture& texture, usize offset, const void* data, usize dataByteSize, u32 layerIndex = 0);
            ENGINE_API void MoveTexture(const sTexture& texture, const void* data, usize dataByteSize, u32 layerIndex = 0);
            ENGINE_API void MoveTextureOffset(const sTexture& texture, usize offset, const void* data, usize dataByteSize, u32 layerIndex = 0);

            ENGINE_API void GenerateMips(const sTexture& texture);

            ENGINE_API void CreateSampler(sSampler& sampler);
            ENGINE_API void BindSampler(const sSampler& sampler);
            ENGINE_API void UnbindSampler(const sSampler& sampler);
            ENGINE_API void FreeSampler(sSampler& sampler);

            ENGINE_API void CreateBuffer(sBuffer& buffer);
            ENGINE_API void FreeBuffer(sBuffer& buffer);
            ENGINE_API void BindVertexBuffer(const sBuffer& buffer);
            ENGINE_API void BindIndexBuffer(const sBuffer& buffer);
            ENGINE_API void BindVSBuffer(const sBuffer& buffer);
            ENGINE_API void UnbindVSBuffer(const sBuffer& buffer);
            ENGINE_API void BindPSBuffer(const sBuffer& buffer);
            ENGINE_API void UnbindPSBuffer(const sBuffer& buffer);

            ENGINE_API void CopyBuffer(const sBuffer& buffer, const void* data, usize dataByteSize);
            ENGINE_API void CopyBufferOffset(const sBuffer& buffer, usize offset, const void* data, usize dataByteSize);
            ENGINE_API void MoveBuffer(const sBuffer& buffer, const void* data, usize dataByteSize);
            ENGINE_API void MoveBufferOffset(const sBuffer& buffer, usize offset, const void* data, usize dataByteSize);

            ENGINE_API void* Map(const sResource& resource, u32 subresourceIndex, eMapType mapType);
            ENGINE_API void Unmap(const sResource& resource);
            ENGINE_API void UpdateSubData(const sResource& resource, u32 subresourceIndex, const void* data, u32 rowPitch, u32 depthPitch);

            ENGINE_API void CreateInputLayout(sInputLayout& inputLayout);
            ENGINE_API void BindInputLayout(const sInputLayout& inputLayout);
            ENGINE_API void FreeInputLayout(sInputLayout& inputLayout);

            ENGINE_API void BindPrimitiveTopology(const ePrimitiveTopology& primitiveTopology);

            ENGINE_API void BindViewport(sViewport* viewport);
            ENGINE_API void UnbindViewport();

            ENGINE_API void CreatePipeline(sPipeline& pipeline);
            ENGINE_API void BindPipeline(const sPipeline& pipeline);
            ENGINE_API void UnbindPipeline(const sPipeline& pipeline);
            ENGINE_API void FreePipeline(sPipeline& pipeline);

            ENGINE_API void CreateDepthStencilMode(sDepthStencilMode& depthStencilMode);
            ENGINE_API void BindDepthStencilMode(void* state);
            ENGINE_API void FreeDepthStencilMode(sDepthStencilMode& depthStencilMode);

            ENGINE_API void CreateBlendMode(sBlendMode& blendMode);
            ENGINE_API void BindBlendMode(void* state);
            ENGINE_API void FreeBlendMode(sBlendMode& blendMode);

            ENGINE_API void CreateRasterizer(sRasterizer& rasterizer);
            ENGINE_API void BindRasterizer(void* state);
            ENGINE_API void FreeRasterizer(sRasterizer& rasterizer);

            ENGINE_API void DrawIndexed(usize indexCount, usize instanceCount = 1, usize vertexOffset = 0, usize indexOffset = 0, usize instanceOffset = 0);
            ENGINE_API void DrawVertexed(usize vertexCount, usize instanceCount = 1, usize vertexOffset = 0, usize instanceOffset = 0);

            ENGINE_API void DrawQuad();

            ENGINE_API void* GetDevice();

        }

    }

}
