#pragma once

#include <d3d11.h>

#include <rendering/debugger.h>
#include <rendering/pipeline.h>

namespace xpe {

    namespace render {

        class ENGINE_API D3D11Context : public Context {

        public:
            D3D11Context() {}
            ~D3D11Context() {}

            void Init() override final;
            void Free() override final;

            void CreateRenderTarget(RenderTarget& renderTarget) override final;
            void BindRenderTarget(void* colorTargetView, void* depthTargetView) override final;
            void BindSwapChainTarget() override final;
            void ClearColorTarget(const glm::vec4& color) override final;
            void ClearDepthTarget(const f32 depth) override final;
            glm::ivec2 GetSwapChainDimensions() override final;
            void FreeRenderTarget(const RenderTarget& renderTarget) override final;
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
            void FreeTexture(const Texture* texture) override final;
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
            void FreeInputLayout(const InputLayout& inputLayout) override final;

            void BindPrimitiveTopology(const ePrimitiveTopology &primitiveTopology) override final;

            void BindViewport(const glm::vec4& coords, f32 minDepth, f32 maxDepth) override final;

            void CreateRenderPipeline(Pipeline& pipeline) override final;
            void BindRenderPipeline(const Pipeline* pipeline) override final;
            void FreeRenderPipeline(Pipeline& pipeline) override final;

            void CreateDepthStencilState(DepthStencilState& state) override final;
            void BindDepthStencilState(const DepthStencilState* state) override final;
            void FreeDepthStencilState(DepthStencilState& state) override final;

            void DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount) override final;
            void DrawBatch(usize vertexOffset, usize vertexCount, usize instanceCount) override final;

            void DrawQuad(const ePrimitiveTopology& primitiveTopology) override final;

            void* GetDevice() override final;

            void InitHardwareConfig() override final;

        private:
            D3D11_SUBRESOURCE_DATA* InitTextureData(const Texture& texture, const u32 arraySize, const u32 mipLevels);
            void FreeTextureData(D3D11_SUBRESOURCE_DATA* initialData);
            void UpdateTextureFlags(Texture& texture, u32& bindFlags, u32& miscFlags);

        private:
            ID3D11Device* m_Device;
            ID3D11DeviceContext* m_ImmContext;
            IDXGISwapChain* m_SwapChain;
            Texture m_SwapChainTexture;
            TextureSampler m_SwapChainSampler;
            RenderTarget m_SwapChainTarget;
        };

    }

}