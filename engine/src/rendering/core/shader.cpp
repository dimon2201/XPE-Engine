#include <rendering/core/context.hpp>
#include <rendering/core/shader.hpp>

namespace xpe {

    namespace render {

        unordered_map<string, sShaderStage>* cShaderManager::s_Stages = nullptr;
        cShader* cShaderManager::s_ComputeShaders = nullptr;
        cShader* cShaderManager::s_PrepassShaders = nullptr;
        cShader* cShaderManager::s_OpaqueShaders = nullptr;
        cShader* cShaderManager::s_TransparentShaders = nullptr;
        cShader* cShaderManager::s_PostfxShaders = nullptr;
        cShader* cShaderManager::s_UiShaders = nullptr;
        cShader* cShaderManager::s_FinalShaders = nullptr;

        void cDefaultShader::Init()
        {
            switch (Category)
            {
                case eCategory::PREPASS:
                    InitPrepass();
                    break;

                case eCategory::OPAQUE_GEOMETRY:
                    InitOpaque();
                    break;

                case eCategory::TRANSPARENT_GEOMETRY:
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

            if (Category != eCategory::NONE) {
                context::CreateRasterizer(Rasterizer);
                context::CreateDepthStencilMode(DepthStencilMode);
                context::CreateBlendMode(BlendMode);

                if (VertexStage != nullptr) {
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
            if (Category != eCategory::NONE) {
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

            if (VertexStage != nullptr) {
                context::BindVSStage(VertexStage);
            }

            if (PixelStage != nullptr) {
                context::BindPSStage(PixelStage);
            }

            if (GeometryStage != nullptr) {
                context::BindGSStage(GeometryStage);
            }

            if (RenderTarget != nullptr) {
                RenderTarget->Bind();
            }

            context::BindRasterizer(Rasterizer.State);
            context::BindDepthStencilMode(DepthStencilMode.State);
            context::BindBlendMode(BlendMode.State);
        }

        void cDefaultShader::Unbind()
        {
            if (VertexStage != nullptr) {
                context::UnbindVSStage(VertexStage);
            }

            if (PixelStage != nullptr) {
                context::UnbindPSStage(PixelStage);
            }

            if (GeometryStage != nullptr) {
                context::UnbindGSStage(GeometryStage);
            }

            if (RenderTarget != nullptr) {
                RenderTarget->Unbind();
            }
        }

        void cComputeShader::Bind()
        {
            if (ComputeStage != nullptr) {
                context::BindCSStage(ComputeStage);
            }
        }

        void cComputeShader::Unbind()
        {
            if (ComputeStage != nullptr) {
                context::UnbindCSStage(ComputeStage);
            }
        }

        void cComputeShader::Draw(ecs::cScene *scene)
        {
            context::Dispatch(m_ThreadGroups);
        }

        void cShaderManager::Init()
        {
            LogInfo("cShaderManager::Init()");

            s_Stages = new unordered_map<string, sShaderStage>();

            cFileManager::CreateDir("generated");
            cFileManager::CreateDir("generated/shaders");
            cFileManager::CreateDir("generated/engine_shaders");
            cFileManager::CreateDir("generated/engine_shaders/passes");
            cFileManager::CreateDir("generated/engine_shaders/passes/msaa");
            cFileManager::CreateDir("generated/engine_shaders/text");

            LogInfo("cShaderManager initialized");
        }

        void cShaderManager::Free()
        {
            LogInfo("cShaderManager::Free()");
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

        sShaderStage* cShaderManager::GetFromFile(const sShaderStage::eType &type, const char *filepath)
        {
            if (s_Stages->find(filepath) != s_Stages->end()) {
                return &s_Stages->at(filepath);
            }
            string src = cFileManager::ReadFileWithIncludes(filepath, "#include");
            WriteGeneratedShader(filepath, src);
            return GetFromSrc(type, filepath, src);
        }

        sShaderStage* cShaderManager::GetFromSrc(
                const sShaderStage::eType &type,
                const string &id,
                const string &src
        ) {
            if (src.empty()) {
                LogError("{} shader stage source is empty", id);
                return nullptr;
            }

            string profile;
            string entryPoint;

            switch (type) {

                case sShaderStage::eType::VERTEX:
                    profile = K_PROFILE_VERTEX;
                    entryPoint = K_ENTRY_POINT_VERTEX;
                    break;

                case sShaderStage::eType::PIXEL:
                    profile = K_PROFILE_PIXEL;
                    entryPoint = K_ENTRY_POINT_PIXEL;
                    break;

                case sShaderStage::eType::GEOMETRY:
                    profile = K_PROFILE_GEOMETRY;
                    entryPoint = K_ENTRY_POINT_GEOMETRY;
                    break;

                case sShaderStage::eType::COMPUTE:
                    profile = K_PROFILE_COMPUTE;
                    entryPoint = K_ENTRY_POINT_COMPUTE;
                    break;

                default:
                    LogError("Failed to identify shader stage type for {}", id);
                    return nullptr;

            }

            s_Stages->insert({ id, { Hash(id), type } });
            sShaderStage* stage = &s_Stages->at(id);
            stage->Profile = profile;
            stage->EntryPoint = entryPoint;
            stage->Source = src;
            context::CompileShaderStage(stage);
            context::CreateShaderStage(stage);
            return stage;
        }

        void cShaderManager::ReloadStage(const char* filepath)
        {
            auto& stages = *s_Stages;

            auto it = stages.find(filepath);
            if (it != stages.end()) {
                string src = cFileManager::ReadFileWithIncludes(filepath, "#include");

                if (src.empty()) {
                    LogError("Failed to read shader stage source from {}", filepath);
                    return;
                }

                auto& stage = it->second;
                stage.Source = src;
                stage.Compiled = false;
                context::FreeShaderStage(&stage);
                context::CreateShaderStage(&stage);
            }
        }

        void cShaderManager::FreeStages()
        {
            for (auto& shader : *s_Stages) {
                context::FreeShaderStage(&shader.second);
            }
        }

        void cShaderManager::WriteGeneratedShader(const char *filepath, const string &src)
        {
            hstringstream ss;
            ss << "generated/" << filepath;
            hstring generatedFilepath = ss.str();
            cFileManager::WriteFile(generatedFilepath.c_str(), src);
        }

        void cShaderManager::SetShader(cShader*& dest, cShader* src)
        {
            if (dest == nullptr) {
                dest = src;
                return;
            }

            cShader*& next = dest->Next;
            while (next != nullptr) {
                next = next->Next;
            }
            next = src;
        }

        void cShaderManager::SetShader(cShader*& dest, cShader* src, u64 id)
        {
            if (dest == nullptr) {
                LogWarning("Can't set shader by ID because destination is NULL");
                return;
            }

            cShader*& next = dest->Next;
            while (next != nullptr) {
                if (next->ID == id) {
                    cShader* prev = next->Next;
                    next->Next = src;
                    src->Next = prev;
                    break;
                }
                next = next->Next;
            }
        }

        void cShaderManager::SetShader(cShader* shader)
        {
            switch (shader->Category)
            {
                case cShader::eCategory::PREPASS:
                    SetShader(s_PrepassShaders, shader);
                    break;

                case cShader::eCategory::OPAQUE_GEOMETRY:
                    SetShader(s_OpaqueShaders, shader);
                    break;

                case cShader::eCategory::TRANSPARENT_GEOMETRY:
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

        void cShaderManager::SetShaderAfter(cShader* shader, const string &name)
        {
            u64 id = Hash(name);
            switch (shader->Category)
            {
                case cShader::eCategory::PREPASS:
                    SetShader(s_PrepassShaders, shader, id);
                    break;

                case cShader::eCategory::OPAQUE_GEOMETRY:
                    SetShader(s_OpaqueShaders, shader, id);
                    break;

                case cShader::eCategory::TRANSPARENT_GEOMETRY:
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

        cShader* cShaderManager::GetShaders(cShader::eCategory category)
        {
            switch (category)
            {
                case cShader::eCategory::NONE:        return nullptr;
                case cShader::eCategory::COMPUTE:     return s_ComputeShaders;
                case cShader::eCategory::PREPASS:     return s_PrepassShaders;
                case cShader::eCategory::OPAQUE_GEOMETRY:      return s_OpaqueShaders;
                case cShader::eCategory::TRANSPARENT_GEOMETRY: return s_TransparentShaders;
                case cShader::eCategory::POSTFX:      return s_PostfxShaders;
                case cShader::eCategory::UI:          return s_UiShaders;
                case cShader::eCategory::FINAL:       return s_FinalShaders;
            }
        }

        cShader* cShaderManager::GetShader(const string& name)
        {
            u64 id = Hash(name);
            cShader* shader = nullptr;

            shader = GetShader(s_ComputeShaders, id);
            if (shader != nullptr) {
                return shader;
            }

            shader = GetShader(s_PrepassShaders, id);
            if (shader != nullptr) {
                return shader;
            }

            shader = GetShader(s_OpaqueShaders, id);
            if (shader != nullptr) {
                return shader;
            }

            shader = GetShader(s_TransparentShaders, id);
            if (shader != nullptr) {
                return shader;
            }

            shader = GetShader(s_PostfxShaders, id);
            if (shader != nullptr) {
                return shader;
            }

            shader = GetShader(s_UiShaders, id);
            if (shader != nullptr) {
                return shader;
            }

            shader = GetShader(s_FinalShaders, id);
            if (shader != nullptr) {
                return shader;
            }

            return shader;
        }

        cShader* cShaderManager::GetShader(cShader*& src, u64 id)
        {
            cShader* next = src;
            while (next != nullptr)
            {
                if (next->ID == id) {
                    return next;
                }
                next = next->Next;
            }
            return nullptr;
        }

    }

}