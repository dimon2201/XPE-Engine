#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        namespace context {

            void* SwapchainTargetView = nullptr;
            void* SwapchainTextureInstance = nullptr;

            void CopyBuffer(const sBuffer& buffer, const void* data, usize dataByteSize)
            {
                if (buffer.Instance == nullptr || buffer.Usage == eBufferUsage::STATIC) return;

                eMapType mapType = eMapType::WRITE;

                if (buffer.Usage == eBufferUsage::DYNAMIC) {
                    mapType = eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(buffer, 0, mapType);
                memcpy(mappedData, data, dataByteSize);
                Unmap(buffer);
            }

            void CopyBufferOffset(const sBuffer& buffer, usize offset, const void* data, usize dataByteSize)
            {
                if (buffer.Instance == nullptr || buffer.Usage == eBufferUsage::STATIC) return;

                eMapType mapType = eMapType::WRITE;

                if (buffer.Usage == eBufferUsage::DYNAMIC) {
                    mapType = eMapType::WRITE_NO_OVERWRITE;
                }

                void* mappedData = Map(buffer, 0, mapType);
                void* dst = (void*)((u8*)mappedData + offset);
                memcpy(dst, data, dataByteSize);
                Unmap(buffer);
            }

            void MoveBuffer(const sBuffer& buffer, const void* data, usize dataByteSize)
            {
                if (buffer.Instance == nullptr || buffer.Usage == eBufferUsage::STATIC) return;

                eMapType mapType = eMapType::WRITE;

                if (buffer.Usage == eBufferUsage::DYNAMIC) {
                    mapType = eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(buffer, 0, mapType);
                memmove(mappedData, data, dataByteSize);
                Unmap(buffer);
            }

            void MoveBufferOffset(const sBuffer& buffer, usize offset, const void* data, usize dataByteSize)
            {
                if (buffer.Instance == nullptr || buffer.Usage == eBufferUsage::STATIC) return;

                eMapType mapType = eMapType::WRITE;

                if (buffer.Usage == eBufferUsage::DYNAMIC) {
                    mapType = eMapType::WRITE_NO_OVERWRITE;
                }

                void* mappedData = Map(buffer, 0, mapType);
                void* dst = (void*)((u8*)mappedData + offset);
                memmove(dst, data, dataByteSize);
                Unmap(buffer);
            }

            void CopyTexture(const sTexture& texture, const void* data, usize dataByteSize, u32 layerIndex)
            {
                if (texture.Instance == nullptr || texture.Usage == sTexture::eUsage::STATIC) return;

                eMapType mapType = eMapType::WRITE;

                if (texture.Usage == sTexture::eUsage::DYNAMIC) {
                    mapType = eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(texture, layerIndex, mapType);
                if (mappedData == nullptr) {
                    u32 rowPitch = dataByteSize / texture.Height;
                    u32 depthPitch = dataByteSize;
                    UpdateSubData(texture, layerIndex, data, rowPitch, depthPitch);
                }
                else {
                    memcpy(mappedData, data, dataByteSize);
                    Unmap(texture);
                }
            }

            void CopyTextureOffset(const sTexture& texture, usize offset, const void* data, usize dataByteSize, u32 layerIndex)
            {
                if (texture.Instance == nullptr || texture.Usage == sTexture::eUsage::STATIC) return;

                eMapType mapType = eMapType::WRITE;

                if (texture.Usage == sTexture::eUsage::DYNAMIC) {
                    mapType = eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(texture, layerIndex, mapType);
                if (mappedData == nullptr) {
                    u32 rowPitch = dataByteSize / texture.Height;
                    u32 depthPitch = dataByteSize;
                    UpdateSubData(texture, layerIndex, data, rowPitch, depthPitch);
                }
                else {
                    void* dst = (void*)((u8*)mappedData + offset);
                    memcpy(dst, data, dataByteSize);
                    Unmap(texture);
                }
            }

            void MoveTexture(const sTexture& texture, const void* data, usize dataByteSize, u32 layerIndex)
            {
                if (texture.Instance == nullptr || texture.Usage == sTexture::eUsage::STATIC) return;

                eMapType mapType = eMapType::WRITE;

                if (texture.Usage == sTexture::eUsage::DYNAMIC) {
                    mapType = eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(texture, layerIndex, mapType);
                if (mappedData == nullptr) {
                    u32 rowPitch = dataByteSize / texture.Height;
                    u32 depthPitch = dataByteSize;
                    UpdateSubData(texture, layerIndex, data, rowPitch, depthPitch);
                }
                else {
                    memmove(mappedData, data, dataByteSize);
                    Unmap(texture);
                }
            }

            void MoveTextureOffset(const sTexture& texture, usize offset, const void* data, usize dataByteSize, u32 layerIndex)
            {
                if (texture.Instance == nullptr || texture.Usage == sTexture::eUsage::STATIC) return;

                eMapType mapType = eMapType::WRITE;

                if (texture.Usage == sTexture::eUsage::DYNAMIC) {
                    mapType = eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(texture, layerIndex, mapType);
                if (mappedData == nullptr) {
                    u32 rowPitch = dataByteSize / texture.Height;
                    u32 depthPitch = dataByteSize;
                    UpdateSubData(texture, layerIndex, data, rowPitch, depthPitch);
                }
                else {
                    void* dst = (void*)((u8*)mappedData + offset);
                    memmove(dst, data, dataByteSize);
                    Unmap(texture);
                }
            }

            void CreateShader(sShader& shader)
            {
                for (auto* stage : shader.Stages)
                {
                    CreateShaderStage(*stage);
                }
            }

            void CompileShader(sShader& shader)
            {
                for (auto* stage : shader.Stages)
                {
                    LogInfo("Compiling shader stage {}", stage->Name);
                    CompileShaderStage(*stage);
                }
            }

            void BindShader(const sShader& shader)
            {
                for (const auto* stage : shader.Stages)
                {
                    if (stage != nullptr) {
                        BindShaderStage(*stage);
                    }
                }
            }

            void FreeShader(sShader& shader)
            {
                for (auto* stage : shader.Stages)
                {
                    FreeShaderStage(*stage);
                }
                shader.Stages.clear();
            }

            void CreateTexture(sTexture& texture)
            {

                switch (texture.Type) {

                    case sTexture::eType::TEXTURE_1D:
                        CreateTexture1D(texture);
                        break;

                    case sTexture::eType::TEXTURE_2D:
                        CreateTexture2D(texture);
                        break;

                    case sTexture::eType::TEXTURE_2D_DEPTH_STENCIL:
                        CreateTextureDepthStencil(texture);
                        break;

                    case sTexture::eType::TEXTURE_2D_ARRAY:
                        CreateTexture2DArray(texture);
                        break;

                    case sTexture::eType::TEXTURE_3D:
                        CreateTexture3D(texture);
                        break;

                    case sTexture::eType::TEXTURE_CUBE:
                        CreateTextureCube(texture);
                        break;

                }

            }

            void FreeTexture(sTexture& texture)
            {
                if (texture.Instance != nullptr) {

                    switch (texture.Type) {

                        case sTexture::eType::TEXTURE_1D:
                            FreeTexture1D(texture.Instance);
                            break;

                        case sTexture::eType::TEXTURE_2D:
                            FreeTexture2D(texture.Instance);
                            break;

                        case sTexture::eType::TEXTURE_2D_ARRAY:
                            FreeTexture2DArray(texture.Instance);
                            break;

                        case sTexture::eType::TEXTURE_2D_DEPTH_STENCIL:
                            FreeTextureDepthStencil(texture.Instance);
                            break;

                        case sTexture::eType::TEXTURE_3D:
                            FreeTexture3D(texture.Instance);
                            break;

                        case sTexture::eType::TEXTURE_CUBE:
                            FreeTextureCube(texture.Instance);
                            break;

                    }

                    texture.Instance = nullptr;
                }

                if (texture.ViewInstance != nullptr) {
                    FreeShaderResourceView(texture.ViewInstance);
                    texture.ViewInstance = nullptr;
                }
            }

            void CreatePipeline(sPipeline& pipeline)
            {
                if (pipeline.Shader == nullptr)
                {
                    LogError("Failed to create input layout. sShader does not exist.");
                    assert(false);
                    return;
                }

                sBlob* vertexBlob = nullptr;
                for (auto* stage : pipeline.Shader->Stages)
                {
                    if (stage->Type == eShaderType::VERTEX) {
                        vertexBlob = &stage->Blob;
                    }
                }

                if (vertexBlob == nullptr)
                {
                    LogError("Failed to create input layout. sShader has no sVertex stage.");
                    assert(false);
                    return;
                }

                if (vertexBlob->ByteCode == nullptr || vertexBlob->ByteCodeSize == 0)
                {
                    LogError("Failed to create input layout. sVertex shader bytecode is empty.");
                    assert(false);
                    return;
                }

                pipeline.InputLayout.VertexBlob = vertexBlob;
                CreateInputLayout(pipeline.InputLayout);
                CreateRasterizer(pipeline.Rasterizer);
                CreateDepthStencilMode(pipeline.DepthStencil);
                CreateBlendMode(pipeline.Blending);
            }

            void BindPipeline(const sPipeline& pipeline)
            {
                if (pipeline.Shader != nullptr) {
                    BindShader(*pipeline.Shader);
                }

                if (pipeline.RenderTarget != nullptr) {
                    BindRenderTarget(pipeline.RenderTarget->ColorViews, pipeline.RenderTarget->DepthStencilView);
                }

                BindPrimitiveTopology(pipeline.PrimitiveTopology);

                BindInputLayout(pipeline.InputLayout);

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

                BindRasterizer(pipeline.Rasterizer.Instance);
                BindDepthStencilMode(pipeline.DepthStencil.State);
                BindBlendMode(pipeline.Blending.State);
            }

            void UnbindPipeline(const sPipeline &pipeline)
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

            void FreePipeline(sPipeline& pipeline)
            {
                FreeInputLayout(pipeline.InputLayout);
                FreeRasterizer(pipeline.Rasterizer);
                FreeDepthStencilMode(pipeline.DepthStencil);
                FreeBlendMode(pipeline.Blending);
            }

            void FreeRenderTarget(sRenderTarget& renderTarget)
            {
                FreeRenderTargetColorViews(renderTarget.ColorViews);
                FreeRenderTargetDepthView(&renderTarget.DepthStencilView);
            }

            void BindRenderTarget(const vector<void*> &colorViews, void *depthView, const vector<sViewport>& viewports)
            {
                BindRenderTarget(colorViews, depthView);
                BindViewports(viewports);
            }

        }

    }

}