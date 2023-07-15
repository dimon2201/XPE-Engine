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
                
                RenderTarget CreateRenderTarget(const glm::ivec2& dimensions, const GPUResource* colorTexture, const GPUResource* colorView, const GPUResource* depthTexture, const GPUResource* depthView) override final;
                void BindRenderTarget(const RenderTarget* renderTarget) override final;
                void ClearRenderTarget(const glm::vec4& color, const f32 depth) override final;
                glm::ivec2 GetSwapChainDimensions() override final;
                void FreeRenderTarget(const RenderTarget& renderTarget) override final;
                void Present() override final;
                
                void CreateShader(Shader& shader) override final;
                void BindShader(const Shader* shader) override final;
                void FreeShader(Shader& shader) override final;

                void CreateShaderStage(ShaderStage &stage) override;
                void BindShaderStage(const ShaderStage &stage) override;
                void FreeShaderStage(ShaderStage &stage) override;

                void CreateTexture(Texture& texture, const void* instance) override final;
                void CreateTexture1D(Texture& texture) override final;
                void CreateTexture2D(Texture& texture) override final;
                void CreateTexture2DArray(Texture& texture) override final;
                void CreateTexture3D(Texture& texture) override final;
                void CreateTextureCube(Texture& texture) override final;
                void BindTexture(const Texture* texture) override final;
                void BindTextureSlot(u32 slot) override final;
                void FreeTexture(const Texture* texture) override final;
                void WriteTexture(const Texture& texture, const void* pixels, usize pixelsSize, u32 index) override final;

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
                
                void BindViewport(const glm::vec4& coords, f32 minDepth, f32 maxDepth) override final;
                
                void CreateRenderPipeline(Pipeline& pipeline) override final;
                void BindRenderPipeline(const Pipeline* pipeline) override final;
                void FreeRenderPipeline(Pipeline& pipeline) override final;

                void CreateDepthStencilState(DepthStencilState& state) override final;
                void BindDepthStencilState(const DepthStencilState* state) override final;
                void FreeDepthStencilState(DepthStencilState& state) override final;

                void DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount) override final;
                void DrawBatch(usize vertexOffset, usize vertexCount, usize instanceCount) override final;
                void DrawQuad() override final;

                void* GetDevice() override final;

                inline GPUResource* GetSwapChainTexture() { return &_swapChainTexture; }

            private:
                IDXGISwapChain* _swapChain;
                ID3D11Device* _device;
                ID3D11DeviceContext* _immContext;
                Texture _swapChainTexture;
                TextureSampler _sampler;
                RenderTarget _rt;
                Shader* _boundShader;
                RenderTarget* _boundRT;
                Pipeline* _boundPipeline;
        };

    }

}