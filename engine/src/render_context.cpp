#include <render_context.hpp>

namespace xpe
{
    namespace render
    {
        namespace context
        {
            void* SwapchainTargetView = nullptr;
            void* SwapchainTextureInstance = nullptr;

            void CopyBuffer(const sBuffer& buffer, const void* data, usize dataByteSize)
            {
                if (buffer.Instance == nullptr || buffer.Usage == sBuffer::eUsage::STATIC) return;

                sResource::eMapType mapType = sResource::eMapType::WRITE;

                if (buffer.Usage == sBuffer::eUsage::DYNAMIC)
                {
                    mapType = sResource::eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(buffer, 0, mapType);
                memcpy(mappedData, data, dataByteSize);
                Unmap(buffer);
            }

            void CopyBufferOffset(const sBuffer& buffer, usize offset, const void* data, usize dataByteSize)
            {
                if (buffer.Instance == nullptr || buffer.Usage == sBuffer::eUsage::STATIC) return;

                sResource::eMapType mapType = sResource::eMapType::WRITE;

                if (buffer.Usage == sBuffer::eUsage::DYNAMIC)
                {
                    mapType = sResource::eMapType::WRITE_NO_OVERWRITE;
                }

                void* mappedData = Map(buffer, 0, mapType);
                void* dst = (void*)((u8*)mappedData + offset);
                memcpy(dst, data, dataByteSize);
                Unmap(buffer);
            }

            void MoveBuffer(const sBuffer& buffer, const void* data, usize dataByteSize)
            {
                if (buffer.Instance == nullptr || buffer.Usage == sBuffer::eUsage::STATIC) return;

                sResource::eMapType mapType = sResource::eMapType::WRITE;

                if (buffer.Usage == sBuffer::eUsage::DYNAMIC)
                {
                    mapType = sResource::eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(buffer, 0, mapType);
                memmove(mappedData, data, dataByteSize);
                Unmap(buffer);
            }

            void MoveBufferOffset(const sBuffer& buffer, usize offset, const void* data, usize dataByteSize)
            {
                if (buffer.Instance == nullptr || buffer.Usage == sBuffer::eUsage::STATIC) return;

                sResource::eMapType mapType = sResource::eMapType::WRITE;

                if (buffer.Usage == sBuffer::eUsage::DYNAMIC)
                {
                    mapType = sResource::eMapType::WRITE_NO_OVERWRITE;
                }

                void* mappedData = Map(buffer, 0, mapType);
                void* dst = (void*)((u8*)mappedData + offset);
                memmove(dst, data, dataByteSize);
                Unmap(buffer);
            }

            void CopyTexture(const cTexture& texture, const void* data, usize dataByteSize, u32 layerIndex)
            {
                if (texture.Instance == nullptr || texture.Usage == cTexture::eUsage::STATIC) return;

                sResource::eMapType mapType = sResource::eMapType::WRITE;

                if (texture.Usage == cTexture::eUsage::DYNAMIC)
                {
                    mapType = sResource::eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(texture, layerIndex, mapType);
                if (mappedData == nullptr)
                {
                    u32 rowPitch = dataByteSize / texture.Height;
                    u32 depthPitch = dataByteSize;
                    UpdateSubData(texture, layerIndex, data, rowPitch, depthPitch);
                }
                else
                {
                    memcpy(mappedData, data, dataByteSize);
                    Unmap(texture);
                }
            }

            void CopyTextureOffset(const cTexture& texture, usize offset, const void* data, usize dataByteSize, u32 layerIndex)
            {
                if (texture.Instance == nullptr || texture.Usage == cTexture::eUsage::STATIC) return;

                sResource::eMapType mapType = sResource::eMapType::WRITE;

                if (texture.Usage == cTexture::eUsage::DYNAMIC)
                {
                    mapType = sResource::eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(texture, layerIndex, mapType);
                if (mappedData == nullptr)
                {
                    u32 rowPitch = dataByteSize / texture.Height;
                    u32 depthPitch = dataByteSize;
                    UpdateSubData(texture, layerIndex, data, rowPitch, depthPitch);
                }
                else
                {
                    void* dst = (void*)((u8*)mappedData + offset);
                    memcpy(dst, data, dataByteSize);
                    Unmap(texture);
                }
            }

            void MoveTexture(const cTexture& texture, const void* data, usize dataByteSize, u32 layerIndex)
            {
                if (texture.Instance == nullptr || texture.Usage == cTexture::eUsage::STATIC) return;

                sResource::eMapType mapType = sResource::eMapType::WRITE;

                if (texture.Usage == cTexture::eUsage::DYNAMIC)
                {
                    mapType = sResource::eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(texture, layerIndex, mapType);
                if (mappedData == nullptr)
                {
                    u32 rowPitch = dataByteSize / texture.Height;
                    u32 depthPitch = dataByteSize;
                    UpdateSubData(texture, layerIndex, data, rowPitch, depthPitch);
                }
                else
                {
                    memmove(mappedData, data, dataByteSize);
                    Unmap(texture);
                }
            }

            void MoveTextureOffset(const cTexture& texture, usize offset, const void* data, usize dataByteSize, u32 layerIndex)
            {
                if (texture.Instance == nullptr || texture.Usage == cTexture::eUsage::STATIC) return;

                sResource::eMapType mapType = sResource::eMapType::WRITE;

                if (texture.Usage == cTexture::eUsage::DYNAMIC) {
                    mapType = sResource::eMapType::WRITE_DISCARD;
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

            void CreateTexture(cTexture& texture)
            {

                switch (texture.Type)
                {

                    case cTexture::eType::TEXTURE_1D:
                        CreateTexture1D(texture);
                        break;

                    case cTexture::eType::TEXTURE_2D:
                        CreateTexture2D(texture);
                        break;

                    case cTexture::eType::TEXTURE_2D_DEPTH_STENCIL:
                        CreateTextureDepthStencil(texture);
                        break;

                    case cTexture::eType::TEXTURE_2D_ARRAY:
                        CreateTexture2DArray(texture);
                        break;

                    case cTexture::eType::TEXTURE_3D:
                        CreateTexture3D(texture);
                        break;

                    case cTexture::eType::TEXTURE_CUBE:
                        CreateTextureCube(texture);
                        break;

                }
            }

            void FreeTexture(cTexture& texture)
            {
                if (texture.Instance != nullptr)
                {

                    switch (texture.Type)
                    {

                        case cTexture::eType::TEXTURE_1D:
                            FreeTexture1D(texture.Instance);
                            break;

                        case cTexture::eType::TEXTURE_2D:
                            FreeTexture2D(texture.Instance);
                            break;

                        case cTexture::eType::TEXTURE_2D_ARRAY:
                            FreeTexture2DArray(texture.Instance);
                            break;

                        case cTexture::eType::TEXTURE_2D_DEPTH_STENCIL:
                            FreeTextureDepthStencil(texture.Instance);
                            break;

                        case cTexture::eType::TEXTURE_3D:
                            FreeTexture3D(texture.Instance);
                            break;

                        case cTexture::eType::TEXTURE_CUBE:
                            FreeTextureCube(texture.Instance);
                            break;

                    }

                    texture.Instance = nullptr;
                }

                if (texture.ViewInstance != nullptr)
                {
                    FreeShaderResourceView(texture.ViewInstance);
                    texture.ViewInstance = nullptr;
                }
            }

            void FreeRenderTarget(cRenderTarget& renderTarget)
            {
                FreeRenderTargetColorViews(renderTarget.ColorViews);
                FreeRenderTargetDepthView(&renderTarget.DepthStencilView);
            }

            void BindRenderTarget(const vector<void*> &colorViews, void* depthView, sViewport* viewport)
            {
                BindRenderTarget(colorViews, depthView);
                BindViewport(viewport);
            }

            void VSBindBuffer(const sBuffer &buffer)
            {
                VSBindBuffer(buffer.ViewType, buffer.Type, buffer.Slot, buffer.Instance, buffer.ViewInstance);
            }

            void VSUnbindBuffer(const sBuffer &buffer)
            {
                static void* nullInstance = nullptr;
                VSBindBuffer(buffer.ViewType, buffer.Type, buffer.Slot, nullInstance, nullInstance);
            }

            void PSBindBuffer(const sBuffer &buffer)
            {
                PSBindBuffer(buffer.ViewType, buffer.Type, buffer.Slot, buffer.Instance, buffer.ViewInstance);
            }

            void PSUnbindBuffer(const sBuffer &buffer)
            {
                static void* nullInstance = nullptr;
                PSBindBuffer(buffer.ViewType, buffer.Type, buffer.Slot, nullInstance, nullInstance);
            }

            void GSBindBuffer(const sBuffer &buffer)
            {
                GSBindBuffer(buffer.ViewType, buffer.Type, buffer.Slot, buffer.Instance, buffer.ViewInstance);
            }

            void GSUnbindBuffer(const sBuffer &buffer)
            {
                static void* nullInstance = nullptr;
                GSBindBuffer(buffer.ViewType, buffer.Type, buffer.Slot, nullInstance, nullInstance);
            }

            void CSBindBuffer(const sBuffer &buffer)
            {
                CSBindBuffer(buffer.ViewType, buffer.Type, buffer.Slot, buffer.Instance, buffer.ViewInstance);
            }

            void CSUnbindBuffer(const sBuffer &buffer)
            {
                static void* nullInstance = nullptr;
                CSBindBuffer(buffer.ViewType, buffer.Type, buffer.Slot, nullInstance, nullInstance);
            }

            void VSBindTexture(const cTexture& texture)
            {
                VSBindTexture(texture.ViewType, texture.Slot, texture.ViewInstance);
            }

            void VSBindTexture(const cTexture &texture, u32 slot)
            {
                VSBindTexture(texture.ViewType, slot, texture.ViewInstance);
            }

            void VSBindTextureSlot(u32 slot)
            {
                void* nullInstance = nullptr;
                VSBindTexture(cTexture::eViewType::SRV, slot, nullInstance);
            }

            void VSUnbindTexture(const cTexture& texture)
            {
                static void* nullInstance = nullptr;
                VSBindTexture(texture.ViewType, texture.Slot, nullInstance);
            }

            void PSBindTexture(const cTexture& texture)
            {
                PSBindTexture(texture.ViewType, texture.Slot, texture.ViewInstance);
            }

            void PSBindTexture(const cTexture &texture, u32 slot)
            {
                PSBindTexture(texture.ViewType, slot, texture.ViewInstance);
            }

            void PSBindTextureSlot(u32 slot)
            {
                void* nullInstance = nullptr;
                PSBindTexture(cTexture::eViewType::SRV, slot, nullInstance);
            }

            void PSUnbindTexture(const cTexture& texture)
            {
                static void* nullInstance = nullptr;
                PSBindTexture(texture.ViewType, texture.Slot, nullInstance);
            }

            void GSBindTexture(const cTexture& texture)
            {
                GSBindTexture(texture.ViewType, texture.Slot, texture.ViewInstance);
            }

            void GSBindTexture(const cTexture &texture, u32 slot)
            {
                GSBindTexture(texture.ViewType, slot, texture.ViewInstance);
            }

            void GSBindTextureSlot(u32 slot)
            {
                void* nullInstance = nullptr;
                GSBindTexture(cTexture::eViewType::SRV, slot, nullInstance);
            }

            void GSUnbindTexture(const cTexture& texture)
            {
                static void* nullInstance = nullptr;
                GSBindTexture(texture.ViewType, texture.Slot, nullInstance);
            }

            void CSBindTexture(const cTexture& texture)
            {
                CSBindTexture(texture.ViewType, texture.Slot, texture.ViewInstance);
            }

            void CSBindTexture(const cTexture &texture, u32 slot)
            {
                CSBindTexture(texture.ViewType, slot, texture.ViewInstance);
            }

            void CSBindTextureSlot(u32 slot)
            {
                void* nullInstance = nullptr;
                CSBindTexture(cTexture::eViewType::SRV, slot, nullInstance);
            }

            void CSUnbindTexture(const cTexture& texture)
            {
                static void* nullInstance = nullptr;
                CSBindTexture(texture.ViewType, texture.Slot, nullInstance);
            }

            void VSBindSampler(const cSampler &sampler)
            {
                VSBindSampler(sampler.Slot, sampler.ViewInstance);
            }

            void VSUnbindSampler(const cSampler& sampler)
            {
                static void* nullInstance = nullptr;
                VSBindSampler(sampler.Slot, nullInstance);
            }

            void PSBindSampler(const cSampler &sampler)
            {
                PSBindSampler(sampler.Slot, sampler.ViewInstance);
            }

            void PSUnbindSampler(const cSampler& sampler)
            {
                static void* nullInstance = nullptr;
                PSBindSampler(sampler.Slot, nullInstance);
            }

            void GSBindSampler(const cSampler &sampler)
            {
                GSBindSampler(sampler.Slot, sampler.ViewInstance);
            }

            void GSUnbindSampler(const cSampler& sampler)
            {
                static void* nullInstance = nullptr;
                GSBindSampler(sampler.Slot, nullInstance);
            }

            void CSBindSampler(const cSampler &sampler)
            {
                CSBindSampler(sampler.Slot, sampler.ViewInstance);
            }

            void CSUnbindSampler(const cSampler& sampler)
            {
                static void* nullInstance = nullptr;
                CSBindSampler(sampler.Slot, nullInstance);
            }
        }
    }
}