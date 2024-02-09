#include <render_manager.hpp>
#include <material_manager.hpp>
#include <geometry_manager.hpp>
#include <anim_manager.hpp>
#include <camera_manager.hpp>

namespace xpe
{
    namespace render
    {
        bool MRender::EnableInfoLog = false;
        bool MRender::EnableWarnLog = false;
        bool MRender::EnableErrorLog = true;
        u32  MRender::AnisotropyLevel = 1;

        void MRender::Init(sViewport& viewport, u32 sampleCount)
        {
            context::Init();
            InitManagers(viewport, sampleCount);
            InitBuffers(viewport, sampleCount);
            InitRenderTargets(viewport, sampleCount);
            MEvent::WindowFrameResized.Add("MRender", eWindowFrameResizedPriority::RENDER_SYSTEM, [](int w, int h)
            {
                MRender::WindowFrameResized(w, h);
            });
        }

        void MRender::Free()
        {
            FreeBuffers();
            FreeRenderTargets();
            FreeManagers();
            context::Free();
            MEvent::WindowFrameResized.Remove("MRender");
        }

        void MRender::InitManagers(sViewport &viewport, u32 sampleCount)
        {
            MShader::Init();
            MCamera::Init(MWindow::GetFrameWidth(), MWindow::GetFrameHeight());
            MGeometry::Init();
            MTexture::Init();
            MMaterial::Init();
            MFont::Init();
            MAnim::Init();
        }

        void MRender::InitBuffers(sViewport &viewport, u32 sampleCount)
        {
            Buffers::DirectLight = new sDirectLightBuffer();
            Buffers::DirectLight->Reserve(10);
            Buffers::PointLight = new sPointLightBuffer();
            Buffers::PointLight->Reserve(10);
            Buffers::SpotLight = new sSpotLightBuffer();
            Buffers::SpotLight->Reserve(10);
        }

        void MRender::InitRenderTargets(sViewport& viewport, u32 sampleCount)
        {
            // Shared depth texture for opaque and transparent render targets
            Textures::SharedDepth = new cTexture();
            Textures::SharedDepth->Type = cTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            Textures::SharedDepth->Width = viewport.Width;
            Textures::SharedDepth->Height = viewport.Height;
            Textures::SharedDepth->Format = eTextureFormat::R32_TYPELESS;
            Textures::SharedDepth->InitializeData = false;
            Textures::SharedDepth->EnableRenderTarget = true;
            Textures::SharedDepth->SampleCount = sampleCount;
            Textures::SharedDepth->SetResizable(false);
            Textures::SharedDepth->Init();

            // Opaque render target
            cTexture* opaqueColor = new cTexture();
            opaqueColor->Width = viewport.Width;
            opaqueColor->Height = viewport.Height;
            opaqueColor->Format = eTextureFormat::HDR;
            opaqueColor->InitializeData = false;
            opaqueColor->EnableRenderTarget = true;
            opaqueColor->SampleCount = sampleCount;
            opaqueColor->SetResizable(false);
            opaqueColor->Init();

            cTexture* opaquePosition = new cTexture();
            opaquePosition->Width = viewport.Width;
            opaquePosition->Height = viewport.Height;
            opaquePosition->Format = eTextureFormat::RGBA32;
            opaquePosition->InitializeData = false;
            opaquePosition->EnableRenderTarget = true;
            opaquePosition->SampleCount = sampleCount;
            opaquePosition->SetResizable(false);
            opaquePosition->Init();

            cTexture* opaqueNormal = new cTexture();
            opaqueNormal->Width = viewport.Width;
            opaqueNormal->Height = viewport.Height;
            opaqueNormal->Format = eTextureFormat::RGBA16;
            opaqueNormal->InitializeData = false;
            opaqueNormal->EnableRenderTarget = true;
            opaqueNormal->SampleCount = sampleCount;
            opaqueNormal->SetResizable(false);
            opaqueNormal->Init();

            RenderTargets::Opaque = new cRenderTarget({ opaqueColor, opaquePosition, opaqueNormal }, Textures::SharedDepth, &viewport);
            RenderTargets::Opaque->SetResizable(false);
            RenderTargets::Opaque->ClearColors.emplace_back(glm::vec4(0));
            RenderTargets::Opaque->ClearColors.emplace_back(glm::vec4(0));
            RenderTargets::Opaque->ClearColors.emplace_back(glm::vec4(0));
            RenderTargets::Opaque->ClearDepth = 1;

            // Transparent render target
            cTexture* transparentAccum = new cTexture();
            transparentAccum->Width = viewport.Width;
            transparentAccum->Height = viewport.Height;
            transparentAccum->Format = eTextureFormat::HDR;
            transparentAccum->InitializeData = false;
            transparentAccum->EnableRenderTarget = true;
            transparentAccum->SampleCount = sampleCount;
            transparentAccum->SetResizable(false);
            transparentAccum->Init();

            cTexture* transparentReveal = new cTexture();
            transparentReveal->Width = viewport.Width;
            transparentReveal->Height = viewport.Height;
            transparentReveal->Format = eTextureFormat::R8;
            transparentReveal->InitializeData = false;
            transparentReveal->EnableRenderTarget = true;
            transparentReveal->SampleCount = sampleCount;
            transparentReveal->SetResizable(false);
            transparentReveal->Init();

            RenderTargets::Transparent = new cRenderTarget({transparentAccum, transparentReveal }, Textures::SharedDepth, &viewport);
            RenderTargets::Transparent->SetResizable(false);
            RenderTargets::Transparent->ClearColors.emplace_back(glm::vec4(0.0f));
            RenderTargets::Transparent->ClearColors.emplace_back(glm::vec4(1.0f));

            // Scene render target
            cTexture* sceneColor = new cTexture();
            sceneColor->Width = viewport.Width;
            sceneColor->Height = viewport.Height;
            sceneColor->Format = eTextureFormat::RGBA8;
            sceneColor->InitializeData = false;
            sceneColor->EnableRenderTarget = true;
            sceneColor->SetResizable(true);
            sceneColor->Init();

            cTexture* sceneDepth = new cTexture();
            sceneDepth->Type = cTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            sceneDepth->Width = viewport.Width;
            sceneDepth->Height = viewport.Height;
            sceneDepth->Format = eTextureFormat::R32_TYPELESS;
            sceneDepth->InitializeData = false;
            sceneDepth->EnableRenderTarget = true;
            sceneDepth->SetResizable(true);
            sceneDepth->Init();

            RenderTargets::Scene = new cRenderTarget({ sceneColor }, sceneDepth, &viewport);
            RenderTargets::Scene->SetResizable(true);
            RenderTargets::Scene->ClearColors.emplace_back(glm::vec4(0.0f));
            RenderTargets::Scene->ClearDepth = 1.0f;

            // UI render target
            cTexture* uiColor = new cTexture();
            uiColor->Width = viewport.Width;
            uiColor->Height = viewport.Height;
            uiColor->Format = eTextureFormat::RGBA8;
            uiColor->InitializeData = false;
            uiColor->EnableRenderTarget = true;
            uiColor->SetResizable(true);
            uiColor->Init();

            cTexture* uiDepth = new cTexture();
            uiDepth->Type = cTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            uiDepth->Width = viewport.Width;
            uiDepth->Height = viewport.Height;
            uiDepth->Format = eTextureFormat::R32_TYPELESS;
            uiDepth->InitializeData = false;
            uiDepth->EnableRenderTarget = true;
            uiDepth->SetResizable(true);
            uiDepth->Init();

            RenderTargets::UI = new cRenderTarget({ uiColor }, uiDepth, &viewport);
            RenderTargets::UI->SetResizable(true);
            RenderTargets::UI->ClearColors.emplace_back(glm::vec4(0.0f));
            RenderTargets::UI->ClearDepth = 1.0f;

            // Final render target
            cTexture* finalColor = new cTexture();
            finalColor->Width = viewport.Width;
            finalColor->Height = viewport.Height;
            finalColor->Format = eTextureFormat::RGBA8;
            finalColor->InitializeData = false;
            finalColor->EnableRenderTarget = true;
            finalColor->SetResizable(true);
            finalColor->Init();

            cTexture* finalDepth = new cTexture();
            finalDepth->Type = cTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            finalDepth->Width = viewport.Width;
            finalDepth->Height = viewport.Height;
            finalDepth->Format = eTextureFormat::R32_TYPELESS;
            finalDepth->InitializeData = false;
            finalDepth->EnableRenderTarget = true;
            finalDepth->SetResizable(true);
            finalDepth->Init();

            RenderTargets::Final = new cRenderTarget({ finalColor }, finalDepth, &viewport);
            RenderTargets::Final->SetResizable(true);
            RenderTargets::Final->ClearColors.emplace_back(glm::vec4(0.0f));
            RenderTargets::Final->ClearDepth = 1.0f;

            Textures::Canvas = RenderTargets::Final->Colors[0];

            Samplers::Widget               = new cSampler();
            Samplers::Widget->Slot         = K_SLOT_WIDGET_SAMPLER;
            Samplers::Widget->Filter       = cSampler::eFilter::MIN_MAG_MIP_LINEAR;
            Samplers::Widget->AddressU     = cSampler::eAddress::WRAP;
            Samplers::Widget->AddressV     = cSampler::eAddress::WRAP;
            Samplers::Widget->AddressW     = cSampler::eAddress::WRAP;
            Samplers::Widget->Init();

            Textures::WidgetAtlas = new cAtlas();
            Textures::WidgetAtlas->InitializeData = true;
            Textures::WidgetAtlas->Type = cTexture::eType::TEXTURE_2D;
            Textures::WidgetAtlas->Usage = cTexture::eUsage::DEFAULT;
            Textures::WidgetAtlas->Format = eTextureFormat::RGBA8;
            Textures::WidgetAtlas->Width = 1024;
            Textures::WidgetAtlas->Height = 1024;
            Textures::WidgetAtlas->Channels = 4;
            Textures::WidgetAtlas->Slot = K_SLOT_WIDGET_ATLAS;
            Textures::WidgetAtlas->EnableMipmapping = true;
            Textures::WidgetAtlas->AddLayer();
            Textures::WidgetAtlas->Init();

            Buffers::ShadowPCF = new sShadowPcfBuffer();

            Samplers::Shadow              = new cSampler();
            Samplers::Shadow->Slot        = K_SLOT_SHADOW_SAMPLER;
            Samplers::Shadow->Filter      = cSampler::eFilter::MIN_MAG_MIP_LINEAR;
            Samplers::Shadow->BorderColor = glm::vec4(1, 1, 1, 1);
            Samplers::Shadow->AddressU    = cSampler::eAddress::BORDER;
            Samplers::Shadow->AddressV    = cSampler::eAddress::BORDER;
            Samplers::Shadow->AddressW    = cSampler::eAddress::BORDER;
            Samplers::Shadow->Init();

            Textures::ShadowCircle = new cCircleFilter3D();
            Textures::ShadowCircle->Usage = cTexture::eUsage::DEFAULT;
            Textures::ShadowCircle->Width = 3 * 3 / 2;
            Textures::ShadowCircle->Height = 9;
            Textures::ShadowCircle->Depth = 9;
            Textures::ShadowCircle->Slot = K_SLOT_SHADOW_CIRCLE_FILTER;
            Textures::ShadowCircle->Init();

            Samplers::ShadowCircle = new cSampler();
            Samplers::ShadowCircle->Slot = K_SLOT_SHADOW_CIRCLE_SAMPLER;
            Samplers::ShadowCircle->Filter = cSampler::eFilter::MIN_MAG_MIP_POINT;
            Samplers::ShadowCircle->AddressU = cSampler::eAddress::WRAP;
            Samplers::ShadowCircle->AddressV = cSampler::eAddress::WRAP;
            Samplers::ShadowCircle->AddressW = cSampler::eAddress::WRAP;
            Samplers::ShadowCircle->Init();

            Viewports::DirectionalShadow = new sViewport();
            Viewports::DirectionalShadow->Width = 256;
            Viewports::DirectionalShadow->Height = 256;

            Viewports::PointShadow = new sViewport();
            Viewports::PointShadow->Width = 256;
            Viewports::PointShadow->Height = 256;

            Viewports::SpotShadow = new sViewport();
            Viewports::SpotShadow->Width = 256;
            Viewports::SpotShadow->Height = 256;

            cTexture* shadowColor = new cTexture();
            shadowColor->Width = 256;
            shadowColor->Height = 256;
            shadowColor->Format = eTextureFormat::R32;
            shadowColor->InitializeData = false;
            shadowColor->EnableRenderTarget = true;
            shadowColor->Slot = K_SLOT_SHADOW_ATLAS;
            shadowColor->SetResizable(false);
            shadowColor->Init();

            cTexture* shadowDepth = new cTexture();
            shadowDepth->Type = cTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            shadowDepth->Width = 256;
            shadowDepth->Height = 256;
            shadowDepth->Format = eTextureFormat::R32_TYPELESS;
            shadowDepth->InitializeData = false;
            shadowDepth->EnableRenderTarget = true;
            shadowDepth->Slot = K_SLOT_SHADOW_ATLAS;
            shadowDepth->SetResizable(false);
            shadowDepth->Init();

            RenderTargets::DirectionalShadow = new cRenderTarget({ shadowColor }, shadowDepth, Viewports::DirectionalShadow);
            RenderTargets::DirectionalShadow->SetResizable(false);
            RenderTargets::DirectionalShadow->ClearColors.emplace_back(glm::vec4(1.0f));
            RenderTargets::DirectionalShadow->ClearDepth = 1.0f;
        }

        void MRender::FreeManagers()
        {
            MAnim::Free();
            MFont::Free();
            MMaterial::Free();
            MTexture::Free();
            MGeometry::Free();
            MCamera::Free();
            MShader::Free();
        }

        void MRender::FreeBuffers()
        {
            delete Buffers::DirectLight;
            delete Buffers::PointLight;
            delete Buffers::SpotLight;
            delete Buffers::ShadowPCF;
        }

        void MRender::FreeRenderTargets()
        {
            delete RenderTargets::Scene;
            delete RenderTargets::Opaque;
            delete RenderTargets::Transparent;
            delete RenderTargets::UI;
            delete RenderTargets::Final;
            delete Samplers::Widget;
            delete Textures::WidgetAtlas;
            delete Buffers::ShadowPCF;
            delete Samplers::Shadow;
            delete RenderTargets::DirectionalShadow;
            delete Viewports::DirectionalShadow;
            for (auto* color : RenderTargets::DirectionalShadow->Colors)
            {
                delete color;
            }
            delete RenderTargets::DirectionalShadow->DepthStencil;
            delete Textures::ShadowCircle;
            delete Samplers::ShadowCircle;
        }

        void MRender::WindowFrameResized(int width, int height)
        {
            MCamera::Resize(width, height);

            Textures::SharedDepth->Resize(width, height);

            RenderTargets::Opaque->DepthStencil = Textures::SharedDepth;
            RenderTargets::Opaque->ResizeColor(width, height);

            RenderTargets::Transparent->DepthStencil = Textures::SharedDepth;
            RenderTargets::Transparent->ResizeColor(width, height);
        }

        void MRender::Prepare()
        {
            context::BindVertexBuffer(*Buffers::Vertex);
            context::BindIndexBuffer(*Buffers::Index);
        }

        void MRender::Update(cScene* scene, float dt)
        {
            Profile();

            Buffers::Skeleton->Flush();
            UpdateLight(scene);
            UpdateShaders(scene);
        }

        void MRender::UpdateLight(cScene* scene)
        {
            Profile();

            Buffers::DirectLight->Clear();
            {
                auto components = scene->GetComponents<CDirectionalLight>();
                for (auto [entity, light] : components.each())
                {
                    sDirectLightData lightData;
                    lightData.Position = light.View.Position;
                    lightData.Color = light.Color * light.Intensity;
                    lightData.ViewProjection = MMath::UpdateLightMatrix(light.Projection, light.View);
                    Buffers::DirectLight->Add(lightData);
                }
            }
            Buffers::DirectLight->Flush();

            Buffers::SpotLight->Clear();
            {
                auto components = scene->GetComponents<CSpotLight>();
                for (auto [entity, light] : components.each())
                {
                    glm::quat orientation = glm::quat({ -light.Rotation.x, -light.Rotation.y, light.Rotation.z });
                    glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), light.View.Position) * glm::toMat4(orientation);
                    viewMatrix = glm::inverse(viewMatrix);

                    sSpotLightData lightData;
                    lightData.Position = light.View.Position;
                    lightData.Direction = glm::rotate(orientation, glm::vec3(0.0f, 0.0f, 1.0f));
                    lightData.Color = light.Color;
                    lightData.InnerCutoff = glm::cos(glm::radians(light.InnerCutoff));
                    lightData.OuterCutoff = glm::cos(glm::radians(light.OuterCutoff));
                    lightData.ViewProjection = MMath::UpdatePerspectiveMatrix(light.Projection) * viewMatrix;
                    lightData.Near = light.Projection.Near;
                    lightData.Far = light.Projection.Far;
                    lightData.Constant = light.Constant;
                    lightData.Linear = light.Linear;
                    lightData.Quadratic = light.Quadratic;
                    Buffers::SpotLight->Add(lightData);

                    LogInfo("Spot light direction [{},{},{}]", lightData.Direction.x, lightData.Direction.y, lightData.Direction.z);
                }
            }
            Buffers::SpotLight->Flush();

            Buffers::PointLight->Clear();
            {
                auto components = scene->GetComponents<CPointLight>();
                for (auto [entity, light] : components.each())
                {
                    sPointLightData lightData;
                    lightData.Position = light.Position;
                    lightData.Color = light.Color * light.Intensity;
                    lightData.Constant = light.Constant;
                    lightData.Linear = light.Linear;
                    lightData.Quadratic = light.Quadratic;
                    lightData.ViewProjection = MMath::UpdateLightMatrix(light.Projection, light.View);
                    Buffers::PointLight->Add(lightData);
                }
            }
            Buffers::PointLight->Flush();
        }

        void MRender::UpdateShaders(cScene* scene)
        {
            Profile();

            // Compute
            {
                cShader* shader = MShader::GetShaders(cShader::eCategory::COMPUTE);
                while (shader != nullptr)
                {
                    if (shader->Enable)
                    {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                    shader = shader->Next;
                }
            }

            // Prepass
            RenderTargets::DirectionalShadow->Clear();
            {
                cShader* shader = MShader::GetShaders(cShader::eCategory::PREPASS);
                while (shader != nullptr) {
                    if (shader->Enable)
                    {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                    shader = shader->Next;
                }
            }

            // Opaque
            RenderTargets::Opaque->Clear();
            {
                cShader* shader = MShader::GetShaders(cShader::eCategory::OPAQUE);
                while (shader != nullptr)
                {
                    if (shader->Enable)
                    {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                    shader = shader->Next;
                }
            }

            // Transparent
            RenderTargets::Transparent->Clear();
            {
                cShader* shader = MShader::GetShaders(cShader::eCategory::TRANSPARENT);
                while (shader != nullptr)
                {
                    if (shader->Enable)
                    {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                    shader = shader->Next;
                }
            }

            // PostFX
            RenderTargets::Scene->Clear();
            {
                cShader* shader = MShader::GetShaders(cShader::eCategory::POSTFX);
                while (shader != nullptr)
                {
                    if (shader->Enable)
                    {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                    shader = shader->Next;
                }
            }

            // UI
            RenderTargets::UI->Clear();
            {
                cShader* shader = MShader::GetShaders(cShader::eCategory::UI);
                while (shader != nullptr)
                {
                    if (shader->Enable)
                    {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                    shader = shader->Next;
                }
            }

            // Final
            RenderTargets::Final->Clear();
            {
                cShader* shader = MShader::GetShaders(cShader::eCategory::FINAL);
                while (shader != nullptr)
                {
                    if (shader->Enable)
                    {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                    shader = shader->Next;
                }
            }
        }
    }
}