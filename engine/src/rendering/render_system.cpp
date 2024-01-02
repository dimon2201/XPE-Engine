#include <rendering/render_system.hpp>
#include <rendering/material/material_manager.hpp>
#include <rendering/geometry/geometry_manager.hpp>
#include <rendering/skybox_manager.hpp>
#include <rendering/camera_manager.hpp>
#include <rendering/shadow_manager.hpp>
#include <rendering/core/shader.hpp>

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
            Buffers::DirectLight = new cDirectLightBuffer(4);
            Buffers::PointLight = new cPointLightBuffer(4);
            Buffers::SpotLight = new cSpotLightBuffer(4);
        }

        void cRenderSystem::InitRenderTargets(sViewport& viewport, u32 sampleCount)
        {
            // Shared depth texture for opaque and transparent render targets
            Textures::SharedDepth = new cTexture();
            Textures::SharedDepth->SetType(cTexture::eType::TEXTURE_2D_DEPTH_STENCIL);
            Textures::SharedDepth->SetWidth(viewport.Width);
            Textures::SharedDepth->SetHeight(viewport.Height);
            Textures::SharedDepth->SetFormat(eTextureFormat::R32_TYPELESS);
            Textures::SharedDepth->SetInitializeData(false);
            Textures::SharedDepth->SetEnableRenderTarget(true);
            Textures::SharedDepth->SetSampleCount(sampleCount);
            Textures::SharedDepth->SetResizable(false);
            Textures::SharedDepth->Init();

            // Opaque render target
            cTexture* opaqueColor = new cTexture();
            opaqueColor->SetWidth(viewport.Width);
            opaqueColor->SetHeight(viewport.Height);
            opaqueColor->SetFormat(eTextureFormat::HDR);
            opaqueColor->SetInitializeData(false);
            opaqueColor->SetEnableRenderTarget(true);
            opaqueColor->SetSampleCount(sampleCount);
            opaqueColor->SetResizable(false);
            opaqueColor->Init();

            cTexture* opaquePosition = new cTexture();
            opaquePosition->SetWidth(viewport.Width);
            opaquePosition->SetHeight(viewport.Height);
            opaquePosition->SetFormat(eTextureFormat::RGBA32);
            opaquePosition->SetInitializeData(false);
            opaquePosition->SetEnableRenderTarget(true);
            opaquePosition->SetSampleCount(sampleCount);
            opaquePosition->SetResizable(false);
            opaquePosition->Init();

            cTexture* opaqueNormal = new cTexture();
            opaqueNormal->SetWidth(viewport.Width);
            opaqueNormal->SetHeight(viewport.Height);
            opaqueNormal->SetFormat(eTextureFormat::RGBA16);
            opaqueNormal->SetInitializeData(false);
            opaqueNormal->SetEnableRenderTarget(true);
            opaqueNormal->SetSampleCount(sampleCount);
            opaqueNormal->SetResizable(false);
            opaqueNormal->Init();

            RenderTargets::Opaque = new sRenderTarget({ opaqueColor, opaquePosition, opaqueNormal }, Textures::SharedDepth, &viewport);
            RenderTargets::Opaque->SetResizable(false);
            RenderTargets::Opaque->ClearColors.emplace_back(glm::vec4(0));
            RenderTargets::Opaque->ClearColors.emplace_back(glm::vec4(0));
            RenderTargets::Opaque->ClearColors.emplace_back(glm::vec4(0));
            RenderTargets::Opaque->ClearDepth = 1;

            // Transparent render target
            cTexture* transparentAccum = new cTexture();
            transparentAccum->SetWidth(viewport.Width);
            transparentAccum->SetHeight(viewport.Height);
            transparentAccum->SetFormat(eTextureFormat::HDR);
            transparentAccum->SetInitializeData(false);
            transparentAccum->SetEnableRenderTarget(true);
            transparentAccum->SetSampleCount(sampleCount);
            transparentAccum->SetResizable(false);
            transparentAccum->Init();

            cTexture* transparentReveal = new cTexture();
            transparentReveal->SetWidth(viewport.Width);
            transparentReveal->SetHeight(viewport.Height);
            transparentReveal->SetFormat(eTextureFormat::R8);
            transparentReveal->SetInitializeData(false);
            transparentReveal->SetEnableRenderTarget(true);
            transparentReveal->SetSampleCount(sampleCount);
            transparentReveal->SetResizable(false);
            transparentReveal->Init();

            RenderTargets::Transparent = new sRenderTarget({transparentAccum, transparentReveal }, Textures::SharedDepth, &viewport);
            RenderTargets::Transparent->SetResizable(false);
            RenderTargets::Transparent->ClearColors.emplace_back(glm::vec4(0.0f));
            RenderTargets::Transparent->ClearColors.emplace_back(glm::vec4(1.0f));

            // Scene render target
            cTexture* sceneColor = new cTexture();
            sceneColor->SetWidth(viewport.Width);
            sceneColor->SetHeight(viewport.Height);
            sceneColor->SetFormat(eTextureFormat::RGBA8);
            sceneColor->SetInitializeData(false);
            sceneColor->SetEnableRenderTarget(true);
            sceneColor->SetResizable(true);
            sceneColor->Init();

            cTexture* sceneDepth = new cTexture();
            sceneDepth->SetType(cTexture::eType::TEXTURE_2D_DEPTH_STENCIL);
            sceneDepth->SetWidth(viewport.Width);
            sceneDepth->SetHeight(viewport.Height);
            sceneDepth->SetFormat(eTextureFormat::R32_TYPELESS);
            sceneDepth->SetInitializeData(false);
            sceneDepth->SetEnableRenderTarget(true);
            sceneDepth->SetResizable(true);
            sceneDepth->Init();

            RenderTargets::Scene = new sRenderTarget({ sceneColor }, sceneDepth, &viewport);
            RenderTargets::Scene->SetResizable(true);
            RenderTargets::Scene->ClearColors.emplace_back(glm::vec4(0.0f));
            RenderTargets::Scene->ClearDepth = 1.0f;

            // UI render target
            cTexture* uiColor = new cTexture();
            uiColor->SetWidth(viewport.Width);
            uiColor->SetHeight(viewport.Height);
            uiColor->SetFormat(eTextureFormat::RGBA8);
            uiColor->SetInitializeData(false);
            uiColor->SetEnableRenderTarget(true);
            uiColor->SetResizable(true);
            uiColor->Init();

            cTexture* uiDepth = new cTexture();
            uiDepth->SetType(cTexture::eType::TEXTURE_2D_DEPTH_STENCIL);
            uiDepth->SetWidth(viewport.Width);
            uiDepth->SetHeight(viewport.Height);
            uiDepth->SetFormat(eTextureFormat::R32_TYPELESS);
            uiDepth->SetInitializeData(false);
            uiDepth->SetEnableRenderTarget(true);
            uiDepth->SetResizable(true);
            uiDepth->Init();

            RenderTargets::UI = new sRenderTarget({ uiColor }, uiDepth, &viewport);
            RenderTargets::UI->SetResizable(true);
            RenderTargets::UI->ClearColors.emplace_back(glm::vec4(0.0f));
            RenderTargets::UI->ClearDepth = 1.0f;

            // Final render target
            cTexture* finalColor = new cTexture();
            finalColor->SetWidth(viewport.Width);
            finalColor->SetHeight(viewport.Height);
            finalColor->SetFormat(eTextureFormat::RGBA8);
            finalColor->SetInitializeData(false);
            finalColor->SetEnableRenderTarget(true);
            finalColor->SetResizable(true);
            finalColor->Init();

            cTexture* finalDepth = new cTexture();
            finalDepth->SetType(cTexture::eType::TEXTURE_2D_DEPTH_STENCIL);
            finalDepth->SetWidth(viewport.Width);
            finalDepth->SetHeight(viewport.Height);
            finalDepth->SetFormat(eTextureFormat::R32_TYPELESS);
            finalDepth->SetInitializeData(false);
            finalDepth->SetEnableRenderTarget(true);
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
            {
                usize index = 0;
                auto components = scene->GetComponents<CDirectionalLight>();
                for (auto [entity, light] : components.each())
                {
                    sDirectLightData lightData;
                    lightData.Position = light.View.Position;
                    lightData.Color = light.Color;
                    lightData.ViewProjection = cMathManager::UpdateLightMatrix(light.Projection, light.View);
                    
                    Buffers::DirectLight->GetList()[index] = lightData;

                    index += 1;
                }

                Buffers::DirectLight->Flush();
            }

            {
                usize index = 0;
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

                    Buffers::SpotLight->GetList()[index] = lightData;

                    index += 1;
                }

                Buffers::SpotLight->Flush();
            }

            {
                usize index = 0;
                auto components = scene->GetComponents<CPointLight>();
                for (auto [entity, light] : components.each())
                {
                    sPointLightData lightData;
                    lightData.Position = light.Position;
                    lightData.Color = light.Color;
                    lightData.Constant = light.Constant;
                    lightData.Linear = light.Linear;
                    lightData.Quadratic = light.Quadratic;

                    Buffers::PointLight->GetList()[index] = lightData;

                    index += 1;
                }

                Buffers::PointLight->Flush();
            }
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
                cShader* shader = cShaderManager::GetShaders(cShader::eCategory::OPAQUE_GEOMETRY);
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
                cShader* shader = cShaderManager::GetShaders(cShader::eCategory::TRANSPARENT_GEOMETRY);
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