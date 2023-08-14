#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        namespace context {

            void* SwapchainTargetView = nullptr;
            Viewport* BoundViewport = nullptr;
            void* BoundColorTargetView = nullptr;
            void* BoundDepthTargetView = nullptr;

            void CreateShader(Shader& shader)
            {
                for (auto* stage : shader.Stages)
                {
                    CreateShaderStage(*stage);
                }
            }

            void CompileShader(Shader& shader)
            {
                for (auto* stage : shader.Stages)
                {
                    CompileShaderStage(*stage);
                }
            }

            void BindShader(const Shader* shader)
            {
                for (const auto* stage : shader->Stages)
                {
                    BindShaderStage(*stage);
                }
            }

            void FreeShader(Shader& shader)
            {
                for (auto* stage : shader.Stages)
                {
                    FreeShaderStage(*stage);
                }
                shader.Stages.clear();
            }

            void CreateTexture(Texture& texture)
            {

                switch (texture.Type)
                {

                    case Texture::eType::TEXTURE_1D:
                        CreateTexture1D(texture);
                        break;

                    case Texture::eType::TEXTURE_2D:
                        CreateTexture2D(texture);
                        break;

                    case Texture::eType::TEXTURE_2D_ARRAY:
                        CreateTexture2DArray(texture);
                        break;

                    case Texture::eType::TEXTURE_3D:
                        CreateTexture3D(texture);
                        break;

                    case Texture::eType::TEXTURE_CUBE:
                        CreateTextureCube(texture);
                        break;

                }

            }

            void CreatePipeline(Pipeline& pipeline)
            {
                if (pipeline.Shader == nullptr)
                {
                    LogError("Failed to create render pipeline. Shader does not exist.");
                    assert(false);
                    return;
                }

                Blob* vertexBlob = nullptr;
                for (auto* stage : pipeline.Shader->Stages)
                {
                    if (stage->Type == eShaderType::VERTEX) {
                        vertexBlob = &stage->Blob;
                    }
                }

                if (vertexBlob == nullptr)
                {
                    LogError("Failed to create render pipeline. Shader has no Vertex stage.");
                    assert(false);
                    return;
                }

                pipeline.InputLayout.VertexBlob = vertexBlob;
                CreateInputLayout(pipeline.InputLayout);

                CreateDepthStencilState(pipeline.DepthStencilState);

                CreateBlendState(pipeline.BlendState);

                CreateRasterizer(pipeline.Rasterizer);
            }

            void BindPipeline(Pipeline& pipeline)
            {
                BindPrimitiveTopology(pipeline.PrimitiveTopology);

                BindInputLayout(pipeline.InputLayout);

                if (pipeline.VertexBuffer != nullptr) {
                    BindVertexBuffer(pipeline.VertexBuffer);
                }

                if (pipeline.IndexBuffer != nullptr) {
                    BindIndexBuffer(pipeline.IndexBuffer);
                }

                for (const auto* buffer : pipeline.VSBuffers) {
                    BindVSBuffer(buffer);
                }

                for (const auto* buffer : pipeline.PSBuffers) {
                    BindPSBuffer(buffer);
                }

                for (const auto* texture : pipeline.Textures) {
                    BindTexture(texture);
                }

                for (const auto* sampler : pipeline.Samplers) {
                    BindSampler(sampler);
                }

                if (pipeline.Shader != nullptr) {
                    BindShader(pipeline.Shader);
                }

                if (pipeline.RenderTarget != nullptr) {
                    BindRenderTarget(pipeline.RenderTarget->ColorTargetView, pipeline.RenderTarget->DepthTargetView);
                }

                BindDepthStencilState(&pipeline.DepthStencilState);
                BindBlendState(&pipeline.BlendState);
                BindRasterizer(&pipeline.Rasterizer);
            }

            void FreePipeline(Pipeline& pipeline)
            {
                FreeInputLayout(pipeline.InputLayout);
                FreeDepthStencilState(pipeline.DepthStencilState);
                FreeBlendState(pipeline.BlendState);
                FreeRasterizer(pipeline.Rasterizer);
            }

        }

    }

}