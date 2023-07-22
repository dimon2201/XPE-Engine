#pragma once

#include <core/windowing.hpp>

#include <rendering/core.h>

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

        class ENGINE_API Context {

            public:
                Context() {}
                ~Context() {}

                inline void* GetSwapchainTargetView() const {
                    return m_SwapchainTargetView;
                }

                virtual void Init() = 0;
                virtual void Free() = 0;

                virtual void CreateSwapchain(int width, int height) = 0;
                virtual void FreeSwapchain() = 0;
                virtual void ResizeSwapchain(RenderTarget& presentTarget, int width, int height) = 0;
                virtual void CreateSwapchainTargetView() = 0;

                virtual void CreateRenderTarget(RenderTarget& renderTarget) = 0;
                virtual void BindRenderTarget(void* colorTargetView, void* depthTargetView) = 0;
                virtual void UnbindRenderTarget() = 0;
                virtual void ClearColorTarget(const glm::vec4& color) = 0;
                virtual void ClearDepthTarget(const f32 depth) = 0;
                virtual void FreeRenderTarget(RenderTarget& renderTarget) = 0;
                virtual void ResizeRenderTarget(RenderTarget& renderTarget, int width, int height) = 0;

                virtual void Present() = 0;

                void CreateShader(Shader& shader);
                void CompileShader(Shader& shader);
                void BindShader(const Shader* shader);
                void FreeShader(Shader& shader);

                virtual void CompileShaderStage(ShaderStage& stage) = 0;
                virtual void CreateShaderStage(ShaderStage& stage) = 0;
                virtual void BindShaderStage(const ShaderStage& stage) = 0;
                virtual void FreeShaderStage(ShaderStage& stage) = 0;
                
                void CreateTexture(Texture& texture);

                virtual void CreateTexture1D(Texture& texture) = 0;
                virtual void CreateTexture2D(Texture& texture) = 0;
                virtual void CreateTexture2DArray(Texture& texture) = 0;
                virtual void CreateTexture3D(Texture& texture) = 0;
                virtual void CreateTextureCube(Texture& texture) = 0;
                virtual void CreateTextureDepthStencil(Texture& texture) = 0;

                virtual void BindTexture(const Texture* texture) = 0;
                virtual void BindTextureSlot(u32 slot) = 0;
                virtual void FreeTexture(Texture& texture) = 0;
                virtual void WriteTexture(const Texture& texture, const void* pixels, usize pixelsSize, u32 index = 0) = 0;

                virtual void GenerateMips(const Texture& texture) = 0;

                virtual void CreateSampler(TextureSampler& sampler) = 0;
                virtual void BindSampler(const TextureSampler* sampler) = 0;
                virtual void FreeSampler(const TextureSampler* sampler) = 0;
                
                virtual void CreateBuffer(Buffer& buffer) = 0;
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

                virtual void BindPrimitiveTopology(const ePrimitiveTopology& primitiveTopology) = 0;
                
                virtual void BindViewport(Viewport* viewport) = 0;

                virtual void CreatePipeline(Pipeline& pipeline) = 0;
                virtual void BindPipeline(const Pipeline* pipeline) = 0;
                virtual void FreePipeline(Pipeline& pipeline) = 0;

                virtual void CreateDepthStencilState(DepthStencilState& state) = 0;
                virtual void BindDepthStencilState(const DepthStencilState* state) = 0;
                virtual void FreeDepthStencilState(DepthStencilState& state) = 0;

                virtual void DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount) = 0;
                virtual void DrawBatch(usize vertexOffset, usize vertexCount, usize instanceCount) = 0;

                virtual void DrawQuad() = 0;

                virtual void* GetDevice() = 0;

                virtual void CreateHardwareConfig() = 0;

        protected:
            void* m_SwapchainTargetView = nullptr;
            Viewport* m_Viewport = nullptr;
            void* m_BoundColorTargetView = nullptr;
            void* m_BoundDepthTargetView = nullptr;
            Shader* m_BoundShader = nullptr;
            Pipeline* m_BoundPipeline = nullptr;
        };

    }

}
