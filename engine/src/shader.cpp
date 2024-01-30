#include <shader.hpp>
#include <render_context.hpp>

namespace xpe
{
    namespace render
    {
        unordered_map<string, cShaderStage>* MShader::s_Stages = nullptr;
        cShader* MShader::s_ComputeShaders = nullptr;
        cShader* MShader::s_PrepassShaders = nullptr;
        cShader* MShader::s_OpaqueShaders = nullptr;
        cShader* MShader::s_TransparentShaders = nullptr;
        cShader* MShader::s_PostfxShaders = nullptr;
        cShader* MShader::s_UiShaders = nullptr;
        cShader* MShader::s_FinalShaders = nullptr;

        void cDefaultShader::Init()
        {
            switch (Category)
            {
                case eCategory::PREPASS:
                    InitPrepass();
                    break;

                case eCategory::OPAQUE:
                    InitOpaque();
                    break;

                case eCategory::TRANSPARENT:
                    InitTransparent();
                    break;

                case eCategory::POSTFX:
                    InitPostFX();
                    break;

                case eCategory::UI:
                    InitUI();
                    break;

                case eCategory::FINAL:
                    InitFinal();
                    break;

                default:
                    break;
            }

            if (Category != eCategory::NONE)
            {
                context::CreateRasterizer(Rasterizer);
                context::CreateDepthStencilMode(DepthStencilMode);
                context::CreateBlendMode(BlendMode);

                if (VertexStage != nullptr)
                {
                    sBlob* vertexBlob = &VertexStage->Blob;

                    if (vertexBlob == nullptr)
                    {
                        LogError("Failed to create input layout. Shader has no vertex stage.");
                        assert(false);
                        return;
                    }

                    if (vertexBlob->ByteCode == nullptr || vertexBlob->ByteCodeSize == 0)
                    {
                        LogError("Failed to create input layout. Vertex shader bytecode is empty.");
                        assert(false);
                        return;
                    }

                    InputLayout.VertexBlob = vertexBlob;
                    context::CreateInputLayout(InputLayout);
                }
            }
        }

        void cDefaultShader::InitPrepass()
        {
            DepthStencilMode.DepthWriteMask = eDepthWriteMask::ALL;

            sBlendTarget target;
            target.Enable = false;
            BlendMode.Targets.push_back(target);
            BlendMode.Targets.push_back(target);
            BlendMode.Targets.push_back(target);
            BlendMode.IndependentBlendEnable = true;

            Rasterizer.CullMode = eCullMode::NONE;
        }

        void cDefaultShader::InitOpaque()
        {
            DepthStencilMode.DepthWriteMask = eDepthWriteMask::ALL;

            sBlendTarget target;
            target.Enable = false;
            BlendMode.Targets.push_back(target);
            BlendMode.Targets.push_back(target);
            BlendMode.Targets.push_back(target);
            BlendMode.IndependentBlendEnable = true;

            Rasterizer.CullMode = eCullMode::NONE;
        }

        void cDefaultShader::InitTransparent()
        {
            DepthStencilMode.DepthWriteMask = eDepthWriteMask::ZERO;

            sBlendTarget target;
            target.Enable = true;
            target.Src = eBlend::ONE;
            target.Dest = eBlend::ONE;
            target.BlendOp = eBlendOp::ADD;
            target.SrcAlpha = eBlend::ONE;
            target.DestAlpha = eBlend::ONE;
            target.BlendOpAlpha = eBlendOp::ADD;
            BlendMode.Targets.push_back(target);

            target.Enable = true;
            target.Src = eBlend::ZERO;
            target.Dest = eBlend::INV_SRC_COLOR;
            target.BlendOp = eBlendOp::ADD;
            target.SrcAlpha = eBlend::ZERO;
            target.DestAlpha = eBlend::ZERO;
            target.BlendOpAlpha = eBlendOp::ADD;
            BlendMode.Targets.push_back(target);

            BlendMode.IndependentBlendEnable = true;

            Rasterizer.CullMode = eCullMode::NONE;
        }

        void cDefaultShader::InitPostFX()
        {
            DepthStencilMode.DepthWriteMask = eDepthWriteMask::ALL;

            sBlendTarget target;
            target.Enable = false;
            BlendMode.Targets.push_back(target);
            BlendMode.IndependentBlendEnable = true;
        }

        void cDefaultShader::InitUI()
        {
            DepthStencilMode.DepthWriteMask = eDepthWriteMask::ALL;

            sBlendTarget target;
            target.Enable = true;
            BlendMode.Targets.push_back(target);
            BlendMode.Targets.push_back(target);
            BlendMode.Targets.push_back(target);
            BlendMode.IndependentBlendEnable = true;
        }

        void cDefaultShader::InitFinal()
        {
            DepthStencilMode.DepthWriteMask = eDepthWriteMask::ALL;

            sBlendTarget target;
            target.Enable = false;
            BlendMode.Targets.push_back(target);
            BlendMode.IndependentBlendEnable = true;
        }

        cDefaultShader::cDefaultShader(cShader::eCategory category, const string& name)
        : cShader(category, name)
        {
        }

        cDefaultShader::~cDefaultShader()
        {
            if (Category != eCategory::NONE)
            {
                context::FreeInputLayout(InputLayout);
                context::FreeRasterizer(Rasterizer);
                context::FreeDepthStencilMode(DepthStencilMode);
                context::FreeBlendMode(BlendMode);
            }
        }

        void cDefaultShader::Bind()
        {
            context::BindPrimitiveTopology(PrimitiveTopology);
            context::BindInputLayout(InputLayout);

            if (VertexStage != nullptr)
            {
                context::VSBindStage(*VertexStage);
            }

            if (PixelStage != nullptr)
            {
                context::PSBindStage(*PixelStage);
            }

            if (GeometryStage != nullptr)
            {
                context::GSBindStage(*GeometryStage);
            }

            if (RenderTarget != nullptr)
            {
                RenderTarget->Bind();
            }

            context::BindRasterizer(Rasterizer.State);
            context::BindDepthStencilMode(DepthStencilMode.State);
            context::BindBlendMode(BlendMode.State);
        }

        void cDefaultShader::Unbind()
        {
            if (VertexStage != nullptr)
            {
                context::VSUnbindStage(*VertexStage);
            }

            if (PixelStage != nullptr)
            {
                context::PSUnbindStage(*PixelStage);
            }

            if (GeometryStage != nullptr)
            {
                context::GSUnbindStage(*GeometryStage);
            }

            if (RenderTarget != nullptr)
            {
                RenderTarget->Unbind();
            }
        }

        void cComputeShader::Bind()
        {
            if (ComputeStage != nullptr)
            {
                context::CSBindStage(*ComputeStage);
            }
        }

        void cComputeShader::Unbind()
        {
            if (ComputeStage != nullptr)
            {
                context::CSUnbindStage(*ComputeStage);
            }
        }

        void cComputeShader::Draw(ecs::cScene *scene)
        {
            context::Dispatch(m_ThreadGroups);
        }

        void MShader::Init()
        {
            LogInfo("MShader::Init()");

            s_Stages = new unordered_map<string, cShaderStage>();

#ifdef K_DEBUG
            // for testing shader code without #includes
            MFile::CreateDir("generated");
            MFile::CreateDir("generated/res");
            MFile::CreateDir("generated/res/shaders");
#endif

            LogInfo("MShader initialized");
        }

        void MShader::Free()
        {
            LogInfo("MShader::Free()");
            FreeStages();
            delete s_Stages;
            delete s_ComputeShaders;
            delete s_PrepassShaders;
            delete s_OpaqueShaders;
            delete s_TransparentShaders;
            delete s_PostfxShaders;
            delete s_UiShaders;
            delete s_FinalShaders;
        }

        cShaderStage* MShader::GetFromFile(const cShaderStage::eType &type, const char *filepath)
        {
            if (s_Stages->find(filepath) != s_Stages->end())
            {
                return &s_Stages->at(filepath);
            }
            string src = MFile::ReadFileWithIncludes(filepath, "#include");
            WriteGeneratedShader(filepath, src);
            return GetFromSrc(type, filepath, src);
        }

        cShaderStage* MShader::GetFromSrc(
                const cShaderStage::eType &type,
                const string &id,
                const string &src
        )
        {
            if (src.empty())
            {
                LogError("{} shader stage source is empty", id);
                return nullptr;
            }

            string profile;
            string entryPoint;

            switch (type)
            {
                case cShaderStage::eType::VERTEX:
                    profile = K_PROFILE_VERTEX;
                    entryPoint = K_ENTRY_POINT_VERTEX;
                    break;

                case cShaderStage::eType::PIXEL:
                    profile = K_PROFILE_PIXEL;
                    entryPoint = K_ENTRY_POINT_PIXEL;
                    break;

                case cShaderStage::eType::GEOMETRY:
                    profile = K_PROFILE_GEOMETRY;
                    entryPoint = K_ENTRY_POINT_GEOMETRY;
                    break;

                case cShaderStage::eType::COMPUTE:
                    profile = K_PROFILE_COMPUTE;
                    entryPoint = K_ENTRY_POINT_COMPUTE;
                    break;

                default:
                    LogError("Failed to identify shader stage type for {}", id);
                    return nullptr;
            }

            s_Stages->insert({ id, { Hash(id), type } });
            cShaderStage* stage = &s_Stages->at(id);
            stage->Profile = profile;
            stage->EntryPoint = entryPoint;
            stage->Source = src;
            context::CompileShaderStage(*stage);
            context::CreateShaderStage(*stage);
            return stage;
        }

        void MShader::ReloadStage(const char* filepath)
        {
            auto& stages = *s_Stages;

            auto it = stages.find(filepath);
            if (it != stages.end())
            {
                string src = MFile::ReadFileWithIncludes(filepath, "#include");

                if (src.empty())
                {
                    LogError("Failed to read shader stage source from {}", filepath);
                    return;
                }

                auto& stage = it->second;
                stage.Source = src;
                stage.Compiled = false;
                context::FreeShaderStage(stage);
                context::CreateShaderStage(stage);
            }
        }

        void MShader::FreeStages()
        {
            for (auto& shader : *s_Stages)
            {
                context::FreeShaderStage(shader.second);
            }
        }

        void MShader::WriteGeneratedShader(const char *filepath, const string &src)
        {
#ifdef K_DEBUG
            hstringstream ss;
            ss << "generated/" << filepath;
            hstring generatedFilepath = ss.str();
            MFile::WriteFile(generatedFilepath.c_str(), src);
#endif
        }

        void MShader::SetShader(cShader*& dest, cShader* src)
        {
            if (dest == nullptr)
            {
                dest = src;
                return;
            }

            cShader*& next = dest->Next;
            while (next != nullptr)
            {
                next = next->Next;
            }
            next = src;
        }

        void MShader::SetShader(cShader*& dest, cShader* src, u64 id)
        {
            if (dest == nullptr)
            {
                LogWarning("Can't set shader by ID because destination is NULL");
                return;
            }

            cShader*& next = dest->Next;
            while (next != nullptr)
            {
                if (next->ID == id)
                {
                    cShader* prev = next->Next;
                    next->Next = src;
                    src->Next = prev;
                    break;
                }
                next = next->Next;
            }
        }

        void MShader::SetShader(cShader* shader)
        {
            switch (shader->Category)
            {
                case cShader::eCategory::PREPASS:
                    SetShader(s_PrepassShaders, shader);
                    break;

                case cShader::eCategory::OPAQUE:
                    SetShader(s_OpaqueShaders, shader);
                    break;

                case cShader::eCategory::TRANSPARENT:
                    SetShader(s_TransparentShaders, shader);
                    break;

                case cShader::eCategory::POSTFX:
                    SetShader(s_PostfxShaders, shader);
                    break;

                case cShader::eCategory::UI:
                    SetShader(s_UiShaders, shader);
                    break;

                case cShader::eCategory::FINAL:
                    SetShader(s_FinalShaders, shader);
                    break;

                case cShader::eCategory::COMPUTE:
                    SetShader(s_ComputeShaders, shader);
                    break;
            }
        }

        void MShader::SetShaderAfter(cShader* shader, const string &name)
        {
            u64 id = Hash(name);
            switch (shader->Category)
            {
                case cShader::eCategory::PREPASS:
                    SetShader(s_PrepassShaders, shader, id);
                    break;

                case cShader::eCategory::OPAQUE:
                    SetShader(s_OpaqueShaders, shader, id);
                    break;

                case cShader::eCategory::TRANSPARENT:
                    SetShader(s_TransparentShaders, shader, id);
                    break;

                case cShader::eCategory::POSTFX:
                    SetShader(s_PostfxShaders, shader, id);
                    break;

                case cShader::eCategory::UI:
                    SetShader(s_UiShaders, shader, id);
                    break;

                case cShader::eCategory::FINAL:
                    SetShader(s_FinalShaders, shader, id);
                    break;

                case cShader::eCategory::COMPUTE:
                    SetShader(s_ComputeShaders, shader, id);
                    break;
            }
        }

        cShader* MShader::GetShaders(cShader::eCategory category)
        {
            switch (category)
            {
                case cShader::eCategory::NONE:        return nullptr;
                case cShader::eCategory::COMPUTE:     return s_ComputeShaders;
                case cShader::eCategory::PREPASS:     return s_PrepassShaders;
                case cShader::eCategory::OPAQUE:      return s_OpaqueShaders;
                case cShader::eCategory::TRANSPARENT: return s_TransparentShaders;
                case cShader::eCategory::POSTFX:      return s_PostfxShaders;
                case cShader::eCategory::UI:          return s_UiShaders;
                case cShader::eCategory::FINAL:       return s_FinalShaders;
            }
        }

        cShader* MShader::GetShader(const string& name)
        {
            u64 id = Hash(name);
            cShader* shader = nullptr;

            shader = GetShader(s_ComputeShaders, id);
            if (shader != nullptr)
            {
                return shader;
            }

            shader = GetShader(s_PrepassShaders, id);
            if (shader != nullptr)
            {
                return shader;
            }

            shader = GetShader(s_OpaqueShaders, id);
            if (shader != nullptr)
            {
                return shader;
            }

            shader = GetShader(s_TransparentShaders, id);
            if (shader != nullptr)
            {
                return shader;
            }

            shader = GetShader(s_PostfxShaders, id);
            if (shader != nullptr)
            {
                return shader;
            }

            shader = GetShader(s_UiShaders, id);
            if (shader != nullptr)
            {
                return shader;
            }

            shader = GetShader(s_FinalShaders, id);
            if (shader != nullptr)
            {
                return shader;
            }

            return shader;
        }

        cShader* MShader::GetShader(cShader*& src, u64 id)
        {
            cShader* next = src;
            while (next != nullptr)
            {
                if (next->ID == id)
                {
                    return next;
                }
                next = next->Next;
            }
            return nullptr;
        }
    }
}