#include <rendering/core/context.hpp>
#include <rendering/core/shader.h>

namespace xpe {

    namespace render {

        ShaderStorage* ShaderManager::s_Storage = nullptr;

        ShaderStorage::~ShaderStorage() {
            Shaders.clear();
            ShaderStages.clear();
        }

        void ShaderManager::Init() {
            LogInfo("ShaderManager::Init()");

            s_Storage = new ShaderStorage();
            FileManager::CreateDir("generated");
            FileManager::CreateDir("generated/shaders");
            FileManager::CreateDir("generated/engine_shaders");
            FileManager::CreateDir("generated/engine_shaders/passes");
            FileManager::CreateDir("generated/engine_shaders/passes/msaa");
            FileManager::CreateDir("generated/engine_shaders/text");

            LogInfo("ShaderManager initialized");
        }

        void ShaderManager::Free() {
            LogInfo("ShaderManager::Free()");
            FreeShaders();
            delete s_Storage;
        }

        Shader* ShaderManager::CreateShader(const string &id) {
            Shader shader;
            shader.Name = id;
            AddShader(id, shader);
            return GetShader(id);
        }

        void ShaderManager::AddVertexStageFromFile(Shader* const shader, const char *filepath) {
            ShaderStage* stage = GetShaderStage(filepath);

            if (stage != nullptr) {
                shader->Stages.emplace_back(stage);
                return;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Vertex stage from filepath {}", filepath);
                return;
            }

            WriteGeneratedShader(filepath, src);

            AddVertexStage(shader, filepath, src);
        }

        void ShaderManager::AddPixelStageFromFile(Shader* const shader, const char *filepath) {
            ShaderStage* stage = GetShaderStage(filepath);

            if (stage != nullptr) {
                shader->Stages.emplace_back(stage);
                return;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Pixel stage from filepath {}", filepath);
                return;
            }

            WriteGeneratedShader(filepath, src);

            AddPixelStage(shader, filepath, src);
        }

        void ShaderManager::AddGeometryStageFromFile(Shader* const shader, const char *filepath) {
            ShaderStage* stage = GetShaderStage(filepath);

            if (stage != nullptr) {
                shader->Stages.emplace_back(stage);
                return;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add GeometryInstances stage from filepath {}", filepath);
                return;
            }

            WriteGeneratedShader(filepath, src);

            AddGeometryStage(shader, filepath, src);
        }

        void ShaderManager::AddTessControlStageFromFile(Shader* const shader, const char *filepath) {
            ShaderStage* stage = GetShaderStage(filepath);

            if (stage != nullptr) {
                shader->Stages.emplace_back(stage);
                return;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Tesselation Control stage from filepath {}", filepath);
                return;
            }

            WriteGeneratedShader(filepath, src);

            AddTessControlStage(shader, filepath, src);
        }

        void ShaderManager::AddTessEvalStageFromFile(Shader* const shader, const char *filepath) {
            ShaderStage* stage = GetShaderStage(filepath);

            if (stage != nullptr) {
                shader->Stages.emplace_back(stage);
                return;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Tesselation Evaluation stage from filepath {}", filepath);
                return;
            }

            WriteGeneratedShader(filepath, src);

            AddTessEvalStage(shader, filepath, src);
        }

        void ShaderManager::AddComputeStageFromFile(Shader* const shader, const char *filepath) {
            ShaderStage* stage = ShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                shader->Stages.emplace_back(stage);
                return;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Compute stage from filepath {}", filepath);
                return;
            }

            WriteGeneratedShader(filepath, src);

            AddComputeStage(shader, filepath, src);
        }

        void ShaderManager::AddVertexStage(Shader* const shader, const string& id, const string &source) {
            ShaderStage stage = { id, eShaderType::VERTEX };
            stage.EntryPoint = "vs_main";
            stage.Profile = "vs_5_0";
            stage.Source = source;

            AddShaderStage(id, stage);

            shader->Stages.emplace_back(GetShaderStage(id));
        }

        void ShaderManager::AddPixelStage(Shader* const shader, const string& id, const string &source) {
            ShaderStage stage = { id, eShaderType::PIXEL };
            stage.EntryPoint = "ps_main";
            stage.Profile = "ps_5_0";
            stage.Source = source;

            AddShaderStage(id, stage);

            shader->Stages.emplace_back(GetShaderStage(id));
        }

        void ShaderManager::AddGeometryStage(Shader* const shader, const string& id, const string &source) {
            ShaderStage stage = { id, eShaderType::GEOMETRY };
            stage.EntryPoint = "gs_main";
            stage.Profile = "gs_5_0";
            stage.Source = source;

            AddShaderStage(id, stage);

            shader->Stages.emplace_back(GetShaderStage(id));
        }

        void ShaderManager::AddTessControlStage(Shader* const shader, const string& id, const string &source) {
            ShaderStage stage = { id, eShaderType::TESS_CONTROL };
            stage.EntryPoint = "tc_main";
            stage.Profile = "tcs_5_0";
            stage.Source = source;

            AddShaderStage(id, stage);

            shader->Stages.emplace_back(GetShaderStage(id));
        }

        void ShaderManager::AddTessEvalStage(Shader* const shader, const string& id, const string &source) {
            ShaderStage stage = { id, eShaderType::TESS_EVAL };
            stage.EntryPoint = "te_main";
            stage.Profile = "tes_5_0";
            stage.Source = source;

            AddShaderStage(id, stage);

            shader->Stages.emplace_back(GetShaderStage(id));
        }

        void ShaderManager::AddComputeStage(Shader* const shader, const string& id, const string &source) {
            ShaderStage stage = { id, eShaderType::COMPUTE };
            stage.EntryPoint = "cs_main";
            stage.Profile = "cs_5_0";
            stage.Source = source;

            AddShaderStage(id, stage);

            shader->Stages.emplace_back(GetShaderStage(id));
        }

        void ShaderManager::BuildShader(Shader *const shader) {
            context::CompileShader(*shader);
            context::CreateShader(*shader);
        }

        void ShaderManager::BuildShader(const string &id) {
            BuildShader(GetShader(id));
        }

        void ShaderManager::AddShaderStage(const string& filepath, const ShaderStage &shaderStage) {
            s_Storage->ShaderStages.insert({ filepath, shaderStage });
        }

        void ShaderManager::RemoveShaderStage(const string& name) {
            s_Storage->ShaderStages.erase(name);
        }

        ShaderStage* ShaderManager::GetShaderStage(const string &name) {
            auto& stages = s_Storage->ShaderStages;
            auto it = stages.find(name);

            if (it != stages.end()) {
                return &it->second;
            }

            return nullptr;
        }

        void ShaderManager::ReloadStage(const char* filepath) {
            auto& stages = s_Storage->ShaderStages;

            auto it = stages.find(filepath);
            if (it != stages.end()) {
                string src = FileManager::ReadFileWithIncludes(filepath, "#include");

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

        void ShaderManager::AddShader(const string &id, const Shader& shader) {
            s_Storage->Shaders[id] = shader;
        }

        void ShaderManager::FreeShader(const string &id) {
            auto& shaders = s_Storage->Shaders;

            auto it = shaders.find(id);
            if (it != shaders.end()) {
                context::FreeShader(it->second);
            }
        }

        void ShaderManager::FreeShaders() {
            for (auto& shader : s_Storage->Shaders) {
                context::FreeShader(shader.second);
            }
        }

        Shader* ShaderManager::GetShader(const string &id) {
            auto& shaders = s_Storage->Shaders;

            auto it = shaders.find(id);
            if (it != shaders.end()) {
                return &shaders[id];
            }

            return nullptr;
        }

        void ShaderManager::WriteGeneratedShader(const char *filepath, const string &src) {
            hstringstream ss;
            ss << "generated/" << filepath;
            hstring generatedFilepath = ss.str();
            FileManager::WriteFile(generatedFilepath.c_str(), src);
        }

    }

}