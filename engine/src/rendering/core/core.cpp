#include <rendering/core/core.hpp>
#include <rendering/core/context.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_resize.h>

namespace xpe {

    namespace render {

        sVertexFormat::sVertexFormat(const std::vector<sVertexFormat::sAttribute>& attributes)
        : Attributes(attributes)
        {
            for (const auto& attribute : attributes)
            {
                Stride += attribute.ByteSize;
            }
        }

        const sVertexFormat sVertex::k_Format =
        {{
             sAttributes::k_Pos,
             sAttributes::k_UV,
             sAttributes::k_Normal,
             sAttributes::k_Tangent,
             sAttributes::k_BoneIds,
             sAttributes::k_BoneWeights
         }};

        sBlendMode::sBlendMode(const vector<sBlendTarget>& targets)
        {
            Targets = targets;
            Init();
        }

        sBlendMode::~sBlendMode()
        {
            context::FreeBlendMode(*this);
        }

        void sBlendMode::Bind()
        {
            context::BindBlendMode(State);
        }

        void sBlendMode::Init()
        {
            context::CreateBlendMode(*this);
        }

        const std::unordered_map<eTextureFormat, int> cTexture::k_ChannelTable =
        {
                { eTextureFormat::R8, 1 },
                { eTextureFormat::R16, 1 },
                { eTextureFormat::R32, 1 },

                { eTextureFormat::RG8, 2 },
                { eTextureFormat::RG16, 2 },
                { eTextureFormat::RG32, 2 },

                { eTextureFormat::RGB8, 3 },
                { eTextureFormat::RGB16, 3 },
                { eTextureFormat::RGB32, 3 },

                { eTextureFormat::RGBA8, 4 },
                { eTextureFormat::RGBA16, 4 },
                { eTextureFormat::RGBA32, 4 },

                { eTextureFormat::SRGBA8, 4 }
        };

        const std::unordered_map<eTextureFormat, int> cTexture::k_BppTable =
        {
                { eTextureFormat::R8, 1 },
                { eTextureFormat::R16, 2 },
                { eTextureFormat::R32, 4 },

                { eTextureFormat::RG8, 2 },
                { eTextureFormat::RG16, 4 },
                { eTextureFormat::RG32, 8 },

                { eTextureFormat::RGB8, 3 },
                { eTextureFormat::RGB16, 6 },
                { eTextureFormat::RGB32, 12 },

                { eTextureFormat::RGBA8, 4 },
                { eTextureFormat::RGBA16, 8 },
                { eTextureFormat::RGBA32, 16 },

                { eTextureFormat::SRGBA8, 4 },
        };

        cTexture::cTexture(
            eType type,
            eViewType viewType,
            eUsage usage,
            const glm::vec3& size,
            usize channelCount,
            eTextureFormat format,
            usize sampleCount,
            dual enableRenderTarget,
            u32 slot,
            u32 mostDetailedMip,
            dual initializeData,
            const vector<sTextureLayer>& layers
        )
        {
            m_Type = type;
            m_ViewType = viewType;
            m_Usage = usage;
            m_Width = size.x;
            m_Height = size.y;
            m_Depth = size.z;
            m_Channels = channelCount;
            m_Format = format;
            m_SampleCount = sampleCount;
            m_EnableRenderTarget = enableRenderTarget;
            m_Slot = slot;
            m_MostDetailedMip = mostDetailedMip;
            m_InitializeData = initializeData;
            for (auto& layer : layers) {
                m_Layers.emplace_back(layer);
            }

            context::CreateTexture(*this);
        }

        cTexture::~cTexture()
        {
            for (auto& layer : GetLayers())
            {
                layer.Free();
            }
            GetLayers().clear();

            context::FreeTexture(*this);
            RemoveWindowFrameResized();
        }

        sTextureLayer cTexture::CreateLayer() const
        {
            sTextureLayer layer;
            usize size = GetWidth() * GetHeight() * k_BppTable.at(GetFormat());
            layer.Pixels = malloc(size);
            memset(layer.Pixels, 0, size);
            layer.Width = GetWidth();
            layer.Height = GetHeight();
            layer.Format = GetFormat();
            return layer;
        }

        void cTexture::RemoveLayerAt(u32 index)
        {
            auto& layer = GetLayers()[index];
            layer.Free();
            GetLayers().erase(GetLayers().begin() + index);
        }

        u32 cTexture::GetMipLevels() const
        {
            if (m_Layers.empty()) {
                return 1;
            }

            auto& mips = m_Layers.front().Mips;
            return mips.empty() ? 1 : mips.size();
        }

        u32 cTexture::GetMipsLevels(s32 width)
        {
            return (u32)(log(width) / log(2)) + 1;
        }

        void cTexture::WindowFrameResized(s32 width, s32 height)
        {
            Resize(width, height);
        }

        void cTexture::Resize(s32 width, s32 height)
        {
            context::FreeTexture(*this);
            m_Width = width;
            m_Height = height;
            context::CreateTexture(*this);
        }

        void cTexture::ResizePixels(s32 width, s32 height)
        {
            switch (m_Format) {

            case eTextureFormat::R8:
                ResizeTextureU8(width, height);
                break;

            case eTextureFormat::R32:
                ResizeTextureFloat(width, height);
                break;

            case eTextureFormat::RG8:
                ResizeTextureU8(width, height);
                break;

            case eTextureFormat::RG32:
                ResizeTextureFloat(width, height);
                break;

            case eTextureFormat::RGB8:
                ResizeTextureU8(width, height);
                break;

            case eTextureFormat::RGB32:
                ResizeTextureFloat(width, height);
                break;

            case eTextureFormat::RGBA8:
                ResizeTextureU8(width, height);
                break;

            case eTextureFormat::SRGBA8:
                ResizeTextureU8(width, height);
                break;

            case eTextureFormat::RGBA32:
                ResizeTextureFloat(width, height);
                break;

            }

            m_Width = width;
            m_Height = height;
        }

        void cTexture::ResizeTextureU8(s32 width, s32 height) {
            for (auto& layer : m_Layers) {
                layer.ResizeU8(width, height);
            }
        }

        void cTexture::ResizeTextureFloat(s32 width, s32 height) {
            for (auto& layer : m_Layers) {
                layer.ResizeFloat(width, height);
            }
        }

        void cTexture::Flip()
        {
            int bpp = k_BppTable.at(m_Format);
            for (auto& layer : m_Layers) {
                stbi__vertical_flip(layer.Pixels, m_Width, m_Height, bpp);
            }
        }

        void cTexture::GenerateMips()
        {
            for (auto& layer : m_Layers) {
                layer.GenerateMips(m_Format, m_Width, m_Height);
            }
        }

        void cTexture::FlushLayer(u32 index)
        {
            context::CopyTexture(
                *this,
                m_Layers[index].Pixels,
                m_Width * m_Height * k_BppTable.at(m_Format),
                index
            );
        }

        void cTexture::Flush()
        {
            //            if (Instance == nullptr) {
            context::FreeTexture(*this);
            context::CreateTexture(*this);
            //            }

            //            else {
            //                u32 layerCount = Layers.size();
            //                for (u32 i = 0 ; i < layerCount ; i++)
            //                {
            //                    FlushLayer(i);
            //                }
            //            }
        }

        void cTexture::SetResizable(bool resizable)
        {
            m_Resizable = resizable;
            if (resizable) {
                AddWindowFrameResized(cTexture, eWindowFrameResizedPriority::TEXTURE);
            }
            else {
                RemoveWindowFrameResized();
            }
        }

        void sTextureLayer::Free()
        {
            if (Pixels != nullptr) {
                main_free(Pixels);
                FreeMips();
                Pixels = nullptr;
            }
        }

        void sTextureLayer::CopyFrom(const sTextureLayer& other)
        {
            Width = other.Width;
            Height = other.Height;
            Format = other.Format;
            usize size = Width * Height * cTexture::k_BppTable.at(Format);
            main_free(Pixels);
            Pixels = main_alloc(size);
            memcpy(Pixels, other.Pixels, size);
        }

        sTextureLayer sTextureLayer::Clone() const
        {
            sTextureLayer clone;
            clone.CopyFrom(*this);
            return clone;
        }

        void sTextureLayer::GenerateMips(const eTextureFormat& format, int width, int height)
        {
            if (Pixels == nullptr) return;

            switch (format) {

            case eTextureFormat::R8:
                GenerateMipsU8(width, height);
                break;

            case eTextureFormat::R32:
                GenerateMipsFloat(width, height);
                break;

            case eTextureFormat::R32_TYPELESS:
                GenerateMipsFloat(width, height);
                break;

            case eTextureFormat::R16_TYPELESS:
                GenerateMipsFloat(width, height);
                break;

            case eTextureFormat::RG8:
                GenerateMipsU8(width, height);
                break;

            case eTextureFormat::RG32:
                GenerateMipsFloat(width, height);
                break;

            case eTextureFormat::RGB8:
                GenerateMipsU8(width, height);
                break;

            case eTextureFormat::RGB32:
                GenerateMipsFloat(width, height);
                break;

            case eTextureFormat::RGBA8:
                GenerateMipsU8(width, height);
                break;

            case eTextureFormat::SRGBA8:
                GenerateMipsU8(width, height);
                break;

            case eTextureFormat::RGBA32:
                GenerateMipsFloat(width, height);
                break;

            }
        }

        void sTextureLayer::GenerateMipsU8(int width, int height)
        {
            void* previousMip = Pixels;
            while (width != 1 && height != 1)
            {
                width /= 2;
                height /= 2;

                previousMip = ResizePixelsU8(previousMip, width * 2, height * 2, width, height);

                Mips.emplace_back(Format, width, height, previousMip);
            }
        }

        void sTextureLayer::GenerateMipsFloat(int width, int height) {
            void* previousMip = Pixels;
            while (width != 1 && height != 1)
            {
                width /= 2;
                height /= 2;

                previousMip = ResizePixelsFloat(previousMip, width * 2, height * 2, width, height);

                Mips.emplace_back(Format, width, height, previousMip);
            }
        }

        void sTextureLayer::FreeMips() {
            for (auto& mip : Mips) {
                if (mip.Pixels != nullptr) {
                    main_free(mip.Pixels);
                    mip.Pixels = nullptr;
                }
            }
            Mips.clear();
        }

        void sTextureLayer::Resize(const eTextureFormat& format, s32 width, s32 height)
        {
            switch (format) {

            case eTextureFormat::R8:
                ResizeU8(width, height);
                break;

            case eTextureFormat::R32:
                ResizeFloat(width, height);
                break;

            case eTextureFormat::RG8:
                ResizeU8(width, height);
                break;

            case eTextureFormat::RG32:
                ResizeFloat(width, height);
                break;

            case eTextureFormat::RGB8:
                ResizeU8(width, height);
                break;

            case eTextureFormat::RGB32:
                ResizeFloat(width, height);
                break;

            case eTextureFormat::RGBA8:
                ResizeU8(width, height);
                break;

            case eTextureFormat::SRGBA8:
                ResizeU8(width, height);
                break;

            case eTextureFormat::RGBA32:
                ResizeFloat(width, height);
                break;

            }
        }

        void sTextureLayer::ResizeU8(s32 width, s32 height) {
            void* pixels = ResizePixelsU8(
                Pixels,
                Width, Height,
                width, height
            );
            Pixels = pixels;
            Width = width;
            Height = height;

            if (!Mips.empty()) {
                FreeMips();
                GenerateMipsU8(width, height);
            }
        }

        void sTextureLayer::ResizeFloat(s32 width, s32 height) {
            void* pixels = ResizePixelsFloat(
                Pixels,
                Width, Height,
                width, height
            );
            Pixels = pixels;
            Width = width;
            Height = height;

            if (!Mips.empty()) {
                FreeMips();
                GenerateMipsFloat(width, height);
            }
        }

        void* sTextureLayer::ResizePixelsU8(
            const void* inputPixels, int inputWidth, int inputHeight,
            int outputWidth, int outputHeight
        ) {
            int channels = cTexture::k_ChannelTable.at(Format);
            auto* output = main_allocT(u8, outputWidth * outputHeight * channels);
            stbir_resize_uint8(
                (const u8*)inputPixels,
                inputWidth, inputHeight, 0,
                output,
                outputWidth, outputHeight, 0,
                channels
            );
            return output;
        }

        void* sTextureLayer::ResizePixelsFloat(
            const void* inputPixels, int inputWidth, int inputHeight,
            int outputWidth, int outputHeight
        ) {
            int channels = cTexture::k_ChannelTable.at(Format);
            auto* output = main_allocT(float, outputWidth * outputHeight * channels);
            stbir_resize_float(
                (const float*)inputPixels,
                inputWidth, inputHeight, 0,
                output,
                outputWidth, outputHeight, 0,
                channels
            );
            return output;
        }

        sAtlas2DTexture cAtlas2D::AddTexture(const glm::vec2& size)
        {
            for (s32 yy = 0; yy < m_Height; yy++)
            {
                for (s32 xx = 0; xx < m_Width; xx++)
                {
                    dual intersection = false;
                    glm::vec2 newBottomLeft = glm::vec2(xx, yy);
                    glm::vec2 newTopRight = newBottomLeft + size;

                    for (auto& texture : m_Textures)
                    {
                        glm::vec2 texBottomLeft = glm::vec2(texture.Offsets.x, texture.Offsets.y);
                        glm::vec2 texTopRight = glm::vec2(texture.Offsets.z, texture.Offsets.w);

                        if ((newBottomLeft.x < texTopRight.x &&
                            newTopRight.x > texBottomLeft.x &&
                            newBottomLeft.y < texTopRight.y &&
                            newTopRight.y > texBottomLeft.y) ||
                            (newTopRight.x > m_Width) || (newTopRight.y > m_Height))
                        {
                            intersection = true;
                            break;
                        }
                    }

                    if (intersection == false)
                    {
                        m_Textures.emplace_back(
                            glm::vec4(newBottomLeft.x, newBottomLeft.y, newTopRight.x, newTopRight.y)
                        );

                        return m_Textures[m_Textures.size() - 1];
                    }
                }
            }

            return { glm::vec4(0.0f)};
        }

        void cAtlas2D::RemoveTexture(const sAtlas2DTexture& texture)
        {
            for (auto it = m_Textures.begin(); it < m_Textures.end(); it++) {
                if (it->Offsets == texture.Offsets) {
                    m_Textures.erase(it);
                }
            }
        }

        void sAtlas::AddLayer()
        {
            m_Layers.emplace_back(CreateLayer());
        }

        sSampler::sSampler()
        {
            m_ViewType = eViewType::SRV;
        }

        sSampler::~sSampler()
        {
            context::FreeSampler(*this);
        }

        void sSampler::Init()
        {
            context::CreateSampler(*this);
        }

    }

}