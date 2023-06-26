#pragma once

#include <iostream>
#include <d3d11.h>

#include "rendering.hpp"

namespace xpe
{
    namespace core
    {
        class D3D11RenderingContext : public RenderingContext_Interface
        {
            public:
                D3D11RenderingContext() {}
                ~D3D11RenderingContext() {}

                void Init(Window& window) override final;
                void Free() override final;
                
                xRenderTarget CreateRenderTarget(const glm::ivec2& dimensions, const GPUResource* colorTexture, const GPUResource* colorView, const GPUResource* depthTexture, const GPUResource* depthView) override final;
                void BindRenderTarget(const xRenderTarget* renderTarget) override final;
                void ClearRenderTarget(const glm::vec4& color, const f32 depth) override final;
                glm::ivec2 GetSwapChainDimensions() override final;
                void FreeRenderTarget(const xRenderTarget& renderTarget) override final;
                void Present() override final;
                
                void CreateShaderFromString(xShader& shader) override final;
                void BindShader(const xShader* shader) override final;
                void FreeShader(const xShader& shader) override final;
                
                GPUResource CreateTexture(const void* texture, const glm::ivec2& dimensions) override final;
                void BindTexture(const GPUResource* texture, const xShader::eType& shaderType, const u32 slot) override final;
                void FreeTexture(const GPUResource* texture) override final;
                
                virtual GPUResource CreateSampler() override final;
                virtual void BindSampler(const GPUResource* sampler) override final;
                virtual void FreeSampler(const GPUResource* sampler) override final;
                
                xBuffer CreateBuffer(const xBuffer::xType& bufferType, usize byteSize, boolean duplicate) override final;
                void BindBuffer(const xBuffer* buffer) override final;
                void WriteBuffer(const xBuffer& buffer, const void* data, usize dataByteSize) override final;
                void WriteBufferOffset(const xBuffer& buffer, usize offset, const void* data, usize dataByteSize) override final;
                void WriteBufferAppend(xBuffer& buffer, const void* data, usize dataByteSize) override final;
                void FreeBuffer(const xBuffer& buffer) override final;
                
                void CreateInputLayout(xInputLayout& inputLayout) override final;
                void BindInputLayout(const xInputLayout* inputLayout) override final;
                void FreeInputLayout(const xInputLayout& inputLayout) override final;
                
                void BindViewport(const glm::vec4& coords, f32 minDepth, f32 maxDepth) override final;
                
                void CreateRenderPipeline(xPipeline& pipeline) override final;
                void BindRenderPipeline(const xPipeline* pipeline) override final;
                void FreeRenderPipeline(xPipeline& pipeline) override final;

                void CreateDepthStencilState(xDepthStencilState& state) override final;
                void BindDepthStencilState(const xDepthStencilState* state) override final;
                void FreeDepthStencilState(xDepthStencilState& state) override final;

                void DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount) override final;
                void DrawQuad() override final;
                void OutputErrors() override final;

                inline GPUResource* GetSwapChainTexture() { return &_swapChainTexture; }

            private:
                IDXGISwapChain* _swapChain;
                ID3D11Device* _device;
                ID3D11DeviceContext* _immContext;
                GPUResource _swapChainTexture;
                GPUResource _sampler;
                xRenderTarget _rt;
                xShader* _boundShader;
                xRenderTarget* _boundRT;
                xPipeline* _boundPipeline;
        };
    }
}