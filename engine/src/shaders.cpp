#include <shaders.hpp>
#include <camera_manager.hpp>

namespace xpe
{
    namespace render
    {
        cInstancingShader::cInstancingShader(eCategory category, const string& name)
        : cDefaultShader(category, name)
        {
            m_InstanceBuffer.Reserve(1000);
        }

        void cInstancingShader::DrawInstanced(cScene* scene, const CGeometryInfo& geometryInfo,
                                              u32 materialIndex, u32 skeletonIndex, u32 lightIndex)
        {
            usize entityCount = geometryInfo.Entities.size();
            usize instanceCount = 0;
            m_InstanceBuffer.Clear();
            for (usize i = 0 ; i < entityCount ; i++)
            {
                auto& entity = geometryInfo.Entities[i];

                if (!scene->HasAnyComponent<CVisible>(entity))
                {
                    continue;
                }

                sRenderInstance instance;
                instance.ModelMatrix = MMath::UpdateModelMatrix(scene->GetComponent<CTransform>(entity));
                instance.NormalMatrix = MMath::UpdateNormalMatrix(instance.NormalMatrix);
                instance.MaterialIndex = materialIndex;
                instance.SkeletonIndex = skeletonIndex;
                instance.LightIndex = lightIndex;

                m_InstanceBuffer.Add(instance);

                instanceCount++;
            }
            m_InstanceBuffer.Flush();

            context::BindPrimitiveTopology(geometryInfo.PrimitiveTopology);
            context::VSBindBuffer(m_InstanceBuffer);

            if (geometryInfo.IndexCount == 0) {
                context::DrawVertexed(geometryInfo.VertexCount, instanceCount, geometryInfo.VertexOffset);
            } else {
                context::DrawIndexed(geometryInfo.IndexCount, instanceCount, geometryInfo.VertexOffset, geometryInfo.IndexOffset);
            }

            context::VSUnbindBuffer(m_InstanceBuffer);
        }

        cOpaqueShader::cOpaqueShader(const string &name)
        : cInstancingShader(eCategory::OPAQUE, name)
        {
            VertexStage = MShader::GetFromFile(cShaderStage::eType::VERTEX, "res/shaders/geometry.vs");
            PixelStage =  MShader::GetFromFile(cShaderStage::eType::PIXEL, "res/shaders/pbr_opaque.ps");

            RenderTarget = RenderTargets::Opaque;

            VertexStage->SetBuffer(Buffers::Camera);
            VertexStage->SetBuffer(Buffers::Skeleton);

            PixelStage->SetBuffer(Buffers::Material);
            PixelStage->SetBuffer(Buffers::DirectLight);
            PixelStage->SetBuffer(Buffers::PointLight);
            PixelStage->SetBuffer(Buffers::SpotLight);
            PixelStage->SetBuffer(Buffers::ShadowPCF);

            PixelStage->SetTexture(Textures::AlbedoAtlas);
            PixelStage->SetTexture(Textures::NormalAtlas);
            PixelStage->SetTexture(Textures::ParallaxAtlas);
            PixelStage->SetTexture(Textures::MetalAtlas);
            PixelStage->SetTexture(Textures::RoughnessAtlas);
            PixelStage->SetTexture(Textures::AOAtlas);
            PixelStage->SetTexture(Textures::EmissionAtlas);
            PixelStage->SetTexture(RenderTargets::DirectionalShadow->Colors[0]);
            PixelStage->SetTexture(Textures::ShadowCircle);

            PixelStage->SetSampler(Samplers::Material);
            PixelStage->SetSampler(Samplers::Shadow);
            PixelStage->SetSampler(Samplers::ShadowCircle);

            Init();
        }

        void cOpaqueShader::Draw(cScene* scene)
        {
            // Draw geometry
            {
                auto components = scene->GetComponents<COpaque, CGeometryInfo, CMaterialInfo>();
                for (auto [entity, opaque, geometryInfo, materialInfo]: components.each())
                {
                    DrawInstanced(
                            scene,
                            geometryInfo,
                            materialInfo.MaterialIndex,
                            0,
                            0
                    );
                }
            }
            // Draw skeletons
            {
                auto components = scene->GetComponents<COpaque, CGeometryInfo, CMaterialInfo, CSkeletonInfo>();
                for (auto [entity, opaque, geometryInfo, materialInfo, skeletonInfo]: components.each())
                {
                    DrawInstanced(
                            scene,
                            geometryInfo,
                            materialInfo.MaterialIndex,
                            skeletonInfo.SkeletonIndex,
                            0
                    );
                }
            }
        }

        cTransparentShader::cTransparentShader(const string &name)
        : cInstancingShader(eCategory::TRANSPARENT, name)
        {
            VertexStage = MShader::GetFromFile(cShaderStage::eType::VERTEX, "res/shaders/geometry.vs");
            PixelStage =  MShader::GetFromFile(cShaderStage::eType::PIXEL, "res/shaders/pbr_transparent.ps");

            RenderTarget = RenderTargets::Transparent;

            VertexStage->SetBuffer(Buffers::Camera);
            VertexStage->SetBuffer(Buffers::Skeleton);

            PixelStage->SetBuffer(Buffers::Material);
            PixelStage->SetBuffer(Buffers::DirectLight);
            PixelStage->SetBuffer(Buffers::PointLight);
            PixelStage->SetBuffer(Buffers::SpotLight);
            PixelStage->SetBuffer(Buffers::ShadowPCF);

            PixelStage->SetTexture(Textures::AlbedoAtlas);
            PixelStage->SetTexture(Textures::NormalAtlas);
            PixelStage->SetTexture(Textures::ParallaxAtlas);
            PixelStage->SetTexture(Textures::MetalAtlas);
            PixelStage->SetTexture(Textures::RoughnessAtlas);
            PixelStage->SetTexture(Textures::AOAtlas);
            PixelStage->SetTexture(Textures::EmissionAtlas);
            PixelStage->SetTexture(RenderTargets::DirectionalShadow->Colors[0]);
            PixelStage->SetTexture(Textures::ShadowCircle);

            PixelStage->SetSampler(Samplers::Material);
            PixelStage->SetSampler(Samplers::Shadow);
            PixelStage->SetSampler(Samplers::ShadowCircle);

            Init();
        }

        void cTransparentShader::Draw(ecs::cScene* scene)
        {
            // Draw geometry
            {
                auto components = scene->GetComponents<CTransparent, CGeometryInfo, CMaterialInfo>();
                for (auto [entity, transparent, geometryInfo, materialInfo]: components.each())
                {
                    DrawInstanced(
                            scene,
                            geometryInfo,
                            materialInfo.MaterialIndex,
                            0,
                            0
                    );
                }
            }
            // Draw skeletons
            {
                auto components = scene->GetComponents<CTransparent, CGeometryInfo, CMaterialInfo, CSkeletonInfo>();
                for (auto [entity, transparent, geometryInfo, materialInfo, skeletonInfo]: components.each())
                {
                    DrawInstanced(
                            scene,
                            geometryInfo,
                            materialInfo.MaterialIndex,
                            skeletonInfo.SkeletonIndex,
                            0
                    );
                }
            }
        }

        cShadowShader::cShadowShader(const string &name)
        : cInstancingShader(eCategory::PREPASS, name)
        {
        }

        void cShadowShader::InitPrepass()
        {
            DepthStencilMode.DepthWriteMask = eDepthWriteMask::ALL;

            sBlendTarget target;
            target.Enable = false;
            BlendMode.Targets.push_back(target);
            BlendMode.IndependentBlendEnable = true;

            Rasterizer.CullMode = eCullMode::FRONT;
        }

        void cShadowShader::DrawShadow(cScene *scene, u32 lightIndex)
        {
            // Draw geometry
            {
                auto components = scene->GetComponents<CShadow, CGeometryInfo>();
                for (auto [entity, hasShadow, geometryInfo]: components.each())
                {
                    DrawInstanced(
                            scene,
                            geometryInfo,
                            0,
                            0,
                            lightIndex
                    );
                }
            }
            // Draw skeletons
            {
                auto components = scene->GetComponents<CShadow, CGeometryInfo, CSkeletonInfo>();
                for (auto [entity, hasShadow, geometryInfo, skeletonInfo]: components.each())
                {
                    DrawInstanced(
                            scene,
                            geometryInfo,
                            0,
                            skeletonInfo.SkeletonIndex,
                            lightIndex
                    );
                }
            }
        }

        cDirectionalShadowShader::cDirectionalShadowShader(const string &name)
        : cShadowShader(name)
        {
            VertexStage = MShader::GetFromFile(cShaderStage::eType::VERTEX, "res/shaders/directional_shadow.vs");
            PixelStage = MShader::GetFromFile(cShaderStage::eType::PIXEL, "res/shaders/directional_shadow.ps");

            RenderTarget = RenderTargets::DirectionalShadow;

            VertexStage->SetBuffer(Buffers::Skeleton);
            VertexStage->SetBuffer(Buffers::DirectLight);

            Init();
        }

        void cDirectionalShadowShader::Draw(cScene* scene)
        {
            RenderTargets::DirectionalShadow->Clear();
            auto lights = scene->GetComponents<CDirectionalLight>();
            u32 lightIndex = 0;
            for (auto [lightEntity, light]: lights.each())
            {
                DrawShadow(scene, lightIndex);
                lightIndex++;
            }
        }

        cPointShadowShader::cPointShadowShader(const string &name)
        : cShadowShader(name)
        {
            VertexStage = MShader::GetFromFile(cShaderStage::eType::VERTEX, "res/shaders/point_shadow.vs");
            PixelStage = MShader::GetFromFile(cShaderStage::eType::PIXEL, "res/shaders/point_shadow.ps");

            RenderTarget = RenderTargets::DirectionalShadow;

            VertexStage->SetBuffer(Buffers::Skeleton);
            VertexStage->SetBuffer(Buffers::PointLight);

            Init();
        }

        void cPointShadowShader::Draw(cScene* scene)
        {
            auto lights = scene->GetComponents<CPointLight>();
            u32 lightIndex = 0;
            for (auto [lightEntity, light]: lights.each())
            {
                DrawShadow(scene, lightIndex);
                lightIndex++;
            }
        }

        cSpotShadowShader::cSpotShadowShader(const string &name)
        : cShadowShader(name)
        {
            VertexStage = MShader::GetFromFile(cShaderStage::eType::VERTEX, "res/shaders/spot_shadow.vs");
            PixelStage = MShader::GetFromFile(cShaderStage::eType::PIXEL, "res/shaders/spot_shadow.ps");

            RenderTarget = RenderTargets::DirectionalShadow;

            VertexStage->SetBuffer(Buffers::Skeleton);
            VertexStage->SetBuffer(Buffers::SpotLight);

            Init();
        }

        void cSpotShadowShader::Draw(cScene* scene)
        {
            auto lights = scene->GetComponents<CSpotLight>();
            u32 lightIndex = 0;
            for (auto [lightEntity, light]: lights.each())
            {
                DrawShadow(scene, lightIndex);
                lightIndex++;
            }
        }

        cSkyboxShader::cSkyboxShader(const string& name)
        : cDefaultShader(eCategory::OPAQUE, name)
        {
            Samplers::Skybox = new cSampler();
            Samplers::Skybox->Filter = cSampler::eFilter::MIN_MAG_MIP_LINEAR;
            Samplers::Skybox->Init();

            VertexStage = MShader::GetFromFile(cShaderStage::eType::VERTEX, "res/shaders/skybox.vs");
            PixelStage = MShader::GetFromFile(cShaderStage::eType::PIXEL, "res/shaders/skybox.ps");

            RenderTarget = RenderTargets::Opaque;

            VertexStage->SetBuffer(Buffers::Camera);

            PixelStage->SetSampler(Samplers::Skybox);

            Init();
        }

        cSkyboxShader::~cSkyboxShader()
        {
            delete Samplers::Skybox;
        }

        void cSkyboxShader::InitOpaque()
        {
            cDefaultShader::InitOpaque();
            DepthStencilMode.EnableDepth = true;
            DepthStencilMode.DepthFunc = eDepthStencilFunc::LESS_EQUAL;
            Rasterizer.CullMode = eCullMode::NONE;
        }

        void cSkyboxShader::Draw(cScene* scene)
        {
            if (scene->Skybox != nullptr)
            {
                sSkybox* skybox = scene->Skybox;
                if (skybox->Texture != nullptr)
                {
                    CGeometryInfo geometryInfo = skybox->GeometryInfo;
                    cTexture& skyboxTexture = *skybox->Texture;
                    context::BindPrimitiveTopology(geometryInfo.PrimitiveTopology);
                    context::PSBindTexture(skyboxTexture);
                    context::DrawIndexed(
                            geometryInfo.IndexCount,
                            1,
                            geometryInfo.VertexOffset,
                            geometryInfo.IndexOffset
                    );
                }
            }
        }

        cCompositeTransparentShader::cCompositeTransparentShader(const string& name, u32 sampleCount)
        : cDefaultShader(eCategory::POSTFX, name)
        {
            VertexStage = MShader::GetFromFile(cShaderStage::eType::VERTEX, "res/shaders/screen.vs");
            PixelStage = MShader::GetFromFile(cShaderStage::eType::PIXEL, sampleCount > 1 ? "res/shaders/msaa_composite_transparent.ps" : "res/shaders/composite_transparent.ps");

            RenderTarget = RenderTargets::Opaque;

            PixelStage->SetTexture(RenderTargets::Transparent->Colors[0], 0);
            PixelStage->SetTexture(RenderTargets::Transparent->Colors[1], 1);

            Init();
        }

        void cCompositeTransparentShader::InitPostFX()
        {
            DepthStencilMode.DepthWriteMask = eDepthWriteMask::ZERO;

            sBlendTarget target;
            target.Src = eBlend::SRC_ALPHA;
            target.Dest = eBlend::INV_SRC_ALPHA;
            target.BlendOp = eBlendOp::ADD;
            target.SrcAlpha = eBlend::SRC_ALPHA;
            target.DestAlpha = eBlend::INV_SRC_ALPHA;
            target.BlendOpAlpha = eBlendOp::ADD;

            target.Enable = true;
            BlendMode.Targets.push_back(target);

            target.Enable = false;
            BlendMode.Targets.push_back(target);

            target.Enable = false;
            BlendMode.Targets.push_back(target);

            BlendMode.IndependentBlendEnable = true;
        }

        void cCompositeTransparentShader::Draw(ecs::cScene *scene)
        {
            context::DrawQuad();
        }

        cSsaoShader::cSsaoShader(const string& name, u32 sampleCount)
        : cDefaultShader(eCategory::POSTFX, name)
        {
            Buffers::SSAO = new sSsaoBuffer();

            Viewports::SSAO = new sViewport();
            Viewports::SSAO->Width = MWindow::GetFrameWidth();
            Viewports::SSAO->Height = MWindow::GetFrameHeight();

            cTexture* ssaoColor = new cTexture();
            ssaoColor->Width = Viewports::SSAO->Width;
            ssaoColor->Height = Viewports::SSAO->Height;
            ssaoColor->Format = eTextureFormat::RGBA8;
            ssaoColor->SampleCount = sampleCount;
            ssaoColor->InitializeData = false;
            ssaoColor->EnableRenderTarget = true;
            ssaoColor->SetResizable(true);
            ssaoColor->Init();

            cTexture* ssaoDepth = new cTexture();
            ssaoDepth->Type = cTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            ssaoDepth->Width = Viewports::SSAO->Width;
            ssaoDepth->Height = Viewports::SSAO->Height;
            ssaoDepth->Format = eTextureFormat::R32_TYPELESS;
            ssaoDepth->SampleCount = sampleCount;
            ssaoDepth->InitializeData = false;
            ssaoDepth->EnableRenderTarget = true;
            ssaoDepth->SetResizable(true);
            ssaoDepth->Init();

            RenderTargets::SSAO = new cRenderTarget({ ssaoColor }, ssaoDepth, Viewports::SSAO);
            RenderTargets::SSAO->SetResizable(true);
            RenderTargets::SSAO->ClearColors.emplace_back(glm::vec4(1.0f));
            RenderTargets::SSAO->ClearDepth = 1.0f;

            RenderTarget = RenderTargets::SSAO;

            VertexStage = MShader::GetFromFile(cShaderStage::eType::VERTEX, "res/shaders/screen.vs");
            PixelStage = MShader::GetFromFile(cShaderStage::eType::PIXEL, sampleCount > 1 ? "res/shaders/msaa_ssao.ps" : "res/shaders/ssao.ps");

            PixelStage->SetBuffer(Buffers::SSAO);
            PixelStage->SetTexture(RenderTargets::Opaque->Colors[1], 1);
            PixelStage->SetTexture(RenderTargets::Opaque->Colors[2], 2);
            PixelStage->SetTexture(RenderTargets::Opaque->DepthStencil, 3);

            Init();
        }

        cSsaoShader::~cSsaoShader()
        {
            delete Viewports::SSAO;
            delete RenderTargets::SSAO;
            delete Buffers::SSAO;
        }

        void cSsaoShader::Draw(ecs::cScene *scene)
        {
            context::DrawQuad();
        }

        cFinalShader::cFinalShader(const string &name, u32 sampleCount)
        : cDefaultShader(eCategory::FINAL, name)
        {
            VertexStage = MShader::GetFromFile(cShaderStage::eType::VERTEX, "res/shaders/screen.vs");
            PixelStage = MShader::GetFromFile(cShaderStage::eType::PIXEL, sampleCount > 1 ? "res/shaders/msaa_final.ps" : "res/shaders/final.ps");

            RenderTarget = RenderTargets::Final;

            PixelStage->SetBuffer(Buffers::Camera);
            PixelStage->SetTexture(RenderTargets::Opaque->Colors[0], 0);
            PixelStage->SetTexture(RenderTargets::SSAO->Colors[0], 1);
            PixelStage->SetTexture(RenderTargets::UI->Colors[0], 2);

            Init();
        }

        void cFinalShader::Draw(ecs::cScene *scene)
        {
            context::DrawQuad();
        }

        cWidgetShader::cWidgetShader(const xpe::core::string &name)
        : cDefaultShader(eCategory::UI, name)
        {
            m_Quad = sQuad();
            m_QuadInfo = MGeometry::Add(m_Quad);
            PrimitiveTopology = m_QuadInfo.PrimitiveTopology;
            m_WidgetBuffer.Reserve(100);

            RenderTarget = RenderTargets::UI;

            VertexStage = MShader::GetFromFile(cShaderStage::eType::VERTEX, "res/shaders/widget.vs");
            PixelStage = MShader::GetFromFile(cShaderStage::eType::PIXEL, "res/shaders/widget.ps");

            VertexStage->SetBuffer(&m_WidgetBuffer);

            PixelStage->SetTexture(Textures::WidgetAtlas);
            PixelStage->SetSampler(Samplers::Widget);

            Init();
        }

        void cWidgetShader::Draw(cScene* scene)
        {
            auto& camera = Buffers::Camera->Item;

            sOrthoMatrix orthoMatrix;
            orthoMatrix.Left = camera.Viewport.Left;
            orthoMatrix.Top = camera.Viewport.Top;
            orthoMatrix.Right = camera.Viewport.Width;
            orthoMatrix.Bottom = camera.Viewport.Height;
            orthoMatrix.Near = -1.0f;
            orthoMatrix.Far = 1.0f;
            m_Projection2D = MMath::UpdateOrthoMatrix(orthoMatrix);
            m_Projection3D = camera.View * camera.Projection;

            // button 2D
            {
                auto components = scene->GetComponents<CVisible, CSpace2D, CTransform, CButton>();
                for (auto [entity, visible, space, transform, button] : components.each())
                {
                    UpdateWidget2D(
                            m_Projection2D,
                            transform,
                            button.Color,
                            button.EnableTexture,
                            button.AtlasLocation,
                            button.FillFrame,
                            MInput::MousePressed(button.MousePressed) && button.EnablePress,
                            [&button](sWidgetData& widget)
                            {
                                widget.Color = button.ColorPressed;
                                if (button.Pressed != nullptr)
                                {
                                    button.Pressed();
                                }
                            },
                            [&button](sWidgetData& widget)
                            {
                                widget.Color = button.ColorHover;
                                if (button.Hovered != nullptr)
                                {
                                    button.Hovered();
                                }
                            }
                    );
                }
            }

            // button 3D
            {
                auto components = scene->GetComponents<CVisible, CSpace3D, CTransform, CButton>();
                for (auto [entity, visible, space, transform, button] : components.each())
                {
                    UpdateWidget2D(
                            m_Projection3D,
                            transform,
                            button.Color,
                            button.EnableTexture,
                            button.AtlasLocation,
                            button.FillFrame,
                            MInput::MousePressed(button.MousePressed) && button.EnablePress,
                            [&button](sWidgetData& widget)
                            {
                                widget.Color = button.ColorPressed;
                                if (button.Pressed != nullptr)
                                {
                                    button.Pressed();
                                }
                            },
                            [&button](sWidgetData& widget)
                            {
                                widget.Color = button.ColorHover;
                                if (button.Hovered != nullptr)
                                {
                                    button.Hovered();
                                }
                            }
                    );
                }
            }

            DrawWidgets();
        }

        void cWidgetShader::UpdateWidget2D(
                glm::mat4 projection,
                CTransform transform,
                const glm::vec4& color,
                bool enableTexture,
                const cAtlas::sLocation& atlasLocation,
                bool fillFrame,
                bool pressed,
                const std::function<void(sWidgetData&)>& pressedCallback,
                const std::function<void(sWidgetData&)>& hoveredCallback
        )
        {
            sWidgetData widget;
            widget.Projection = projection;
            widget.HasTexture = enableTexture;
            widget.AtlasUV[0] = atlasLocation.UV[0];
            widget.AtlasUV[1] = atlasLocation.UV[1];
            widget.AtlasUV[2] = atlasLocation.UV[2];
            widget.AtlasUV[3] = atlasLocation.UV[3];
            bool isHovered;
            glm::mat4 modelMatrix;

            float frameWidth = (float) MWindow::GetFrameWidth();
            float frameHeight = (float) MWindow::GetFrameHeight();

            transform.Position = {
                    transform.Position.x * frameWidth,
                    transform.Position.y * frameHeight,
                    transform.Position.z
            };

            if (fillFrame)
            {
                transform.Scale = {
                        transform.Scale.x * frameWidth,
                        transform.Scale.y * frameHeight,
                        transform.Scale.z
                };
            }

            modelMatrix = MMath::UpdateModelMatrix(transform);
            auto mousePos = MInput::GetMouseCursor().Position;
            // flip Y axis for mouse to match coordinate system with widget
            mousePos.y = (float) MWindow::GetFrameHeight() - mousePos.y;
            isHovered = IsHovered2D(modelMatrix, mousePos);

            widget.ModelMatrix = modelMatrix;

            if (isHovered)
            {
                hoveredCallback(widget);
                if (pressed)
                {
                    pressedCallback(widget);
                }
            }
            else
            {
                widget.Color = color;
            }

            m_WidgetBuffer.Add(widget);
        }

        void cWidgetShader::UpdateWidget3D(
                glm::mat4 projection,
                CTransform transform,
                const glm::vec4& color,
                bool enableTexture,
                const cAtlas::sLocation& atlasLocation,
                bool fillFrame,
                bool pressed,
                const std::function<void(sWidgetData&)>& pressedCallback,
                const std::function<void(sWidgetData&)>& hoveredCallback
        ) {
            sWidgetData widget;
            widget.Projection = projection;
            widget.HasTexture = enableTexture;
            widget.AtlasUV[0] = atlasLocation.UV[0];
            widget.AtlasUV[1] = atlasLocation.UV[1];
            widget.AtlasUV[2] = atlasLocation.UV[2];
            widget.AtlasUV[3] = atlasLocation.UV[3];
            bool isHovered;
            glm::mat4 modelMatrix;

            float frameWidth = (float) MWindow::GetFrameWidth();
            float frameHeight = (float) MWindow::GetFrameHeight();

            transform.Position = {
                    transform.Position.x * frameWidth,
                    transform.Position.y * frameHeight,
                    transform.Position.z
            };

            if (fillFrame)
            {
                transform.Scale = {
                        transform.Scale.x * frameWidth,
                        transform.Scale.y * frameHeight,
                        transform.Scale.z
                };
            }

            modelMatrix = MMath::UpdateModelMatrix(transform);
            auto mousePos = MInput::GetMouseCursor().Position;
            // flip Y axis for mouse to match coordinate system with widget
            mousePos.y = (float) MWindow::GetFrameHeight() - mousePos.y;
            isHovered = IsHovered3D(modelMatrix, mousePos);

            widget.ModelMatrix = modelMatrix;

            if (isHovered)
            {
                hoveredCallback(widget);
                if (pressed)
                {
                    pressedCallback(widget);
                }
            }
            else
            {
                widget.Color = color;
            }

            m_WidgetBuffer.Add(widget);
        }

        void cWidgetShader::DrawWidgets()
        {
            if (m_WidgetBuffer.Empty())
            {
                return;
            }

            m_WidgetBuffer.Flush();

            context::VSBindBuffer(m_WidgetBuffer);
            context::DrawIndexed(m_QuadInfo.IndexCount, m_WidgetBuffer.Size(), m_QuadInfo.VertexOffset, m_QuadInfo.IndexOffset);
            context::VSUnbindBuffer(m_WidgetBuffer);

            m_WidgetBuffer.Clear();
        }

        bool cWidgetShader::IsHovered2D(const glm::mat4 &modelMatrix, const glm::vec2 &mousePos)
        {
            glm::vec4 v1 = modelMatrix * glm::vec4(m_Quad.Vertices[0].Position, 1.0);
            glm::vec4 v2 = modelMatrix * glm::vec4(m_Quad.Vertices[1].Position, 1.0);
            glm::vec4 v3 = modelMatrix * glm::vec4(m_Quad.Vertices[2].Position, 1.0);
            glm::vec4 v4 = modelMatrix * glm::vec4(m_Quad.Vertices[3].Position, 1.0);

            float minX = Min(Min(Min(v1.x, v2.x), v3.x), v4.x);
            float maxX = Max(Max(Max(v1.x, v2.x), v3.x), v4.x);
            float minY = Min(Min(Min(v1.y, v2.y), v3.y), v4.y);
            float maxY = Max(Max(Max(v1.y, v2.y), v3.y), v4.y);

            bool isNotHovered = mousePos.x < minX || mousePos.x > maxX || mousePos.y < minY || mousePos.y > maxY;

            return !isNotHovered;
        }

        bool cWidgetShader::IsHovered3D(const glm::mat4 &modelMatrix, const glm::vec2 &mousePos)
        {
            return false;
        }

        cTextShader::cTextShader(const string& name)
        : cDefaultShader(eCategory::UI, name)
        {
            m_Quad = sQuad();
            m_QuadInfo = MGeometry::Add(m_Quad);
            PrimitiveTopology = m_Quad.PrimitiveTopology;
            m_CharBuffer.Reserve(1000);
            m_TextBuffer.Reserve(100);

            Samplers::Font            = new cSampler();
            Samplers::Font->Slot      = K_SLOT_FONT_SAMPLER;
            Samplers::Font->Filter    = cSampler::eFilter::MIN_MAG_MIP_LINEAR;
            Samplers::Font->AddressU  = cSampler::eAddress::WRAP;
            Samplers::Font->AddressV  = cSampler::eAddress::WRAP;
            Samplers::Font->AddressW  = cSampler::eAddress::WRAP;
            Samplers::Font->Init();

            RenderTarget = RenderTargets::UI;

            VertexStage = MShader::GetFromFile(cShaderStage::eType::VERTEX, "res/shaders/text.vs");
            PixelStage = MShader::GetFromFile(cShaderStage::eType::PIXEL, "res/shaders/text.ps");

            VertexStage->SetBuffer(&m_CharBuffer);
            VertexStage->SetBuffer(&m_TextBuffer);

            PixelStage->SetSampler(Samplers::Font);

            Init();
        }

        cTextShader::~cTextShader()
        {
            delete Samplers::Font;
        }

        void cTextShader::Draw(cScene *scene)
        {
            auto& camera = Buffers::Camera->Item;

            sOrthoMatrix orthoMatrix;
            orthoMatrix.Left = camera.Viewport.Left;
            orthoMatrix.Top = camera.Viewport.Top;
            orthoMatrix.Right = camera.Viewport.Width;
            orthoMatrix.Bottom = camera.Viewport.Height;
            orthoMatrix.Near = -1.0f;
            orthoMatrix.Far = 1.0f;
            m_Projection2D = MMath::UpdateOrthoMatrix(orthoMatrix);
            m_Projection3D = camera.View * camera.Projection;
            cTexture* fontAtlas = nullptr;

            // label 2D
            {
                auto components = scene->GetComponents<CVisible, CSpace2D, CTransform, CLabel>();
                for (auto [entity, visible, space, transform, label] : components.each())
                {
                    fontAtlas = &label.Font->Atlas;
                    UpdateText2D(
                            m_Projection2D,
                            transform,
                            0,
                            *label.Font,
                            label.Text,
                            label.Color,
                            label.FillFrame
                    );
                }
            }

            // label 3D
            {
                auto components = scene->GetComponents<CVisible, CSpace2D, CTransform, CLabel>();
                for (auto [entity, visible, space, transform, label] : components.each())
                {
                    fontAtlas = &label.Font->Atlas;
                    UpdateText3D(
                            m_Projection3D,
                            transform,
                            0,
                            *label.Font,
                            label.Text,
                            label.Color,
                            label.FillFrame
                    );
                }
            }

            if (fontAtlas != nullptr)
            {
                DrawTexts(*fontAtlas);
            }
        }

        void cTextShader::DrawTexts(cTexture& fontAtlas)
        {
            if (m_CharBuffer.Empty() || m_TextBuffer.Empty())
            {
                return;
            }

            m_CharBuffer.Flush();
            m_TextBuffer.Flush();

            context::VSBindBuffer(m_CharBuffer);
            context::VSBindBuffer(m_TextBuffer);
            context::PSBindTexture(fontAtlas, K_SLOT_FONT_ATLAS);
            context::DrawIndexed(m_QuadInfo.IndexCount, m_CharBuffer.Size(), m_QuadInfo.VertexOffset, m_QuadInfo.IndexOffset);
            context::PSUnbindTexture(fontAtlas);
            context::VSUnbindBuffer(m_TextBuffer);
            context::VSUnbindBuffer(m_CharBuffer);

            m_CharBuffer.Clear();
            m_TextBuffer.Clear();
        }

        void cTextShader::UpdateText2D(
                glm::mat4 projection,
                CTransform transform,
                u32 textIndex,
                sFont& font,
                const string& textString,
                const glm::vec4& textColor,
                bool fillFrame
        )
        {
            if (textString.empty())
            {
                return;
            }

            usize charsCount = textString.size();
            const char* chars = textString.c_str();
            glm::mat4 modelMatrix;
            sText text;
            text.Projection = projection;
            text.Color = textColor;

            float frameWidth = (float) MWindow::GetFrameWidth();
            float frameHeight = (float) MWindow::GetFrameHeight();

            transform.Position = {
                    transform.Position.x * frameWidth,
                    transform.Position.y * frameHeight,
                    transform.Position.z
            };

            if (fillFrame) {
                transform.Scale = {
                        transform.Scale.x * frameWidth,
                        transform.Scale.y * frameHeight,
                        transform.Scale.z
                };
            }

            text.ModelMatrix = MMath::UpdateModelMatrix(transform);

            m_TextBuffer.Add(text);

            glm::vec2 advance = { 0, 0 };

            for (usize i = 0; i < charsCount; i++)
            {
                char c = chars[i];

                // find glyph by char
                auto it = font.AlphaBet.find(c);
                if (it == font.AlphaBet.end())
                {
                    continue;
                }
                sFont::sGlyph glyph = it->second;

                xpe::render::sChar character;
                character.TextIndex = textIndex;
                character.GlyphSize = font.GlyphSize;
                character.Width = glyph.Width;
                character.Height = glyph.Height;
                character.Left = glyph.Left;
                character.Top = glyph.Top;
                character.AdvanceX = advance.x;
                character.AdvanceY = advance.y;
                character.AtlasXOffset = glyph.AtlasXOffset;
                character.AtlasYOffset = glyph.AtlasYOffset;

                // Tab
                if (c == '\t')
                {
                    advance.x += font.WhitespaceOffset * transform.Scale.x;
                }

                    // New line
                else if (c == '\n')
                {
                    advance.x = 0.0f;
                    advance.y += (font.MaxGlyphHeight + font.NewLineOffset) * transform.Scale.y;
                }

                else
                {
                    advance.x += (glyph.AdvanceX / 64.0f) * transform.Scale.x;
                    m_CharBuffer.Add(character);
                }
            }
        }

        void cTextShader::UpdateText3D(
                glm::mat4 projection,
                CTransform transform,
                u32 textIndex,
                sFont& font,
                const string& textString,
                const glm::vec4& textColor,
                bool fillFrame
        )
        {
            if (textString.empty())
            {
                return;
            }

            usize charsCount = textString.size();
            const char* chars = textString.c_str();
            glm::mat4 modelMatrix;
            sText text;
            text.Projection = projection;
            text.Color = textColor;
            text.ModelMatrix = MMath::UpdateModelMatrix(transform);

            m_TextBuffer.Add(text);

            glm::vec2 advance = { 0, 0 };

            for (usize i = 0; i < charsCount; i++)
            {
                char c = chars[i];

                // find glyph by char
                auto it = font.AlphaBet.find(c);
                if (it == font.AlphaBet.end())
                {
                    continue;
                }
                sFont::sGlyph glyph = it->second;

                xpe::render::sChar character;
                character.TextIndex = textIndex;
                character.GlyphSize = font.GlyphSize;
                character.Width = glyph.Width;
                character.Height = glyph.Height;
                character.Left = glyph.Left;
                character.Top = glyph.Top;
                character.AdvanceX = advance.x;
                character.AdvanceY = advance.y;
                character.AtlasXOffset = glyph.AtlasXOffset;
                character.AtlasYOffset = glyph.AtlasYOffset;

                // Tab
                if (c == '\t')
                {
                    advance.x += font.WhitespaceOffset * transform.Scale.x;
                }

                // New line
                else if (c == '\n')
                {
                    advance.x = 0.0f;
                    advance.y += (font.MaxGlyphHeight + font.NewLineOffset) * transform.Scale.y;
                }

                else
                {
                    advance.x += (glyph.AdvanceX / 64.0f) * transform.Scale.x;
                    m_CharBuffer.Add(character);
                }
            }
        }

        bool cTextShader::IsHovered2D(const glm::mat4& modelMatrix, const glm::vec2 &mousePos)
        {
            glm::vec4 v1 = modelMatrix * glm::vec4(m_Quad.Vertices[0].Position, 1.0);
            glm::vec4 v2 = modelMatrix * glm::vec4(m_Quad.Vertices[1].Position, 1.0);
            glm::vec4 v3 = modelMatrix * glm::vec4(m_Quad.Vertices[2].Position, 1.0);
            glm::vec4 v4 = modelMatrix * glm::vec4(m_Quad.Vertices[3].Position, 1.0);

            float minX = Min(Min(Min(v1.x, v2.x), v3.x), v4.x);
            float maxX = Max(Max(Max(v1.x, v2.x), v3.x), v4.x);
            float minY = Min(Min(Min(v1.y, v2.y), v3.y), v4.y);
            float maxY = Max(Max(Max(v1.y, v2.y), v3.y), v4.y);

            bool isNotHovered = mousePos.x < minX || mousePos.x > maxX || mousePos.y < minY || mousePos.y > maxY;

            return !isNotHovered;
        }

        bool cTextShader::IsHovered3D(const glm::mat4 &modelMatrix, const glm::vec2 &mousePos)
        {
            return false;
        }
    }
}