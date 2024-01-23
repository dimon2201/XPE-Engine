#pragma once

#include <rendering/core/render_target.hpp>

namespace xpe {

    namespace render {

        class sShaderStage;

        namespace context {

            extern void* SwapchainTargetView;
            extern void* SwapchainTextureInstance;

            ENGINE_API void Init();
            ENGINE_API void Free();

            ENGINE_API void CreateSwapchain(int width, int height);
            ENGINE_API void FreeSwapchain();
            ENGINE_API void ResizeSwapchain(cRenderTarget& presentTarget, int width, int height);
            ENGINE_API void CreateSwapchainTargetView();

            ENGINE_API void CreateRenderTarget(cRenderTarget& renderTarget);
            ENGINE_API void BindRenderTarget(const vector<void*>& colorViews, void* depthView);
            ENGINE_API void BindRenderTarget(const vector<void*>& colorViews, void* depthView, sViewport* viewport);
            ENGINE_API void UnbindRenderTarget();
            ENGINE_API void ClearColorTarget(void* colorView, const glm::vec4& color);
            ENGINE_API void ClearDepthTarget(void* depthView, const f32 depth);
            ENGINE_API void ClearStencilTarget(void* depthView, const u8 stencil);
            ENGINE_API void ClearDepthStencilTarget(void* depthView, const f32 depth, const u8 stencil);
            ENGINE_API void FreeRenderTarget(cRenderTarget& renderTarget);
            ENGINE_API void FreeRenderTargetColors(vector<cTexture*>& colors);
            ENGINE_API void FreeRenderTargetColorViews(vector<void*>& colorViews);
            ENGINE_API void FreeRenderTargetDepth(cTexture* depth);
            ENGINE_API void FreeRenderTargetDepthView(void** depthView);
            ENGINE_API void ResizeRenderTarget(cRenderTarget& renderTarget, int width, int height);

            ENGINE_API void Present();

            ENGINE_API void CompileShaderStage(sShaderStage* stage);
            ENGINE_API void CreateShaderStage(sShaderStage* stage);
            ENGINE_API void FreeShaderStage(sShaderStage* stage);

            ENGINE_API void BindVSStage(const sShaderStage* stage);
            ENGINE_API void BindPSStage(const sShaderStage* stage);
            ENGINE_API void BindGSStage(const sShaderStage* stage);
            ENGINE_API void BindCSStage(const sShaderStage* stage);

            ENGINE_API void UnbindVSStage(const sShaderStage* stage);
            ENGINE_API void UnbindPSStage(const sShaderStage* stage);
            ENGINE_API void UnbindGSStage(const sShaderStage* stage);
            ENGINE_API void UnbindCSStage(const sShaderStage* stage);

            ENGINE_API void FreeShaderResourceView(void* viewInstance);
            ENGINE_API void FreeUnorderedAccessView(void* viewInstance);

            ENGINE_API void CreateTexture(cTexture& texture);

            ENGINE_API void CreateTexture1D(cTexture& texture);
            ENGINE_API void CreateTexture2D(cTexture& texture);
            ENGINE_API void CreateTexture2DArray(cTexture& texture);
            ENGINE_API void CreateTexture3D(cTexture& texture);
            ENGINE_API void CreateTextureCube(cTexture& texture);
            ENGINE_API void CreateTextureDepthStencil(cTexture& texture);

            ENGINE_API void VSBindTexture(cTexture::eViewType viewType, u32 slot, void* viewInstance);
            ENGINE_API void VSBindTexture(const cTexture& texture);
            ENGINE_API void VSBindTexture(const cTexture& texture, u32 slot);
            ENGINE_API void VSBindTextureSlot(u32 slot);
            ENGINE_API void VSUnbindTexture(const cTexture& texture);

            ENGINE_API void PSBindTexture(const cResource::eViewType& viewType, u32 slot, void* instance, void* viewInstance);
            ENGINE_API void PSUnbindTexture(const cTexture& texture);

            ENGINE_API void GSBindTexture(cTexture::eViewType viewType, u32 slot, void* viewInstance);
            ENGINE_API void GSBindTexture(const cTexture& texture);
            ENGINE_API void GSBindTexture(const cTexture& texture, u32 slot);
            ENGINE_API void GSBindTextureSlot(u32 slot);
            ENGINE_API void GSUnbindTexture(const cTexture& texture);

            ENGINE_API void CSBindTexture(cTexture::eViewType viewType, u32 slot, void* viewInstance);
            ENGINE_API void CSBindTexture(const cTexture& texture);
            ENGINE_API void CSBindTexture(const cTexture& texture, u32 slot);
            ENGINE_API void CSBindTextureSlot(u32 slot);
            ENGINE_API void CSUnbindTexture(const cTexture& texture);

            ENGINE_API void FreeTexture(cTexture& texture);
            ENGINE_API void FreeTexture1D(void* instance);
            ENGINE_API void FreeTexture2D(void* instance);
            ENGINE_API void FreeTexture2DArray(void* instance);
            ENGINE_API void FreeTexture3D(void* instance);
            ENGINE_API void FreeTextureCube(void* instance);
            ENGINE_API void FreeTextureDepthStencil(void* instance);

            ENGINE_API void CopyTexture(const cTexture& texture, const void* data, usize dataByteSize, u32 layerIndex = 0);
            ENGINE_API void CopyTextureOffset(const cTexture& texture, usize offset, const void* data, usize dataByteSize, u32 layerIndex = 0);
            ENGINE_API void MoveTexture(const cTexture& texture, const void* data, usize dataByteSize, u32 layerIndex = 0);
            ENGINE_API void MoveTextureOffset(const cTexture& texture, usize offset, const void* data, usize dataByteSize, u32 layerIndex = 0);
            ENGINE_API void WriteTextureOffset(const cTexture& texture, const glm::vec2& offset, const glm::vec2& size, const void* data, usize dataByteSize);

            ENGINE_API void GenerateMips(const cTexture& texture);

            ENGINE_API void CreateSampler(sSampler& sampler);
            ENGINE_API void FreeSampler(sSampler& sampler);

            ENGINE_API void VSBindSampler(const sSampler& sampler);
            ENGINE_API void VSBindSampler(u32 slot, void* viewInstance);
            ENGINE_API void VSUnbindSampler(const sSampler& sampler);

            ENGINE_API void PSBindSampler(const sSampler& sampler);
            ENGINE_API void PSBindSampler(u32 slot, void* viewInstance);
            ENGINE_API void PSUnbindSampler(const sSampler& sampler);

            ENGINE_API void GSBindSampler(const sSampler& sampler);
            ENGINE_API void GSBindSampler(u32 slot, void* viewInstance);
            ENGINE_API void GSUnbindSampler(const sSampler& sampler);

            ENGINE_API void CSBindSampler(const sSampler& sampler);
            ENGINE_API void CSBindSampler(u32 slot, void* viewInstance);
            ENGINE_API void CSUnbindSampler(const sSampler& sampler);

            ENGINE_API void CreateBuffer(cBuffer& buffer);
            ENGINE_API void FreeBuffer(cBuffer& buffer);

            ENGINE_API void BindVertexBuffer(cBuffer& buffer);
            ENGINE_API void BindIndexBuffer(cBuffer& buffer);

            ENGINE_API void VSBindBuffer(const cResource::eViewType& viewType, const cBuffer::eType& type, u32 slot, void* instance, void* viewInstance);
            ENGINE_API void VSUnbindBuffer(cBuffer& buffer);

            ENGINE_API void PSBindBuffer(const cResource::eViewType& viewType, const cBuffer::eType& type, u32 slot, void* instance, void* viewInstance);
            ENGINE_API void PSUnbindBuffer(cBuffer& buffer);

            ENGINE_API void GSBindBuffer(const cResource::eViewType& viewType, const cBuffer::eType& type, u32 slot, void* instance, void* viewInstance);
            ENGINE_API void GSBindBuffer(const cResource::eViewType& viewType, cBuffer& buffer);
            ENGINE_API void GSUnbindBuffer(cBuffer& buffer);

            ENGINE_API void CSBindBuffer(const cResource::eViewType& viewType, const cBuffer::eType& type, u32 slot, void* instance, void* viewInstance);
            ENGINE_API void CSBindBuffer(const cResource::eViewType& viewType, cBuffer& buffer);
            ENGINE_API void CSUnbindBuffer(cBuffer& buffer);

            ENGINE_API void WriteBuffer(cBuffer& buffer, const void* data, usize dataByteSize);
            ENGINE_API void CopyBufferOffset(cBuffer& buffer, usize offset, const void* data, usize dataByteSize);
            ENGINE_API void MoveBuffer(cBuffer& buffer, const void* data, usize dataByteSize);
            ENGINE_API void MoveBufferOffset(cBuffer& buffer, usize offset, const void* data, usize dataByteSize);

            ENGINE_API void* Map(const cResource& resource, u32 subresourceIndex, cResource::eMapType mapType);
            ENGINE_API void Unmap(const cResource& resource);
            ENGINE_API void UpdateSubData(const cResource& resource, u32 subresourceIndex, const void* data, u32 rowPitch, u32 depthPitch);

            ENGINE_API void CreateInputLayout(sInputLayout& inputLayout);
            ENGINE_API void BindInputLayout(const sInputLayout& inputLayout);
            ENGINE_API void FreeInputLayout(sInputLayout& inputLayout);

            ENGINE_API void BindPrimitiveTopology(const ePrimitiveTopology& primitiveTopology);

            ENGINE_API void BindViewport(sViewport* viewport);
            ENGINE_API void UnbindViewport();

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
            ENGINE_API void DrawQuads(usize count);

            ENGINE_API void Dispatch(const glm::ivec3& threadGroupCount);

            ENGINE_API void* GetDevice();

        }

    }

}
