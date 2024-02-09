#include <bindings.hpp>

namespace xpe
{
    namespace render
    {
        sViewport*           Viewports::Canvas = nullptr;
        sViewport*           Viewports::DirectionalShadow = nullptr;
        sViewport*           Viewports::PointShadow = nullptr;
        sViewport*           Viewports::SpotShadow = nullptr;
        sViewport*           Viewports::SSAO = nullptr;

        cRenderTarget*       RenderTargets::Scene = nullptr;
        cRenderTarget*       RenderTargets::Opaque = nullptr;
        cRenderTarget*       RenderTargets::Transparent = nullptr;
        cRenderTarget*       RenderTargets::UI = nullptr;
        cRenderTarget*       RenderTargets::Final = nullptr;
        cRenderTarget*       RenderTargets::DirectionalShadow = nullptr;
        cRenderTarget*       RenderTargets::PointShadow = nullptr;
        cRenderTarget*       RenderTargets::SpotShadow = nullptr;
        cRenderTarget*       RenderTargets::SSAO = nullptr;

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

        cTexture*            Textures::Canvas = nullptr;
        cTexture*            Textures::SharedDepth = nullptr;
        cTexture*            Textures::AlbedoAtlas = nullptr;
        cTexture*            Textures::NormalAtlas = nullptr;
        cTexture*            Textures::ParallaxAtlas = nullptr;
        cTexture*            Textures::MetalAtlas = nullptr;
        cTexture*            Textures::RoughnessAtlas = nullptr;
        cTexture*            Textures::AOAtlas = nullptr;
        cTexture*            Textures::EmissionAtlas = nullptr;
        cAtlas*              Textures::WidgetAtlas = nullptr;
        cCircleFilter3D*     Textures::ShadowCircle = nullptr;

        cSampler*            Samplers::Material = nullptr;
        cSampler*            Samplers::Shadow = nullptr;
        cSampler*            Samplers::Widget = nullptr;
        cSampler*            Samplers::Skybox = nullptr;
        cSampler*            Samplers::Font = nullptr;
        cSampler*            Samplers::ShadowCircle = nullptr;
    }
}