#include <rendering/shaders.hpp>
#include <rendering/bindings.hpp>
#include <rendering/skybox_manager.hpp>
#include <rendering/camera_manager.hpp>
#include <rendering/geometry/geometry_manager.hpp>
#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        cInstancingShader::cInstancingShader(eCategory category, const string& name)
        : cDefaultShader(category, name)
        {
            m_InstanceBuffer.Reserve(1000);
        }

        void cInstancingShader::DrawInstanced(
                cScene* scene,
                const sGeometryInfo& geometryInfo,
                const std::function<void(EntityID entityId, sRenderInstance&)>& callback
        ) {
            usize entityCount = geometryInfo.Entities.size();
            usize instanceCount = 0;
            m_InstanceBuffer.Clear();
            for (usize i = 0 ; i < entityCount ; i++)
            {
                auto& entity = geometryInfo.Entities[i];

                if (!scene->HasAnyComponent<CVisible>(entity))
                    continue;

                sRenderInstance instance;
                instance.ModelMatrix = cMathManager::UpdateModelMatrix(scene->GetComponent<CTransform>(entity));
                instance.NormalMatrix = cMathManager::UpdateNormalMatrix(instance.ModelMatrix);
                callback(entity, instance);

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
            VertexStage = cShaderManager::GetFromFile(sShaderStage::eType::VERTEX, "engine_shaders/passes/geometry_pass.vs");
            PixelStage =  cShaderManager::GetFromFile(sShaderStage::eType::PIXEL, "engine_shaders/passes/pbr_pass_opaque.ps");

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
            PixelStage->SetTexture(RenderTargets::Shadow->Colors[0]);

            PixelStage->SetSampler(Samplers::Material);
            PixelStage->SetSampler(Samplers::Shadow);

            Init();
        }

        void cOpaqueShader::Draw(ecs::cScene* scene)
        {
            // Draw geometry
            {
                auto components = scene->GetComponents<COpaque, CGeometryInfo>();
                for (auto [entity, opaque, geometryInfo]: components.each()) {
                    DrawInstanced(
                            scene,
                            geometryInfo,
                            [scene](EntityID entityId, sRenderInstance &instance) {
                                instance.MaterialIndex = scene->GetComponent<CMaterial>(entityId).Index;
                            }
                    );
                }
            }
            // Draw skeletons
            {
                auto components = scene->GetComponents<COpaque, CSkeletonInfo>();
                for (auto [entity, opaque, skeletonInfo]: components.each()) {
                    DrawInstanced(
                            scene,
                            skeletonInfo.GeometryInfo,
                            [scene, &skeletonInfo](EntityID entityId, sRenderInstance &instance) {
                                instance.MaterialIndex = scene->GetComponent<CMaterial>(entityId).Index;
                                instance.SkeletonIndex = skeletonInfo.SkeletonIndex;
                            }
                    );
                }
            }
        }

        cTransparentShader::cTransparentShader(const string &name)
        : cInstancingShader(eCategory::TRANSPARENT, name)
        {
            VertexStage = cShaderManager::GetFromFile(sShaderStage::eType::VERTEX, "engine_shaders/passes/geometry_pass.vs");
            PixelStage =  cShaderManager::GetFromFile(sShaderStage::eType::PIXEL, "engine_shaders/passes/pbr_pass_transparent.ps");

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
            PixelStage->SetTexture(RenderTargets::Shadow->Colors[0]);

            PixelStage->SetSampler(Samplers::Material);
            PixelStage->SetSampler(Samplers::Shadow);

            Init();
        }

        void cTransparentShader::Draw(ecs::cScene* scene)
        {
            // Draw geometry
            {
                auto components = scene->GetComponents<CTransparent, CGeometryInfo>();
                for (auto [entity, transparent, geometryInfo]: components.each()) {
                    DrawInstanced(
                            scene,
                            geometryInfo,
                            [scene](EntityID entityId, sRenderInstance &instance) {
                                instance.MaterialIndex = scene->GetComponent<CMaterial>(entityId).Index;
                            }
                    );
                }
            }
            // Draw skeletons
            {
                auto components = scene->GetComponents<CTransparent, CSkeletonInfo>();
                for (auto [entity, transparent, skeletonInfo]: components.each()) {
                    DrawInstanced(
                            scene,
                            skeletonInfo.GeometryInfo,
                            [scene, &skeletonInfo](EntityID entityId, sRenderInstance &instance) {
                                instance.MaterialIndex = scene->GetComponent<CMaterial>(entityId).Index;
                                instance.SkeletonIndex = skeletonInfo.SkeletonIndex;
                            }
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
                auto components = scene->GetComponents<CHasShadow, CGeometryInfo>();
                for (auto [entity, hasShadow, geometryInfo]: components.each())
                {
                    DrawInstanced(
                            scene,
                            geometryInfo,
                            [&lightIndex](EntityID entityId, sRenderInstance& instance) {
                                instance.LightIndex = lightIndex;
                            }
                    );
                }
            }
            // Draw skeletons
            {
                auto components = scene->GetComponents<CHasShadow, CSkeletonInfo>();
                for (auto [entity, hasShadow, skeletonInfo]: components.each())
                {
                    DrawInstanced(
                            scene,
                            skeletonInfo.GeometryInfo,
                            [&skeletonInfo, &lightIndex](EntityID entityId, sRenderInstance &instance) {
                                instance.SkeletonIndex = skeletonInfo.SkeletonIndex;
                                instance.LightIndex = lightIndex;
                            }
                    );
                }
            }
        }

        cDirectionalShadowShader::cDirectionalShadowShader(const string &name)
        : cShadowShader(name)
        {
            VertexStage = cShaderManager::GetFromFile(sShaderStage::eType::VERTEX, "engine_shaders/passes/directional_shadow_pass.vs");
            PixelStage = cShaderManager::GetFromFile(sShaderStage::eType::PIXEL, "engine_shaders/passes/directional_shadow_pass.ps");

            RenderTarget = RenderTargets::Shadow;

            VertexStage->SetBuffer(Buffers::Skeleton);
            VertexStage->SetBuffer(Buffers::DirectLight);

            Init();
        }

        void cDirectionalShadowShader::Draw(cScene* scene)
        {
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
            VertexStage = cShaderManager::GetFromFile(sShaderStage::eType::VERTEX, "engine_shaders/passes/point_shadow_pass.vs");
            PixelStage = cShaderManager::GetFromFile(sShaderStage::eType::PIXEL, "engine_shaders/passes/point_shadow_pass.ps");

            RenderTarget = RenderTargets::Shadow;

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
            VertexStage = cShaderManager::GetFromFile(sShaderStage::eType::VERTEX, "engine_shaders/passes/spot_shadow_pass.vs");
            PixelStage = cShaderManager::GetFromFile(sShaderStage::eType::PIXEL, "engine_shaders/passes/spot_shadow_pass.ps");

            RenderTarget = RenderTargets::Shadow;

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
            Samplers::Skybox = new sSampler();
            Samplers::Skybox->Filter = sSampler::eFilter::MIN_MAG_MIP_LINEAR;
            Samplers::Skybox->Init();

            VertexStage = cShaderManager::GetFromFile(sShaderStage::eType::VERTEX, "engine_shaders/passes/skybox_pass.vs");
            PixelStage = cShaderManager::GetFromFile(sShaderStage::eType::PIXEL, "engine_shaders/passes/skybox_pass.ps");

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
            auto& skybox = cSkyboxManager::Get().Skybox;
            if (skybox.Texture) {
                sGeometryInfo geometryInfo = skybox.GeometryInfo;
                sTexture& skyboxTexture = *skybox.Texture;
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

        cCompositeTransparentShader::cCompositeTransparentShader(const string& name, u32 sampleCount)
        : cDefaultShader(eCategory::POSTFX, name)
        {
            VertexStage = cShaderManager::GetFromFile(sShaderStage::eType::VERTEX, "engine_shaders/passes/screen.vs");
            PixelStage = cShaderManager::GetFromFile(sShaderStage::eType::PIXEL, sampleCount > 1 ? "engine_shaders/passes/msaa/composite_pass_transparent.ps" : "engine_shaders/passes/composite_pass_transparent.ps");

            RenderTarget = RenderTargets::Opaque;

            PixelStage->SetTexture(RenderTargets::Transparent->Colors[0], 0);
            PixelStage->SetTexture(RenderTargets::Transparent->Colors[1], 1);

            Init();
        }

        void cCompositeTransparentShader::InitPostFX() {
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
            Viewports::SSAO->Width = cWindowManager::GetFrameWidth();
            Viewports::SSAO->Height = cWindowManager::GetFrameHeight();

            sTexture* ssaoColor = new sTexture();
            ssaoColor->Width = Viewports::SSAO->Width;
            ssaoColor->Height = Viewports::SSAO->Height;
            ssaoColor->Format = eTextureFormat::RGBA8;
            ssaoColor->SampleCount = sampleCount;
            ssaoColor->InitializeData = false;
            ssaoColor->EnableRenderTarget = true;
            ssaoColor->SetResizable(true);
            ssaoColor->Init();

            sTexture* ssaoDepth = new sTexture();
            ssaoDepth->Type = sTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            ssaoDepth->Width = Viewports::SSAO->Width;
            ssaoDepth->Height = Viewports::SSAO->Height;
            ssaoDepth->Format = eTextureFormat::R32_TYPELESS;
            ssaoDepth->SampleCount = sampleCount;
            ssaoDepth->InitializeData = false;
            ssaoDepth->EnableRenderTarget = true;
            ssaoDepth->SetResizable(true);
            ssaoDepth->Init();

            RenderTargets::SSAO = new sRenderTarget({ ssaoColor }, ssaoDepth, Viewports::SSAO);
            RenderTargets::SSAO->SetResizable(true);
            RenderTargets::SSAO->ClearColors.emplace_back(glm::vec4(1.0f));
            RenderTargets::SSAO->ClearDepth = 1.0f;

            RenderTarget = RenderTargets::SSAO;

            VertexStage = cShaderManager::GetFromFile(sShaderStage::eType::VERTEX, "engine_shaders/passes/screen.vs");
            PixelStage = cShaderManager::GetFromFile(sShaderStage::eType::PIXEL, sampleCount > 1 ? "engine_shaders/passes/msaa/ssao_pass.ps" : "engine_shaders/passes/ssao_pass.ps");

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
            VertexStage = cShaderManager::GetFromFile(sShaderStage::eType::VERTEX, "engine_shaders/passes/screen.vs");
            PixelStage = cShaderManager::GetFromFile(sShaderStage::eType::PIXEL, sampleCount > 1 ? "engine_shaders/passes/msaa/final_pass.ps" : "engine_shaders/passes/final_pass.ps");

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
            std::tie(m_Quad, m_QuadInfo) = cGeometryManager::AddGeometry(sQuad());
            PrimitiveTopology = m_QuadInfo.PrimitiveTopology;
            m_WidgetBuffer.Reserve(100);

            RenderTarget = RenderTargets::UI;

            VertexStage = cShaderManager::GetFromFile(sShaderStage::eType::VERTEX, "engine_shaders/passes/widget_pass.vs");
            PixelStage = cShaderManager::GetFromFile(sShaderStage::eType::PIXEL, "engine_shaders/passes/widget_pass.ps");

            VertexStage->SetBuffer(&m_WidgetBuffer);

            PixelStage->SetTexture(Textures::WidgetAtlas);
            PixelStage->SetSampler(Samplers::Widget);

            Init();
        }

        void cWidgetShader::Draw(cScene* scene)
        {
            sOrthoMatrix orthoMatrix;
            orthoMatrix.Left = cCameraManager::GetViewport()->Left;
            orthoMatrix.Top = cCameraManager::GetViewport()->Top;
            orthoMatrix.Right = cCameraManager::GetViewport()->Width;
            orthoMatrix.Bottom = cCameraManager::GetViewport()->Height;
            orthoMatrix.Near = -1.0f;
            orthoMatrix.Far = 1.0f;
            m_Projection2D = cMathManager::UpdateOrthoMatrix(orthoMatrix);
            m_Projection3D = cCameraManager::GetViewProjection();

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
                            cInputManager::MousePressed(button.MousePressed) && button.EnablePress,
                            [&button](sWidgetData& widget) {
                                widget.Color = button.ColorPressed;
                                if (button.Pressed != nullptr) {
                                    button.Pressed();
                                }
                            },
                            [&button](sWidgetData& widget) {
                                widget.Color = button.ColorHover;
                                if (button.Hovered != nullptr) {
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
                            cInputManager::MousePressed(button.MousePressed) && button.EnablePress,
                            [&button](sWidgetData& widget) {
                                widget.Color = button.ColorPressed;
                                if (button.Pressed != nullptr) {
                                    button.Pressed();
                                }
                            },
                            [&button](sWidgetData& widget) {
                                widget.Color = button.ColorHover;
                                if (button.Hovered != nullptr) {
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
                sTransform transform,
                const glm::vec4& color,
                bool enableTexture,
                const sAtlas::sLocation& atlasLocation,
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

            float frameWidth = (float) cWindowManager::GetFrameWidth();
            float frameHeight = (float) cWindowManager::GetFrameHeight();

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

            modelMatrix = cMathManager::UpdateModelMatrix(transform);
            auto mousePos = cInputManager::GetMouseCursor().Position;
            // flip Y axis for mouse to match coordinate system with widget
            mousePos.y = (float) cWindowManager::GetFrameHeight() - mousePos.y;
            isHovered = IsHovered2D(modelMatrix, mousePos);

            widget.ModelMatrix = modelMatrix;

            if (isHovered) {
                hoveredCallback(widget);
                if (pressed) {
                    pressedCallback(widget);
                }
            }
            else {
                widget.Color = color;
            }

            m_WidgetBuffer.Add(widget);
        }

        void cWidgetShader::UpdateWidget3D(
                glm::mat4 projection,
                sTransform transform,
                const glm::vec4& color,
                bool enableTexture,
                const sAtlas::sLocation& atlasLocation,
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

            float frameWidth = (float) cWindowManager::GetFrameWidth();
            float frameHeight = (float) cWindowManager::GetFrameHeight();

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

            modelMatrix = cMathManager::UpdateModelMatrix(transform);
            auto mousePos = cInputManager::GetMouseCursor().Position;
            // flip Y axis for mouse to match coordinate system with widget
            mousePos.y = (float) cWindowManager::GetFrameHeight() - mousePos.y;
            isHovered = IsHovered3D(modelMatrix, mousePos);

            widget.ModelMatrix = modelMatrix;

            if (isHovered) {
                hoveredCallback(widget);
                if (pressed) {
                    pressedCallback(widget);
                }
            }
            else {
                widget.Color = color;
            }

            m_WidgetBuffer.Add(widget);
        }

        void cWidgetShader::DrawWidgets()
        {
            if (m_WidgetBuffer.Empty())
                return;

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

            float minX = min(min(min(v1.x, v2.x), v3.x), v4.x);
            float maxX = max(max(max(v1.x, v2.x), v3.x), v4.x);
            float minY = min(min(min(v1.y, v2.y), v3.y), v4.y);
            float maxY = max(max(max(v1.y, v2.y), v3.y), v4.y);

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
            std::tie(m_Quad, m_QuadInfo) = cGeometryManager::AddGeometry(sQuad());
            PrimitiveTopology = m_Quad.PrimitiveTopology;
            m_CharBuffer.Reserve(1000);
            m_TextBuffer.Reserve(100);

            Samplers::Font            = new sSampler();
            Samplers::Font->Slot      = K_SLOT_FONT_SAMPLER;
            Samplers::Font->Filter    = sSampler::eFilter::MIN_MAG_MIP_LINEAR;
            Samplers::Font->AddressU  = sSampler::eAddress::WRAP;
            Samplers::Font->AddressV  = sSampler::eAddress::WRAP;
            Samplers::Font->AddressW  = sSampler::eAddress::WRAP;
            Samplers::Font->Init();

            RenderTarget = RenderTargets::UI;

            VertexStage = cShaderManager::GetFromFile(sShaderStage::eType::VERTEX, "engine_shaders/passes/text_pass.vs");
            PixelStage = cShaderManager::GetFromFile(sShaderStage::eType::PIXEL, "engine_shaders/passes/text_pass.ps");

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
            sOrthoMatrix orthoMatrix;
            orthoMatrix.Left = cCameraManager::GetViewport()->Left;
            orthoMatrix.Top = cCameraManager::GetViewport()->Top;
            orthoMatrix.Right = cCameraManager::GetViewport()->Width;
            orthoMatrix.Bottom = cCameraManager::GetViewport()->Height;
            orthoMatrix.Near = -1.0f;
            orthoMatrix.Far = 1.0f;
            m_Projection2D = cMathManager::UpdateOrthoMatrix(orthoMatrix);
            m_Projection3D = cCameraManager::GetViewProjection();
            sTexture* fontAtlas = nullptr;

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

            if (fontAtlas != nullptr) {
                DrawTexts(*fontAtlas);
            }
        }

        void cTextShader::DrawTexts(sTexture& fontAtlas)
        {
            if (m_CharBuffer.Empty() || m_TextBuffer.Empty())
                return;

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
                sTransform transform,
                u32 textIndex,
                sFont& font,
                const string& textString,
                const glm::vec4& textColor,
                bool fillFrame
        ) {
            if (textString.empty()) return;

            usize charsCount = textString.size();
            const char* chars = textString.c_str();
            glm::mat4 modelMatrix;
            sText text;
            text.Projection = projection;
            text.Color = textColor;

            float frameWidth = (float) cWindowManager::GetFrameWidth();
            float frameHeight = (float) cWindowManager::GetFrameHeight();

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

            text.ModelMatrix = cMathManager::UpdateModelMatrix(transform);

            m_TextBuffer.Add(text);

            glm::vec2 advance = { 0, 0 };

            for (usize i = 0; i < charsCount; i++)
            {
                char c = chars[i];

                // find glyph by char
                auto it = font.AlphaBet.find(c);
                if (it == font.AlphaBet.end()) {
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
                sTransform transform,
                u32 textIndex,
                sFont& font,
                const string& textString,
                const glm::vec4& textColor,
                bool fillFrame
        ) {
            if (textString.empty()) return;

            usize charsCount = textString.size();
            const char* chars = textString.c_str();
            glm::mat4 modelMatrix;
            sText text;
            text.Projection = projection;
            text.Color = textColor;
            text.ModelMatrix = cMathManager::UpdateModelMatrix(transform);

            m_TextBuffer.Add(text);

            glm::vec2 advance = { 0, 0 };

            for (usize i = 0; i < charsCount; i++)
            {
                char c = chars[i];

                // find glyph by char
                auto it = font.AlphaBet.find(c);
                if (it == font.AlphaBet.end()) {
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

        bool cTextShader::IsHovered2D(const glm::mat4& modelMatrix, const glm::vec2 &mousePos) {
            glm::vec4 v1 = modelMatrix * glm::vec4(m_Quad.Vertices[0].Position, 1.0);
            glm::vec4 v2 = modelMatrix * glm::vec4(m_Quad.Vertices[1].Position, 1.0);
            glm::vec4 v3 = modelMatrix * glm::vec4(m_Quad.Vertices[2].Position, 1.0);
            glm::vec4 v4 = modelMatrix * glm::vec4(m_Quad.Vertices[3].Position, 1.0);

            float minX = min(min(min(v1.x, v2.x), v3.x), v4.x);
            float maxX = max(max(max(v1.x, v2.x), v3.x), v4.x);
            float minY = min(min(min(v1.y, v2.y), v3.y), v4.y);
            float maxY = max(max(max(v1.y, v2.y), v3.y), v4.y);

            bool isNotHovered = mousePos.x < minX || mousePos.x > maxX || mousePos.y < minY || mousePos.y > maxY;

            return !isNotHovered;
        }

        bool cTextShader::IsHovered3D(const glm::mat4 &modelMatrix, const glm::vec2 &mousePos)
        {
            return false;
        }

    }

}