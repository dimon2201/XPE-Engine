#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        namespace context {

            void* SwapchainTargetView = nullptr;
            Viewport* BoundViewport = nullptr;
            void* BoundColorTargetView = nullptr;
            void* BoundDepthTargetView = nullptr;

            void CopyBuffer(const Buffer& buffer, const void* data, usize dataByteSize)
            {
                if (buffer.Instance == nullptr) return;

                void* mappedData = Map(buffer, 0, eMapType::WRITE_DISCARD);
                memcpy(mappedData, data, dataByteSize);
                Unmap(buffer);
            }

            void CopyBufferOffset(const Buffer& buffer, usize offset, const void* data, usize dataByteSize)
            {
                if (buffer.Instance == nullptr) return;

                void* mappedData = Map(buffer, 0, eMapType::WRITE_NO_OVERWRITE);
                void* dst = (void*)((u8*)mappedData + offset);
                memcpy(dst, data, dataByteSize);
                Unmap(buffer);
            }

            void MoveBuffer(const Buffer& buffer, const void* data, usize dataByteSize)
            {
                if (buffer.Instance == nullptr) return;

                void* mappedData = Map(buffer, 0, eMapType::WRITE_DISCARD);
                memmove(mappedData, data, dataByteSize);
                Unmap(buffer);
            }

            void MoveBufferOffset(const Buffer& buffer, usize offset, const void* data, usize dataByteSize)
            {
                if (buffer.Instance == nullptr) return;

                void* mappedData = Map(buffer, 0, eMapType::WRITE_NO_OVERWRITE);
                void* dst = (void*)((u8*)mappedData + offset);
                memmove(dst, data, dataByteSize);
                Unmap(buffer);
            }

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

            void BindShader(const Shader& shader)
            {
                for (const auto* stage : shader.Stages)
                {
                    if (stage != nullptr) {
                        BindShaderStage(*stage);
                    }
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
                CreateRasterizer(pipeline.Rasterizer);
                CreateDepthStencilMode(pipeline.DepthStencil);
                CreateBlendMode(pipeline.Blending);
            }

            void BindPipeline(const Pipeline& pipeline)
            {
                if (pipeline.Shader != nullptr) {
                    BindShader(*pipeline.Shader);
                }

                if (pipeline.RenderTarget != nullptr) {
                    BindRenderTarget(pipeline.RenderTarget->ColorViews, pipeline.RenderTarget->DepthStencilView);
                }

                BindPrimitiveTopology(pipeline.PrimitiveTopology);

                BindInputLayout(pipeline.InputLayout);

                if (pipeline.VertexBuffer != nullptr) {
                    BindVertexBuffer(*pipeline.VertexBuffer);
                }

                if (pipeline.IndexBuffer != nullptr) {
                    BindIndexBuffer(*pipeline.IndexBuffer);
                }

                for (const auto* buffer : pipeline.VSBuffers) {
                    if (buffer != nullptr) {
                        BindVSBuffer(*buffer);
                    }
                }

                for (const auto* buffer : pipeline.PSBuffers) {
                    if (buffer != nullptr) {
                        BindPSBuffer(*buffer);
                    }
                }

                for (const auto* texture : pipeline.Textures) {
                    if (texture != nullptr) {
                        BindTexture(*texture);
                    }
                }

                for (const auto* sampler : pipeline.Samplers) {
                    if (sampler != nullptr) {
                        BindSampler(*sampler);
                    }
                }

                BindRasterizer(pipeline.Rasterizer.State);
                BindDepthStencilMode(pipeline.DepthStencil.State);
                BindBlendMode(pipeline.Blending.State);
            }

            void UnbindPipeline(const Pipeline &pipeline)
            {
                UnbindRenderTarget();

                for (const auto* buffer : pipeline.VSBuffers) {
                    if (buffer != nullptr) {
                        UnbindVSBuffer(*buffer);
                    }
                }

                for (const auto* buffer : pipeline.PSBuffers) {
                    if (buffer != nullptr) {
                        UnbindPSBuffer(*buffer);
                    }
                }

                for (const auto* texture : pipeline.Textures) {
                    if (texture != nullptr) {
                        UnbindTexture(*texture);
                    }
                }

                for (const auto* sampler : pipeline.Samplers) {
                    if (sampler != nullptr) {
                        UnbindSampler(*sampler);
                    }
                }
            }

            void FreePipeline(Pipeline& pipeline)
            {
                FreeInputLayout(pipeline.InputLayout);
                FreeRasterizer(pipeline.Rasterizer);
                FreeDepthStencilMode(pipeline.DepthStencil);
                FreeBlendMode(pipeline.Blending);
            }

            void FreeRenderTarget(RenderTarget& renderTarget)
            {
                FreeRenderTargetColors(renderTarget.Colors);
                FreeRenderTargetColorViews(renderTarget.ColorViews);
                FreeRenderTargetDepth(&renderTarget.DepthStencil);
                FreeRenderTargetDepthView(&renderTarget.DepthStencilView);
            }

            void BindRenderTarget(const vector<void*> &colorViews, void *depthView, const vector<Viewport> *viewports)
            {
                BindRenderTarget(colorViews, depthView);
                if (viewports != nullptr) {
                    BindViewports(*viewports);
                }
            }

        }

    }

}