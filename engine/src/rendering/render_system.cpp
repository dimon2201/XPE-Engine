#include <rendering/render_system.hpp>
#include <rendering/material/material_manager.hpp>
#include <rendering/geometry/geometry_manager.hpp>
#include <rendering/skybox_manager.hpp>
#include <rendering/camera_manager.hpp>
#include <rendering/shadow_manager.hpp>

#include <anim/skeleton_manager.hpp>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        cRenderSystem::cRenderSystem(sViewport& viewport, u32 sampleCount)
        {
            context::Init();
            InitManagers(viewport, sampleCount);
            cCameraManager::SetViewport(viewport);
            InitBuffers(*cCameraManager::GetViewport(), sampleCount);
            InitRenderTargets(*cCameraManager::GetViewport(), sampleCount);
            AddWindowFrameResized(cRenderSystem, eWindowFrameResizedPriority::RENDER_SYSTEM);
        }

        cRenderSystem::~cRenderSystem()
        {
            FreeBuffers();
            FreeRenderTargets();
            FreeManagers();
            context::Free();
            RemoveWindowFrameResized();
        }

        void cRenderSystem::InitManagers(sViewport &viewport, u32 sampleCount)
        {
            cShaderManager::Init();
            cCameraManager::Init();
            cGeometryManager::Init();
            cMaterialManager::Init();
            cSkeletonManager::Init();
            cSkyboxManager::Init();
            cShadowManager::Init();
            cWidgetManager::Init();
        }

        void cRenderSystem::InitBuffers(sViewport &viewport, u32 sampleCount)
        {
            Buffers::DirectLight = new sDirectLightBuffer();
            Buffers::DirectLight->Reserve(10);
            Buffers::PointLight = new sPointLightBuffer();
            Buffers::PointLight->Reserve(10);
            Buffers::SpotLight = new sSpotLightBuffer();
            Buffers::SpotLight->Reserve(10);
        }

        void cRenderSystem::InitRenderTargets(sViewport& viewport, u32 sampleCount)
        {
            // Shared depth texture for opaque and transparent render targets
            Textures::SharedDepth = new sTexture();
            Textures::SharedDepth->Type = sTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            Textures::SharedDepth->Width = viewport.Width;
            Textures::SharedDepth->Height = viewport.Height;
            Textures::SharedDepth->Format = eTextureFormat::R32_TYPELESS;
            Textures::SharedDepth->InitializeData = false;
            Textures::SharedDepth->EnableRenderTarget = true;
            Textures::SharedDepth->SampleCount = sampleCount;
            Textures::SharedDepth->SetResizable(false);
            Textures::SharedDepth->Init();

            // Opaque render target
            sTexture* opaqueColor = new sTexture();
            opaqueColor->Width = viewport.Width;
            opaqueColor->Height = viewport.Height;
            opaqueColor->Format = eTextureFormat::HDR;
            opaqueColor->InitializeData = false;
            opaqueColor->EnableRenderTarget = true;
            opaqueColor->SampleCount = sampleCount;
            opaqueColor->SetResizable(false);
            opaqueColor->Init();

            sTexture* opaquePosition = new sTexture();
            opaquePosition->Width = viewport.Width;
            opaquePosition->Height = viewport.Height;
            opaquePosition->Format = eTextureFormat::RGBA32;
            opaquePosition->InitializeData = false;
            opaquePosition->EnableRenderTarget = true;
            opaquePosition->SampleCount = sampleCount;
            opaquePosition->SetResizable(false);
            opaquePosition->Init();

            sTexture* opaqueNormal = new sTexture();
            opaqueNormal->Width = viewport.Width;
            opaqueNormal->Height = viewport.Height;
            opaqueNormal->Format = eTextureFormat::RGBA16;
            opaqueNormal->InitializeData = false;
            opaqueNormal->EnableRenderTarget = true;
            opaqueNormal->SampleCount = sampleCount;
            opaqueNormal->SetResizable(false);
            opaqueNormal->Init();

            RenderTargets::Opaque = new sRenderTarget({ opaqueColor, opaquePosition, opaqueNormal }, Textures::SharedDepth, &viewport);
            RenderTargets::Opaque->SetResizable(false);
            RenderTargets::Opaque->ClearColors.emplace_back(glm::vec4(0));
            RenderTargets::Opaque->ClearColors.emplace_back(glm::vec4(0));
            RenderTargets::Opaque->ClearColors.emplace_back(glm::vec4(0));
            RenderTargets::Opaque->ClearDepth = 1;

            // Transparent render target
            sTexture* transparentAccum = new sTexture();
            transparentAccum->Width = viewport.Width;
            transparentAccum->Height = viewport.Height;
            transparentAccum->Format = eTextureFormat::HDR;
            transparentAccum->InitializeData = false;
            transparentAccum->EnableRenderTarget = true;
            transparentAccum->SampleCount = sampleCount;
            transparentAccum->SetResizable(false);
            transparentAccum->Init();

            sTexture* transparentReveal = new sTexture();
            transparentReveal->Width = viewport.Width;
            transparentReveal->Height = viewport.Height;
            transparentReveal->Format = eTextureFormat::R8;
            transparentReveal->InitializeData = false;
            transparentReveal->EnableRenderTarget = true;
            transparentReveal->SampleCount = sampleCount;
            transparentReveal->SetResizable(false);
            transparentReveal->Init();

            RenderTargets::Transparent = new sRenderTarget({transparentAccum, transparentReveal }, Textures::SharedDepth, &viewport);
            RenderTargets::Transparent->SetResizable(false);
            RenderTargets::Transparent->ClearColors.emplace_back(glm::vec4(0.0f));
            RenderTargets::Transparent->ClearColors.emplace_back(glm::vec4(1.0f));

            // Scene render target
            sTexture* sceneColor = new sTexture();
            sceneColor->Width = viewport.Width;
            sceneColor->Height = viewport.Height;
            sceneColor->Format = eTextureFormat::RGBA8;
            sceneColor->InitializeData = false;
            sceneColor->EnableRenderTarget = true;
            sceneColor->SetResizable(true);
            sceneColor->Init();

            sTexture* sceneDepth = new sTexture();
            sceneDepth->Type = sTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            sceneDepth->Width = viewport.Width;
            sceneDepth->Height = viewport.Height;
            sceneDepth->Format = eTextureFormat::R32_TYPELESS;
            sceneDepth->InitializeData = false;
            sceneDepth->EnableRenderTarget = true;
            sceneDepth->SetResizable(true);
            sceneDepth->Init();

            RenderTargets::Scene = new sRenderTarget({ sceneColor }, sceneDepth, &viewport);
            RenderTargets::Scene->SetResizable(true);
            RenderTargets::Scene->ClearColors.emplace_back(glm::vec4(0.0f));
            RenderTargets::Scene->ClearDepth = 1.0f;

            // UI render target
            sTexture* uiColor = new sTexture();
            uiColor->Width = viewport.Width;
            uiColor->Height = viewport.Height;
            uiColor->Format = eTextureFormat::RGBA8;
            uiColor->InitializeData = false;
            uiColor->EnableRenderTarget = true;
            uiColor->SetResizable(true);
            uiColor->Init();

            sTexture* uiDepth = new sTexture();
            uiDepth->Type = sTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            uiDepth->Width = viewport.Width;
            uiDepth->Height = viewport.Height;
            uiDepth->Format = eTextureFormat::R32_TYPELESS;
            uiDepth->InitializeData = false;
            uiDepth->EnableRenderTarget = true;
            uiDepth->SetResizable(true);
            uiDepth->Init();

            RenderTargets::UI = new sRenderTarget({ uiColor }, uiDepth, &viewport);
            RenderTargets::UI->SetResizable(true);
            RenderTargets::UI->ClearColors.emplace_back(glm::vec4(0.0f));
            RenderTargets::UI->ClearDepth = 1.0f;

            // Final render target
            sTexture* finalColor = new sTexture();
            finalColor->Width = viewport.Width;
            finalColor->Height = viewport.Height;
            finalColor->Format = eTextureFormat::RGBA8;
            finalColor->InitializeData = false;
            finalColor->EnableRenderTarget = true;
            finalColor->SetResizable(true);
            finalColor->Init();

            sTexture* finalDepth = new sTexture();
            finalDepth->Type = sTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            finalDepth->Width = viewport.Width;
            finalDepth->Height = viewport.Height;
            finalDepth->Format = eTextureFormat::R32_TYPELESS;
            finalDepth->InitializeData = false;
            finalDepth->EnableRenderTarget = true;
            finalDepth->SetResizable(true);
            finalDepth->Init();

            RenderTargets::Final = new sRenderTarget({ finalColor }, finalDepth, &viewport);
            RenderTargets::Final->SetResizable(true);
            RenderTargets::Final->ClearColors.emplace_back(glm::vec4(0.0f));
            RenderTargets::Final->ClearDepth = 1.0f;

            Textures::Canvas = RenderTargets::Final->Colors[0];
        }

        void cRenderSystem::FreeManagers()
        {
            cWidgetManager::Free();
            cShadowManager::Free();
            cSkyboxManager::Free();
            cSkeletonManager::Free();
            cMaterialManager::Free();
            cGeometryManager::Free();
            cCameraManager::Free();
            cShaderManager::Free();
        }

        void cRenderSystem::FreeBuffers()
        {
            delete Buffers::DirectLight;
            delete Buffers::PointLight;
            delete Buffers::SpotLight;
        }

        void cRenderSystem::FreeRenderTargets()
        {
            delete RenderTargets::Scene;
            delete RenderTargets::Opaque;
            delete RenderTargets::Transparent;
            delete RenderTargets::UI;
            delete RenderTargets::Final;
        }

        void cRenderSystem::WindowFrameResized(int width, int height)
        {
            sViewport& viewport = *cCameraManager::GetViewport();
            viewport.Width = width;
            viewport.Height = height;
            cCameraManager::Flush();

            Textures::SharedDepth->Resize(width, height);

            RenderTargets::Opaque->DepthStencil = Textures::SharedDepth;
            RenderTargets::Opaque->ResizeColors(width, height);
            RenderTargets::Opaque->Resize(width, height);

            RenderTargets::Transparent->DepthStencil = Textures::SharedDepth;
            RenderTargets::Transparent->ResizeColors(width, height);
            RenderTargets::Transparent->Resize(width, height);
        }

        void cRenderSystem::Prepare()
        {
            context::BindVertexBuffer(*Buffers::Vertex);
            context::BindIndexBuffer(*Buffers::Index);
        }

        void cRenderSystem::Update(cScene* scene, const cTime& dt)
        {
            cSkeletonManager::Flush();
            UpdateLight(scene);
            UpdateShaders(scene);
        }

        void cRenderSystem::UpdateLight(cScene* scene)
        {
            Buffers::DirectLight->Clear();
            {
                auto components = scene->GetComponents<CDirectionalLight>();
                for (auto [entity, light] : components.each())
                {
                    sDirectLightData lightData;
                    lightData.Position = light.View.Position;
                    lightData.Color = light.Color;
                    lightData.ViewProjection = cMathManager::UpdateLightMatrix(light.Projection, light.View);
                    Buffers::DirectLight->Add(lightData);
                }
            }
            Buffers::DirectLight->Flush();

            Buffers::SpotLight->Clear();
            {
                auto components = scene->GetComponents<CSpotLight>();
                for (auto [entity, light] : components.each())
                {
                    sSpotLightData lightData;
                    lightData.Position = light.View.Position;
                    lightData.Color = light.Color;
                    lightData.Direction = light.View.Front;
                    lightData.Outer = light.Outer;
                    lightData.Cutoff = light.Cutoff;
                    lightData.ViewProjection = cMathManager::UpdateLightMatrix(light.Projection, light.View);
                    lightData.Near = light.Projection.Near;
                    lightData.Far = light.Projection.Far;

                    Buffers::SpotLight->Add(lightData);
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
                    lightData.Color = light.Color;
                    lightData.Constant = light.Constant;
                    lightData.Linear = light.Linear;
                    lightData.Quadratic = light.Quadratic;
                    Buffers::PointLight->Add(lightData);
                }
            }
            Buffers::PointLight->Flush();
        }

        void cRenderSystem::UpdateShaders(cScene* scene)
        {
            // Compute
            {
                cShader* shader = cShaderManager::GetShaders(cShader::eCategory::COMPUTE);
                while (shader != nullptr) {
                    if (shader->Enable) {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                    shader = shader->Next;
                }
            }

            // Prepass
            RenderTargets::Shadow->Clear();
            {
                cShader* shader = cShaderManager::GetShaders(cShader::eCategory::PREPASS);
                while (shader != nullptr) {
                    if (shader->Enable) {
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
                cShader* shader = cShaderManager::GetShaders(cShader::eCategory::OPAQUE);
                while (shader != nullptr) {
                    if (shader->Enable) {
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
                cShader* shader = cShaderManager::GetShaders(cShader::eCategory::TRANSPARENT);
                while (shader != nullptr) {
                    if (shader->Enable) {
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
                cShader* shader = cShaderManager::GetShaders(cShader::eCategory::POSTFX);
                while (shader != nullptr) {
                    if (shader->Enable) {
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
                cShader* shader = cShaderManager::GetShaders(cShader::eCategory::UI);
                while (shader != nullptr) {
                    if (shader->Enable) {
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
                cShader* shader = cShaderManager::GetShaders(cShader::eCategory::FINAL);
                while (shader != nullptr) {
                    if (shader->Enable) {
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