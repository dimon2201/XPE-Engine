#include <rendering/core/core.hpp>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        namespace context {

            void* SwapchainTargetView = nullptr;
            void* SwapchainTextureInstance = nullptr;

            void WriteBuffer(cBuffer& buffer, const void* data, usize dataByteSize)
            {
                if (buffer.GetInstance() == nullptr || buffer.GetUsage() == cBuffer::eUsage::STATIC) return;

                cResource::eMapType mapType = cResource::eMapType::WRITE;

                if (buffer.GetUsage() == cBuffer::eUsage::DYNAMIC) {
                    mapType = cResource::eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(buffer, 0, mapType);
                memcpy(mappedData, data, dataByteSize);
                Unmap(buffer);
            }

            void CopyBufferOffset(cBuffer& buffer, usize offset, const void* data, usize dataByteSize)
            {
                if (buffer.GetInstance() == nullptr || buffer.GetUsage() == cBuffer::eUsage::STATIC) return;

                cResource::eMapType mapType = cResource::eMapType::WRITE;

                if (buffer.GetUsage() == cBuffer::eUsage::DYNAMIC) {
                    mapType = cResource::eMapType::WRITE_NO_OVERWRITE;
                }

                void* mappedData = Map(buffer, 0, mapType);
                void* dst = (void*)((u8*)mappedData + offset);
                memcpy(dst, data, dataByteSize);
                Unmap(buffer);
            }

            void MoveBuffer(cBuffer& buffer, const void* data, usize dataByteSize)
            {
                if (buffer.GetInstance() == nullptr || buffer.GetUsage() == cBuffer::eUsage::STATIC) return;

                cResource::eMapType mapType = cResource::eMapType::WRITE;

                if (buffer.GetUsage() == cBuffer::eUsage::DYNAMIC) {
                    mapType = cResource::eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(buffer, 0, mapType);
                memmove(mappedData, data, dataByteSize);
                Unmap(buffer);
            }

            void MoveBufferOffset(cBuffer& buffer, usize offset, const void* data, usize dataByteSize)
            {
                if (buffer.GetInstance() == nullptr || buffer.GetUsage() == cBuffer::eUsage::STATIC) return;

                cResource::eMapType mapType = cResource::eMapType::WRITE;

                if (buffer.GetUsage() == cBuffer::eUsage::DYNAMIC) {
                    mapType = cResource::eMapType::WRITE_NO_OVERWRITE;
                }

                void* mappedData = Map(buffer, 0, mapType);
                void* dst = (void*)((u8*)mappedData + offset);
                memmove(dst, data, dataByteSize);
                Unmap(buffer);
            }

            void CopyTexture(const cTexture& texture, const void* data, usize dataByteSize, u32 layerIndex)
            {
                if (texture.GetInstance() == nullptr || texture.GetUsage() == cTexture::eUsage::STATIC) return;

                cResource::eMapType mapType = cResource::eMapType::WRITE;

                if (texture.GetUsage() == cTexture::eUsage::DYNAMIC) {
                    mapType = cResource::eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(texture, layerIndex, mapType);
                if (mappedData == nullptr) {
                    u32 rowPitch = dataByteSize / texture.GetHeight();
                    u32 depthPitch = dataByteSize;
                    UpdateSubData(texture, layerIndex, data, rowPitch, depthPitch);
                }
                else {
                    memcpy(mappedData, data, dataByteSize);
                    Unmap(texture);
                }
            }

            void CopyTextureOffset(const cTexture& texture, usize offset, const void* data, usize dataByteSize, u32 layerIndex)
            {
                if (texture.GetInstance() == nullptr || texture.GetUsage() == cTexture::eUsage::STATIC) return;

                cResource::eMapType mapType = cResource::eMapType::WRITE;

                if (texture.GetUsage() == cTexture::eUsage::DYNAMIC) {
                    mapType = cResource::eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(texture, layerIndex, mapType);
                if (mappedData == nullptr)
                {
                    u32 rowPitch = dataByteSize / texture.GetHeight();
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
                if (texture.GetInstance() == nullptr || texture.GetUsage() == cTexture::eUsage::STATIC) return;

                cResource::eMapType mapType = cResource::eMapType::WRITE;

                if (texture.GetUsage() == cTexture::eUsage::DYNAMIC) {
                    mapType = cResource::eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(texture, layerIndex, mapType);
                if (mappedData == nullptr) {
                    u32 rowPitch = dataByteSize / texture.GetHeight();
                    u32 depthPitch = dataByteSize;
                    UpdateSubData(texture, layerIndex, data, rowPitch, depthPitch);
                }
                else {
                    memmove(mappedData, data, dataByteSize);
                    Unmap(texture);
                }
            }

            void MoveTextureOffset(const cTexture& texture, usize offset, const void* data, usize dataByteSize, u32 layerIndex)
            {
                if (texture.GetInstance() == nullptr || texture.GetUsage() == cTexture::eUsage::STATIC) return;

                cResource::eMapType mapType = cResource::eMapType::WRITE;

                if (texture.GetUsage() == cTexture::eUsage::DYNAMIC) {
                    mapType = cResource::eMapType::WRITE_DISCARD;
                }

                void* mappedData = Map(texture, layerIndex, mapType);
                if (mappedData == nullptr) {
                    u32 rowPitch = dataByteSize / texture.GetHeight();
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

                switch (texture.GetType()) {

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
                if (texture.GetInstance() != nullptr) {

                    switch (texture.GetType()) {

                        case cTexture::eType::TEXTURE_1D:
                            FreeTexture1D(texture.GetInstance());
                            break;

                        case cTexture::eType::TEXTURE_2D:
                            FreeTexture2D(texture.GetInstance());
                            break;

                        case cTexture::eType::TEXTURE_2D_ARRAY:
                            FreeTexture2DArray(texture.GetInstance());
                            break;

                        case cTexture::eType::TEXTURE_2D_DEPTH_STENCIL:
                            FreeTextureDepthStencil(texture.GetInstance());
                            break;

                        case cTexture::eType::TEXTURE_3D:
                            FreeTexture3D(texture.GetInstance());
                            break;

                        case cTexture::eType::TEXTURE_CUBE:
                            FreeTextureCube(texture.GetInstance());
                            break;

                    }

                    //texture.GetInstance() = nullptr;
                }

                if (texture.GetSRVInstance() != nullptr && texture.GetViewType() == cResource::eViewType::SRV)
                {
                    FreeShaderResourceView(texture.GetSRVInstance());
                    //texture.GetSRVInstance() = nullptr;
                } else if (texture.GetUAVInstance() != nullptr && texture.GetViewType() == cResource::eViewType::UAV)
                {
                    FreeUnorderedAccessView(texture.GetUAVInstance());
                    //texture.GetUAVInstance() = nullptr;
                }
            }

            void FreeRenderTarget(cRenderTarget& renderTarget)
            {
                FreeRenderTargetColorViews(renderTarget.GetColorViews());
                FreeRenderTargetDepthView(renderTarget.GetDepthStencilViewPtr());
            }

            void BindRenderTarget(const vector<void*> &colorViews, void* depthView, sViewport* viewport)
            {
                BindRenderTarget(colorViews, depthView);
                BindViewport(viewport);
            }

            void VSBindBuffer(const cResource::eViewType& viewType, cBuffer &buffer, void* view)
            {
                if (viewType == cResource::eViewType::SRV) {
                    VSBindBuffer(cResource::eViewType::SRV, buffer.GetType(), buffer.GetSlot(), buffer.GetInstance(), view);
                } else if (viewType == cResource::eViewType::UAV) {
                    VSBindBuffer(cResource::eViewType::UAV, buffer.GetType(), buffer.GetSlot(), buffer.GetInstance(), view);
                }
            }

            void VSUnbindBuffer(cBuffer &buffer)
            {
                VSBindBuffer(buffer.GetViewType(), buffer.GetType(), buffer.GetSlot(), nullptr, nullptr);
            }

            void PSBindBuffer(const cResource::eViewType& viewType, cBuffer &buffer, void* view)
            {
                if (viewType == cResource::eViewType::SRV) {
                    PSBindBuffer(buffer.GetViewType(), buffer.GetType(), buffer.GetSlot(), buffer.GetInstance(), view);
                } else if (viewType == cResource::eViewType::UAV) {
                    PSBindBuffer(buffer.GetViewType(), buffer.GetType(), buffer.GetSlot(), buffer.GetInstance(), view);
                }
            }

            void PSUnbindBuffer(cBuffer &buffer)
            {
                PSBindBuffer(buffer.GetViewType(), buffer.GetType(), buffer.GetSlot(), nullptr, nullptr);
            }

            void GSBindBuffer(const cResource::eViewType& viewType, cBuffer &buffer)
            {
                if (viewType == cResource::eViewType::SRV) {
                    GSBindBuffer(buffer.GetViewType(), buffer.GetType(), buffer.GetSlot(), buffer.GetInstance(), buffer.GetSRVInstance());
                } else if (viewType == cResource::eViewType::UAV) {
                    GSBindBuffer(buffer.GetViewType(), buffer.GetType(), buffer.GetSlot(), buffer.GetInstance(), buffer.GetSRVInstance());
                }
            }

            void GSUnbindBuffer(cBuffer &buffer)
            {
                GSBindBuffer(buffer.GetViewType(), buffer.GetType(), buffer.GetSlot(), nullptr, nullptr);
            }

            void CSBindBuffer(const cResource::eViewType& viewType, cBuffer &buffer)
            {
                if (viewType == cResource::eViewType::SRV) {
                    CSBindBuffer(buffer.GetViewType(), buffer.GetType(), buffer.GetSlot(), buffer.GetInstance(), buffer.GetSRVInstance());
                } else if (viewType == cResource::eViewType::UAV) {
                    CSBindBuffer(buffer.GetViewType(), buffer.GetType(), buffer.GetSlot(), buffer.GetInstance(), buffer.GetUAVInstance());
                }
            }

            void CSUnbindBuffer(cBuffer &buffer)
            {
                CSBindBuffer(buffer.GetViewType(), buffer.GetType(), buffer.GetSlot(), nullptr, nullptr);
            }

            void VSBindTexture(const cTexture& texture)
            {
                VSBindTexture(texture.GetViewType(), texture.GetSlot(), texture.GetSRVInstance());
            }

            void VSBindTexture(const cTexture &texture, u32 slot)
            {
                VSBindTexture(texture.GetViewType(), slot, texture.GetSRVInstance());
            }

            void VSBindTextureSlot(u32 slot)
            {
                void* nullInstance = nullptr;
                VSBindTexture(cTexture::eViewType::SRV, slot, nullInstance);
            }

            void VSUnbindTexture(const cTexture& texture)
            {
                static void* nullInstance = nullptr;
                VSBindTexture(texture.GetViewType(), texture.GetSlot(), nullInstance);
            }

            void PSUnbindTexture(const cTexture& texture)
            {
                PSBindTexture(texture.GetViewType(), texture.GetSlot(), nullptr, nullptr);
            }

            void GSBindTexture(const cTexture& texture)
            {
                GSBindTexture(texture.GetViewType(), texture.GetSlot(), texture.GetSRVInstance());
            }

            void GSBindTexture(const cTexture &texture, u32 slot)
            {
                GSBindTexture(texture.GetViewType(), slot, texture.GetSRVInstance());
            }

            void GSBindTextureSlot(u32 slot)
            {
                void* nullInstance = nullptr;
                GSBindTexture(cTexture::eViewType::SRV, slot, nullInstance);
            }

            void GSUnbindTexture(const cTexture& texture)
            {
                static void* nullInstance = nullptr;
                GSBindTexture(texture.GetViewType(), texture.GetSlot(), nullInstance);
            }

            void CSBindTexture(const cTexture& texture)
            {
                CSBindTexture(texture.GetViewType(), texture.GetSlot(), texture.GetSRVInstance());
            }

            void CSBindTexture(const cTexture &texture, u32 slot)
            {
                CSBindTexture(texture.GetViewType(), slot, texture.GetSRVInstance());
            }

            void CSBindTextureSlot(u32 slot)
            {
                void* nullInstance = nullptr;
                CSBindTexture(cTexture::eViewType::SRV, slot, nullInstance);
            }

            void CSUnbindTexture(const cTexture& texture)
            {
                static void* nullInstance = nullptr;
                CSBindTexture(texture.GetViewType(), texture.GetSlot(), nullInstance);
            }

            void VSBindSampler(const sSampler &sampler)
            {
                VSBindSampler(sampler.Slot, sampler.GetSRVInstance());
            }

            void VSUnbindSampler(const sSampler& sampler)
            {
                static void* nullInstance = nullptr;
                VSBindSampler(sampler.Slot, nullInstance);
            }

            void PSBindSampler(const sSampler &sampler)
            {
                PSBindSampler(sampler.Slot, sampler.GetSRVInstance());
            }

            void PSUnbindSampler(const sSampler& sampler)
            {
                static void* nullInstance = nullptr;
                PSBindSampler(sampler.Slot, nullInstance);
            }

            void GSBindSampler(const sSampler &sampler)
            {
                GSBindSampler(sampler.Slot, sampler.GetSRVInstance());
            }

            void GSUnbindSampler(const sSampler& sampler)
            {
                static void* nullInstance = nullptr;
                GSBindSampler(sampler.Slot, nullInstance);
            }

            void CSBindSampler(const sSampler &sampler)
            {
                CSBindSampler(sampler.Slot, sampler.GetSRVInstance());
            }

            void CSUnbindSampler(const sSampler& sampler)
            {
                static void* nullInstance = nullptr;
                CSBindSampler(sampler.Slot, nullInstance);
            }

        }

    }

}