#include <rendering/core/context.hpp>
#include <rendering/core/shader.hpp>

namespace xpe {

    namespace render {

        unordered_map<string, sShaderStage>* cShaderManager::s_Stages = nullptr;
        vector<cShader*>* cShaderManager::s_ComputeShaders;
        vector<cShader*>* cShaderManager::s_PrepassShaders;
        vector<cShader*>* cShaderManager::s_OpaqueShaders;
        vector<cShader*>* cShaderManager::s_TransparentShaders;
        vector<cShader*>* cShaderManager::s_PostfxShaders;
        vector<cShader*>* cShaderManager::s_UiShaders;
        vector<cShader*>* cShaderManager::s_FinalShaders;

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

            if (Category != eCategory::NONE)
            {
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
            } else {
                context::BindPSStage(nullptr);
            }

            if (GeometryStage != nullptr) {
                context::BindGSStage(GeometryStage);
            }

            if (RenderTarget != nullptr) {
                RenderTarget->Bind();
            }
            
            if (Viewport != nullptr) {
                context::BindViewport(Viewport);
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

            s_ComputeShaders = new vector<cShader*>();
            s_PrepassShaders = new vector<cShader*>();
            s_OpaqueShaders = new vector<cShader*>();
            s_TransparentShaders = new vector<cShader*>();
            s_PostfxShaders = new vector<cShader*>();
            s_UiShaders = new vector<cShader*>();
            s_FinalShaders = new vector<cShader*>();

            s_ComputeShaders->resize(s_MaxShaderCount);
            s_PrepassShaders->resize(s_MaxShaderCount);
            s_OpaqueShaders->resize(s_MaxShaderCount);
            s_TransparentShaders->resize(s_MaxShaderCount);
            s_PostfxShaders->resize(s_MaxShaderCount);
            s_UiShaders->resize(s_MaxShaderCount);
            s_FinalShaders->resize(s_MaxShaderCount);

            for (usize i = 0; i < s_MaxShaderCount; i++)
            {
                (*s_ComputeShaders)[i] = nullptr;
                (*s_PrepassShaders)[i] = nullptr;
                (*s_OpaqueShaders)[i] = nullptr;
                (*s_TransparentShaders)[i] = nullptr;
                (*s_PostfxShaders)[i] = nullptr;
                (*s_UiShaders)[i] = nullptr;
                (*s_FinalShaders)[i] = nullptr;
            }

            LogInfo("cShaderManager initialized");
        }

        void cShaderManager::Free()
        {
            LogInfo("cShaderManager::Free()");
            FreeStages();
            delete s_Stages;
            FreeShaders(s_ComputeShaders);
            FreeShaders(s_PrepassShaders);
            FreeShaders(s_OpaqueShaders);
            FreeShaders(s_TransparentShaders);
            FreeShaders(s_PostfxShaders);
            FreeShaders(s_UiShaders);
            FreeShaders(s_FinalShaders);
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

        void cShaderManager::FreeShaders(vector<cShader*>* shaders)
        {
            for (auto shader : *shaders) {
                if (shader != nullptr) {
                    delete shader;
                }
            }
        }

        void cShaderManager::WriteGeneratedShader(const char *filepath, const string &src)
        {
            hstringstream ss;
            ss << "generated/" << filepath;
            hstring generatedFilepath = ss.str();
            cFileManager::WriteFile(generatedFilepath.c_str(), src);
        }

        void cShaderManager::SetShader(cShader* shader, u32 slot)
        {
            switch (shader->Category)
            {
                case cShader::eCategory::PREPASS:
                    (*s_PrepassShaders)[slot] = shader;
                    break;

                case cShader::eCategory::OPAQUE_GEOMETRY:
                    (*s_OpaqueShaders)[slot] = shader;
                    break;

                case cShader::eCategory::TRANSPARENT_GEOMETRY:
                    (*s_TransparentShaders)[slot] = shader;
                    break;

                case cShader::eCategory::POSTFX:
                    (*s_PostfxShaders)[slot] = shader;
                    break;

                case cShader::eCategory::UI:
                    (*s_UiShaders)[slot] = shader;
                    break;

                case cShader::eCategory::FINAL:
                    (*s_FinalShaders)[slot] = shader;
                    break;
            }
        }

        vector<cShader*>* cShaderManager::GetShaders(const cShader::eCategory& category)
        {
            switch (category)
            {
                case cShader::eCategory::NONE:        return nullptr;
                case cShader::eCategory::PREPASS:     return s_PrepassShaders;
                case cShader::eCategory::OPAQUE_GEOMETRY:      return s_OpaqueShaders;
                case cShader::eCategory::TRANSPARENT_GEOMETRY: return s_TransparentShaders;
                case cShader::eCategory::POSTFX:      return s_PostfxShaders;
                case cShader::eCategory::UI:          return s_UiShaders;
                case cShader::eCategory::FINAL:       return s_FinalShaders;
                default: return nullptr;
            }
        }

        cShader* cShaderManager::GetShader(const cShader::eCategory& category, const string& name)
        {
            switch (category)
            {
                case cShader::eCategory::NONE:        return nullptr;
                case cShader::eCategory::PREPASS:     return GetShader(s_PrepassShaders, name);
                case cShader::eCategory::OPAQUE_GEOMETRY:      return GetShader(s_OpaqueShaders, name);
                case cShader::eCategory::TRANSPARENT_GEOMETRY: return GetShader(s_TransparentShaders, name);
                case cShader::eCategory::POSTFX:      return GetShader(s_PostfxShaders, name);
                case cShader::eCategory::UI:          return GetShader(s_UiShaders, name);
                case cShader::eCategory::FINAL:       return GetShader(s_FinalShaders, name);
                default: return nullptr;
            }
        }

        cShader* cShaderManager::GetShader(const vector<cShader*>* shaders, const string& name)
        {
            for (auto shader : *shaders) {
                if (shader != nullptr && shader->Name == name) {
                    return shader;
                }
            }

            return nullptr;
        }

    }

}