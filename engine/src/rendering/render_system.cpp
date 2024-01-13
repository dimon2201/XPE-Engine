#include <rendering/render_system.hpp>
#include <rendering/material/material_manager.hpp>
#include <rendering/geometry/geometry_manager.hpp>
#include <rendering/skybox_manager.hpp>
#include <rendering/camera_manager.hpp>
#include <rendering/light_manager.hpp>
#include <rendering/core/shader.hpp>

#include <anim/skeleton_manager.hpp>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        cRenderSystem::cRenderSystem(const glm::vec2& windowSize, u32 sampleCount)
        {
            context::Init();
            InitManagers(windowSize, sampleCount);
            InitBuffers(windowSize, sampleCount);
            InitRenderTargets(windowSize, sampleCount);
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

        void cRenderSystem::InitManagers(const glm::vec2& windowSize, u32 sampleCount)
        {
            cShaderManager::Init();
            cCameraManager::Init();
            cGeometryManager::Init();
            cMaterialManager::Init();
            cSkeletonManager::Init();
            cSkyboxManager::Init();
            cLightManager::Init(4, glm::vec2(4096));
            cWidgetManager::Init();
        }

        void cRenderSystem::InitBuffers(const glm::vec2& windowSize, u32 sampleCount)
        {
            Buffers::PointLight = new cPointLightBuffer(4);
            Buffers::SpotLight = new cSpotLightBuffer(4);
        }

        void cRenderSystem::InitRenderTargets(const glm::vec2& windowSize, u32 sampleCount)
        {
            Viewports::Main = new sViewport(glm::vec4(0.0f, 0.0f, windowSize.x, windowSize.y), glm::vec2(0.0f, 1.0f));

            // Shared depth texture for opaque and transparent render targets
            Textures::SharedDepth = new cTexture(
                cTexture::eType::TEXTURE_2D_DEPTH_STENCIL,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(windowSize.x, windowSize.y, 1.0f),
                1,
                eTextureFormat::R32_TYPELESS,
                sampleCount,
                true,
                0,
                0,
                false,
                {}
            );
            Textures::SharedDepth->SetResizable(false);

            // Opaque render target
            cTexture* opaqueColor = new cTexture(
                cTexture::eType::TEXTURE_2D,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(windowSize.x, windowSize.y, 1.0f),
                4,
                eTextureFormat::HDR,
                sampleCount,
                true,
                0,
                0,
                false,
                {}
            );
            opaqueColor->SetResizable(false);

            cTexture* opaquePosition = new cTexture(
                cTexture::eType::TEXTURE_2D,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(windowSize.x, windowSize.y, 1.0f),
                4,
                eTextureFormat::RGBA32,
                sampleCount,
                true,
                0,
                0,
                false,
                {}
            );
            opaquePosition->SetResizable(false);

            cTexture* opaqueNormal = new cTexture(
                cTexture::eType::TEXTURE_2D,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(windowSize.x, windowSize.y, 1.0f),
                4,
                eTextureFormat::RGBA16,
                sampleCount,
                true,
                0,
                0,
                false,
                {}
            );
            opaqueNormal->SetResizable(false);

            RenderTargets::Opaque = new cRenderTarget({ opaqueColor, opaquePosition, opaqueNormal }, Textures::SharedDepth);
            RenderTargets::Opaque->SetResizable(false);
            RenderTargets::Opaque->GetClearColors().emplace_back(glm::vec4(0));
            RenderTargets::Opaque->GetClearColors().emplace_back(glm::vec4(0));
            RenderTargets::Opaque->GetClearColors().emplace_back(glm::vec4(0));
            RenderTargets::Opaque->SetClearDepth(1);

            // Transparent render target
            cTexture* transparentAccum = new cTexture(
                cTexture::eType::TEXTURE_2D,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(windowSize.x, windowSize.y, 1.0f),
                4,
                eTextureFormat::HDR,
                sampleCount,
                true,
                0,
                0,
                false,
                {}
            );
            transparentAccum->SetResizable(false);

            cTexture* transparentReveal = new cTexture(
                cTexture::eType::TEXTURE_2D,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(windowSize.x, windowSize.y, 1.0f),
                1,
                eTextureFormat::R8,
                sampleCount,
                true,
                0,
                0,
                false,
                {}
            );
            transparentAccum->SetResizable(false);
            transparentReveal->SetResizable(false);

            RenderTargets::Transparent = new cRenderTarget({transparentAccum, transparentReveal }, Textures::SharedDepth);
            RenderTargets::Transparent->SetResizable(false);
            RenderTargets::Transparent->GetClearColors().emplace_back(glm::vec4(0.0f));
            RenderTargets::Transparent->GetClearColors().emplace_back(glm::vec4(1.0f));

            // Scene render target
            cTexture* sceneColor = new cTexture(
                cTexture::eType::TEXTURE_2D,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(windowSize.x, windowSize.y, 1.0f),
                4,
                eTextureFormat::RGBA8,
                1,
                true,
                0,
                0,
                false,
                {}
            );
            sceneColor->SetResizable(true);

            cTexture* sceneDepth = new cTexture(
                cTexture::eType::TEXTURE_2D_DEPTH_STENCIL,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(windowSize.x, windowSize.y, 1.0f),
                1,
                eTextureFormat::R32_TYPELESS,
                1,
                true,
                0,
                0,
                false,
                {}
            );
            sceneDepth->SetResizable(true);

            RenderTargets::Scene = new cRenderTarget({ sceneColor }, sceneDepth);
            RenderTargets::Scene->SetResizable(true);
            RenderTargets::Scene->GetClearColors().emplace_back(glm::vec4(0.0f));
            RenderTargets::Scene->SetClearDepth(1.0f);

            // UI render target
            cTexture* uiColor = new cTexture(
                cTexture::eType::TEXTURE_2D,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(windowSize.x, windowSize.y, 1.0f),
                4,
                eTextureFormat::RGBA8,
                1,
                true,
                0,
                0,
                false,
                {}
            );
            uiColor->SetResizable(true);

            cTexture* uiDepth = new cTexture(
                cTexture::eType::TEXTURE_2D_DEPTH_STENCIL,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(windowSize.x, windowSize.y, 1.0f),
                1,
                eTextureFormat::R32_TYPELESS,
                1,
                true,
                0,
                0,
                false,
                {}
            );
            uiDepth->SetResizable(true);

            RenderTargets::UI = new cRenderTarget({ uiColor }, uiDepth);
            RenderTargets::UI->SetResizable(true);
            RenderTargets::UI->GetClearColors().emplace_back(glm::vec4(0.0f));
            RenderTargets::UI->SetClearDepth(1.0f);

            // Final render target
            cTexture* finalColor = new cTexture(
                cTexture::eType::TEXTURE_2D,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(windowSize.x, windowSize.y, 1.0f),
                4,
                eTextureFormat::RGBA8,
                1,
                true,
                0,
                0,
                false,
                {}
            );
            finalColor->SetResizable(true);

            cTexture* finalDepth = new cTexture(
                cTexture::eType::TEXTURE_2D_DEPTH_STENCIL,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(windowSize.x, windowSize.y, 1.0f),
                1,
                eTextureFormat::R32_TYPELESS,
                1,
                true,
                0,
                0,
                false,
                {}
            );
            finalDepth->SetResizable(true);

            RenderTargets::Final = new cRenderTarget({ finalColor }, finalDepth);
            RenderTargets::Final->SetResizable(true);
            RenderTargets::Final->GetClearColors().emplace_back(glm::vec4(0.0f));
            RenderTargets::Final->SetClearDepth(1.0f);

            Textures::Canvas = RenderTargets::Final->GetColors()[0];
        }

        void cRenderSystem::FreeManagers()
        {
            cWidgetManager::Free();
            cLightManager::Free();
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
            Viewports::Main->Width = width;
            Viewports::Main->Height = height;

            cCameraManager::Flush();

            Textures::SharedDepth->Resize(width, height);

            RenderTargets::Opaque->SetDepthStencil(Textures::SharedDepth);
            RenderTargets::Opaque->ResizeColors(width, height);
            RenderTargets::Opaque->Resize(width, height);

            RenderTargets::Transparent->SetDepthStencil(Textures::SharedDepth);
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
            UpdateShaders(scene);
        }

        void cRenderSystem::UpdateShaders(cScene* scene)
        {
            // Prepass
            RenderTargets::Shadow->Clear();
            {
                auto shaders = cShaderManager::GetShaders(cShader::eCategory::PREPASS);
                if (shaders == nullptr) {
                    LogError("GetShaders() returned NULL for PREPASS category!");
                }

                for (auto shader : *shaders)
                {
                    if (shader != nullptr)
                    {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                }
            }

            // Opaque
            RenderTargets::Opaque->Clear();
            {
                auto shaders = cShaderManager::GetShaders(cShader::eCategory::OPAQUE_GEOMETRY);
                if (shaders == nullptr) {
                    LogError("GetShaders() returned NULL for OPAQUE_GEOMETRY category!");
                }

                for (auto shader : *shaders)
                {
                    if (shader != nullptr)
                    {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                }
            }

            // Transparent
            RenderTargets::Transparent->Clear();
            {
                auto shaders = cShaderManager::GetShaders(cShader::eCategory::TRANSPARENT_GEOMETRY);
                if (shaders == nullptr) {
                    LogError("GetShaders() returned NULL for TRANSPARENT_GEOMETRY category!");
                }

                for (auto shader : *shaders)
                {
                    if (shader != nullptr)
                    {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                }
            }

            // PostFX
            RenderTargets::Scene->Clear();
            {
                auto shaders = cShaderManager::GetShaders(cShader::eCategory::POSTFX);
                if (shaders == nullptr) {
                    LogError("GetShaders() returned NULL for POSTFX category!");
                }

                for (auto shader : *shaders)
                {
                    if (shader != nullptr)
                    {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                }
            }

            // UI
            RenderTargets::UI->Clear();
            {
                auto shaders = cShaderManager::GetShaders(cShader::eCategory::UI);
                if (shaders == nullptr) {
                    LogError("GetShaders() returned NULL for UI category!");
                }

                for (auto shader : *shaders)
                {
                    if (shader != nullptr)
                    {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                }
            }

            // Final
            RenderTargets::Final->Clear();
            {
                auto shaders = cShaderManager::GetShaders(cShader::eCategory::FINAL);
                if (shaders == nullptr) {
                    LogError("GetShaders() returned NULL for FINAL category!");
                }

                for (auto shader : *shaders)
                {
                    if (shader != nullptr)
                    {
                        shader->Bind();
                        shader->Draw(scene);
                        shader->Unbind();
                    }
                }
            }
        }

    }

}