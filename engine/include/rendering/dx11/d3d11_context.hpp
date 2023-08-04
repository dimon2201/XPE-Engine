#pragma once

#include <d3d11.h>

#include <rendering/core/debugger.h>
#include <rendering/core/pipeline.h>

namespace xpe {

    namespace render {

        class ENGINE_API D3D11Context : public Context {

        public:
            D3D11Context() {}
            ~D3D11Context() {}

            void Init() override final;
            void Free() override final;

            void CreateSwapchain(int width, int height) override final;
            void FreeSwapchain() override final;
            void ResizeSwapchain(RenderTarget& renderTarget, int width, int height) override final;
            void CreateSwapchainTargetView() override final;

            void CreateRenderTarget(RenderTarget& renderTarget) override final;
            void BindRenderTarget(void* colorTargetView, void* depthTargetView) override final;
            void UnbindRenderTarget() override final;
            void ClearColorTarget(const glm::vec4& color) override final;
            void ClearDepthTarget(const f32 depth) override final;
            void FreeRenderTarget(RenderTarget& renderTarget) override final;
            void ResizeRenderTarget(RenderTarget& renderTarget, int width, int height) override final;

            void Present() override final;

            void CompileShaderStage(ShaderStage& stage) override final;
            void CreateShaderStage(ShaderStage& stage) override final;
            void BindShaderStage(const ShaderStage& stage) override final;
            void FreeShaderStage(ShaderStage& stage) override final;

            void CreateTexture1D(Texture& texture) override final;
            void CreateTexture2D(Texture& texture) override final;
            void CreateTexture2DArray(Texture& texture) override final;
            void CreateTexture3D(Texture& texture) override final;
            void CreateTextureCube(Texture& texture) override final;
            void CreateTextureDepthStencil(Texture& texture) override final;

            void BindTexture(const Texture* texture) override final;
            void BindTextureSlot(u32 slot) override final;
            void FreeTexture(Texture& texture) override final;
            void WriteTexture(const Texture& texture, const void* pixels, usize pixelsSize, u32 index) override final;
            void GenerateMips(const Texture& texture) override final;

            virtual void CreateSampler(TextureSampler& sampler) override final;
            virtual void BindSampler(const TextureSampler* sampler) override final;
            virtual void FreeSampler(const TextureSampler* sampler) override final;

            void CreateBuffer(Buffer& buffer) override final;
            void BindVertexBuffer(const Buffer* buffer) override final;
            void BindIndexBuffer(const Buffer* buffer) override final;
            void BindVSBuffer(const Buffer* buffer) override final;
            void BindPSBuffer(const Buffer* buffer) override final;
            void WriteBuffer(const Buffer& buffer, const void* data, usize dataByteSize) override final;
            void WriteBufferOffset(const Buffer& buffer, usize offset, const void* data, usize dataByteSize) override final;
            void WriteBufferAppend(Buffer& buffer, const void* data, usize dataByteSize) override final;
            void FreeBuffer(const Buffer& buffer) override final;

            void CreateInputLayout(InputLayout& inputLayout) override final;
            void BindInputLayout(const InputLayout* inputLayout) override final;
            void FreeInputLayout(InputLayout& inputLayout) override final;

            void BindPrimitiveTopology(const ePrimitiveTopology &primitiveTopology) override final;

            void BindViewport(Viewport* viewport) override final;

            void CreatePipeline(Pipeline& pipeline) override final;
            void BindPipeline(const Pipeline* pipeline) override final;
            void FreePipeline(Pipeline& pipeline) override final;

            void CreateDepthStencilState(DepthStencilState& state) override final;
            void BindDepthStencilState(const DepthStencilState* state) override final;
            void FreeDepthStencilState(DepthStencilState& state) override final;

            void CreateBlendState(BlendState& state) override final;
            void BindBlendState(const BlendState* state) override final;
            void FreeBlendState(BlendState& state) override final;

            void CreateRasterizer(Rasterizer& rasterizer) override final;
            void BindRasterizer(const Rasterizer* rasterizer) override final;
            void FreeRasterizer(Rasterizer& rasterizer) override final;

            void DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount) override final;
            void DrawBatch(usize vertexOffset, usize vertexCount, usize instanceCount) override final;

            void DrawQuad() override final;

            void* GetDevice() override final;

            void CreateHardwareConfig() override final;

        private:
            D3D11_SUBRESOURCE_DATA* InitTextureData(const Texture& texture, const u32 arraySize, const u32 mipLevels);
            void FreeTextureData(D3D11_SUBRESOURCE_DATA* initialData);
            void UpdateTextureFlags(Texture& texture, u32& bindFlags, u32& miscFlags);

        private:
            ID3D11Device* m_Device = nullptr;
            ID3D11DeviceContext* m_ImmContext = nullptr;
            IDXGISwapChain* m_SwapChain = nullptr;
            IDXGIDevice* m_GIDevice = nullptr;
            IDXGIAdapter* m_GIAdapter = nullptr;
            IDXGIFactory* m_GIFactory = nullptr;
        };

    }

}