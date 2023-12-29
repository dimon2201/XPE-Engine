#include <rendering/bindings.hpp>

namespace xpe {

    namespace render {

        sViewport*           Viewports::Shadow = nullptr;
        sViewport*           Viewports::SSAO = nullptr;

        sRenderTarget*       RenderTargets::Scene = nullptr;
        sRenderTarget*       RenderTargets::Opaque = nullptr;
        sRenderTarget*       RenderTargets::Transparent = nullptr;
        sRenderTarget*       RenderTargets::UI = nullptr;
        sRenderTarget*       RenderTargets::Final = nullptr;
        sRenderTarget*       RenderTargets::Shadow = nullptr;
        sRenderTarget*       RenderTargets::SSAO = nullptr;

        sVertexBuffer*       Buffers::Vertex = nullptr;
        sIndexBuffer*        Buffers::Index = nullptr;
        sCameraBuffer*       Buffers::Camera = nullptr;
        sDirectLightBuffer*  Buffers::DirectLight = nullptr;
        sPointLightBuffer*   Buffers::PointLight = nullptr;
        sSpotLightBuffer*    Buffers::SpotLight = nullptr;
        sMaterialDataBuffer* Buffers::Material = nullptr;
        sShadowPcfBuffer*    Buffers::ShadowPCF = nullptr;
        sSkeletonBuffer*     Buffers::Skeleton = nullptr;
        sSsaoBuffer*         Buffers::SSAO = nullptr;

        sTexture*            Textures::Canvas = nullptr;
        sTexture*            Textures::SharedDepth = nullptr;
        sTexture*            Textures::AlbedoAtlas = nullptr;
        sTexture*            Textures::NormalAtlas = nullptr;
        sTexture*            Textures::ParallaxAtlas = nullptr;
        sTexture*            Textures::MetalAtlas = nullptr;
        sTexture*            Textures::RoughnessAtlas = nullptr;
        sTexture*            Textures::AOAtlas = nullptr;
        sTexture*            Textures::EmissionAtlas = nullptr;
        sAtlas*              Textures::WidgetAtlas = nullptr;

        sSampler*            Samplers::Material = nullptr;
        sSampler*            Samplers::Shadow = nullptr;
        sSampler*            Samplers::Widget = nullptr;
        sSampler*            Samplers::Skybox = nullptr;
        sSampler*            Samplers::Font = nullptr;

    }

}