#include <rendering/core/context.hpp>
#include <rendering/core/shader.h>

namespace xpe {

    namespace render {

        sShaderStorage* cShaderManager::s_Storage = nullptr;

        sShaderStorage::~sShaderStorage() {
            Shaders.clear();
            ShaderStages.clear();
        }

        void cShaderManager::Init() {
            LogInfo("cShaderManager::Init()");

            s_Storage = new sShaderStorage();
            cFileManager::CreateDir("generated");
            cFileManager::CreateDir("generated/shaders");
            cFileManager::CreateDir("generated/engine_shaders");
            cFileManager::CreateDir("generated/engine_shaders/passes");
            cFileManager::CreateDir("generated/engine_shaders/passes/msaa");
            cFileManager::CreateDir("generated/engine_shaders/text");

            LogInfo("cShaderManager initialized");
        }

        void cShaderManager::Free() {
            LogInfo("cShaderManager::Free()");
            FreeShaders();
            delete s_Storage;
        }

        sShader* cShaderManager::CreateShader(const string &id) {
            sShader shader;
            shader.Name = id;
            AddShader(id, shader);
            return GetShader(id);
        }

        void cShaderManager::AddVertexStageFromFile(sShader* const shader, const char *filepath) {
            sShaderStage* stage = GetShaderStage(filepath);

            if (stage != nullptr) {
                shader->Stages.emplace_back(stage);
                return;
            }

            string src = cFileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add sVertex stage from filepath {}", filepath);
                return;
            }

            WriteGeneratedShader(filepath, src);

            AddVertexStage(shader, filepath, src);
        }

        void cShaderManager::AddPixelStageFromFile(sShader* const shader, const char *filepath) {
            sShaderStage* stage = GetShaderStage(filepath);

            if (stage != nullptr) {
                shader->Stages.emplace_back(stage);
                return;
            }

            string src = cFileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Pixel stage from filepath {}", filepath);
                return;
            }

            WriteGeneratedShader(filepath, src);

            AddPixelStage(shader, filepath, src);
        }

        void cShaderManager::AddGeometryStageFromFile(sShader* const shader, const char *filepath) {
            sShaderStage* stage = GetShaderStage(filepath);

            if (stage != nullptr) {
                shader->Stages.emplace_back(stage);
                return;
            }

            string src = cFileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add GeometryInstances stage from filepath {}", filepath);
                return;
            }

            WriteGeneratedShader(filepath, src);

            AddGeometryStage(shader, filepath, src);
        }

        void cShaderManager::AddTessControlStageFromFile(sShader* const shader, const char *filepath) {
            sShaderStage* stage = GetShaderStage(filepath);

            if (stage != nullptr) {
                shader->Stages.emplace_back(stage);
                return;
            }

            string src = cFileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Tesselation Control stage from filepath {}", filepath);
                return;
            }

            WriteGeneratedShader(filepath, src);

            AddTessControlStage(shader, filepath, src);
        }

        void cShaderManager::AddTessEvalStageFromFile(sShader* const shader, const char *filepath) {
            sShaderStage* stage = GetShaderStage(filepath);

            if (stage != nullptr) {
                shader->Stages.emplace_back(stage);
                return;
            }

            string src = cFileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Tesselation Evaluation stage from filepath {}", filepath);
                return;
            }

            WriteGeneratedShader(filepath, src);

            AddTessEvalStage(shader, filepath, src);
        }

        void cShaderManager::AddComputeStageFromFile(sShader* const shader, const char *filepath) {
            sShaderStage* stage = cShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                shader->Stages.emplace_back(stage);
                return;
            }

            string src = cFileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Compute stage from filepath {}", filepath);
                return;
            }

            WriteGeneratedShader(filepath, src);

            AddComputeStage(shader, filepath, src);
        }

        void cShaderManager::AddVertexStage(sShader* const shader, const string& id, const string &source) {
            sShaderStage stage = {id, eShaderType::VERTEX };
            stage.EntryPoint = "vs_main";
            stage.Profile = "vs_5_0";
            stage.Source = source;

            AddShaderStage(id, stage);

            shader->Stages.emplace_back(GetShaderStage(id));
        }

        void cShaderManager::AddPixelStage(sShader* const shader, const string& id, const string &source) {
            sShaderStage stage = {id, eShaderType::PIXEL };
            stage.EntryPoint = "ps_main";
            stage.Profile = "ps_5_0";
            stage.Source = source;

            AddShaderStage(id, stage);

            shader->Stages.emplace_back(GetShaderStage(id));
        }

        void cShaderManager::AddGeometryStage(sShader* const shader, const string& id, const string &source) {
            sShaderStage stage = {id, eShaderType::GEOMETRY };
            stage.EntryPoint = "gs_main";
            stage.Profile = "gs_5_0";
            stage.Source = source;

            AddShaderStage(id, stage);

            shader->Stages.emplace_back(GetShaderStage(id));
        }

        void cShaderManager::AddTessControlStage(sShader* const shader, const string& id, const string &source) {
            sShaderStage stage = {id, eShaderType::TESS_CONTROL };
            stage.EntryPoint = "ts_main";
            stage.Profile = "tcs_5_0";
            stage.Source = source;

            AddShaderStage(id, stage);

            shader->Stages.emplace_back(GetShaderStage(id));
        }

        void cShaderManager::AddTessEvalStage(sShader* const shader, const string& id, const string &source) {
            sShaderStage stage = {id, eShaderType::TESS_EVAL };
            stage.EntryPoint = "ts_main";
            stage.Profile = "tes_5_0";
            stage.Source = source;

            AddShaderStage(id, stage);

            shader->Stages.emplace_back(GetShaderStage(id));
        }

        void cShaderManager::AddComputeStage(sShader* const shader, const string& id, const string &source) {
            sShaderStage stage = {id, eShaderType::COMPUTE };
            stage.EntryPoint = "cs_main";
            stage.Profile = "cs_5_0";
            stage.Source = source;

            AddShaderStage(id, stage);

            shader->Stages.emplace_back(GetShaderStage(id));
        }

        void cShaderManager::BuildShader(sShader *const shader) {
            context::CompileShader(*shader);
            context::CreateShader(*shader);
        }

        void cShaderManager::BuildShader(const string &id) {
            BuildShader(GetShader(id));
        }

        void cShaderManager::AddShaderStage(const string& filepath, const sShaderStage &shaderStage) {
            s_Storage->ShaderStages.insert({ filepath, shaderStage });
        }

        void cShaderManager::RemoveShaderStage(const string& name) {
            s_Storage->ShaderStages.erase(name);
        }

        sShaderStage* cShaderManager::GetShaderStage(const string &name) {
            auto& stages = s_Storage->ShaderStages;
            auto it = stages.find(name);

            if (it != stages.end()) {
                return &it->second;
            }

            return nullptr;
        }

        void cShaderManager::ReloadStage(const char* filepath) {
            auto& stages = s_Storage->ShaderStages;

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
                context::FreeShaderStage(stage);
                context::CreateShaderStage(stage);
            }
        }

        void cShaderManager::AddShader(const string &id, const sShader& shader) {
            s_Storage->Shaders[id] = shader;
        }

        void cShaderManager::FreeShader(const string &id) {
            auto& shaders = s_Storage->Shaders;

            auto it = shaders.find(id);
            if (it != shaders.end()) {
                context::FreeShader(it->second);
            }
        }

        void cShaderManager::FreeShaders() {
            for (auto& shader : s_Storage->Shaders) {
                context::FreeShader(shader.second);
            }
        }

        sShader* cShaderManager::GetShader(const string &id) {
            auto& shaders = s_Storage->Shaders;

            auto it = shaders.find(id);
            if (it != shaders.end()) {
                return &shaders[id];
            }

            return nullptr;
        }

        void cShaderManager::WriteGeneratedShader(const char *filepath, const string &src) {
            hstringstream ss;
            ss << "generated/" << filepath;
            hstring generatedFilepath = ss.str();
            cFileManager::WriteFile(generatedFilepath.c_str(), src);
        }

    }

}