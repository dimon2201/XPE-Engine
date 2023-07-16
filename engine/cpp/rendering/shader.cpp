#include <rendering/context.hpp>
#include <rendering/shader.h>

namespace xpe {

    namespace render {

        Shader* ShaderBuilder::Build(const string& shaderId) {
            m_Shader.ID = shaderId;
            m_Context->CreateShader(m_Shader);
            ShaderManager::AddShader(shaderId, m_Shader);
            return ShaderManager::GetShader(shaderId);
        }

        ShaderBuilder& ShaderBuilder::AddVertexStageFromFile(const char *filepath) {
            ShaderStage* stage = ShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                m_Shader.Stages.emplace_back(stage);
                return *this;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Vertex stage from filepath {}", filepath);
                return *this;
            }

            AddVertexStage(filepath, src);

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddPixelStageFromFile(const char *filepath) {
            ShaderStage* stage = ShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                m_Shader.Stages.emplace_back(stage);
                return *this;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Pixel stage from filepath {}", filepath);
                return *this;
            }

            AddPixelStage(filepath, src);

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddGeometryStageFromFile(const char *filepath) {
            ShaderStage* stage = ShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                m_Shader.Stages.emplace_back(stage);
                return *this;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add GeometryInstances stage from filepath {}", filepath);
                return *this;
            }

            AddGeometryStage(filepath, src);

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddTessControlStageFromFile(const char *filepath) {
            ShaderStage* stage = ShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                m_Shader.Stages.emplace_back(stage);
                return *this;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Tesselation Control stage from filepath {}", filepath);
                return *this;
            }

            AddTessControlStage(filepath, src);

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddTessEvalStageFromFile(const char *filepath) {
            ShaderStage* stage = ShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                m_Shader.Stages.emplace_back(stage);
                return *this;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Tesselation Evaluation stage from filepath {}", filepath);
                return *this;
            }

            AddTessEvalStage(filepath, src);

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddComputeStageFromFile(const char *filepath) {
            ShaderStage* stage = ShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                m_Shader.Stages.emplace_back(stage);
                return *this;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Compute stage from filepath {}", filepath);
                return *this;
            }

            AddComputeStage(filepath, src);

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddVertexStage(const string& id, const string &source) {
            ShaderStage stage = eShaderType::VERTEX;
            stage.EntryPoint = "vs_main";
            stage.Profile = "vs_5_0";
            stage.Source = source;

            ShaderManager::AddShaderStage(id, stage);

            m_Shader.Stages.emplace_back(ShaderManager::GetShaderStage(id));

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddPixelStage(const string& id, const string &source) {
            ShaderStage stage = eShaderType::PIXEL;
            stage.EntryPoint = "ps_main";
            stage.Profile = "ps_5_0";
            stage.Source = source;

            ShaderManager::AddShaderStage(id, stage);

            m_Shader.Stages.emplace_back(ShaderManager::GetShaderStage(id));

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddGeometryStage(const string& id, const string &source) {
            ShaderStage stage = eShaderType::GEOMETRY;
            stage.EntryPoint = "gs_main";
            stage.Profile = "gs_5_0";
            stage.Source = source;

            ShaderManager::AddShaderStage(id, stage);

            m_Shader.Stages.emplace_back(ShaderManager::GetShaderStage(id));

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddTessControlStage(const string& id, const string &source) {
            ShaderStage stage = eShaderType::TESS_CONTROL;
            stage.EntryPoint = "tc_main";
            stage.Profile = "tcs_5_0";
            stage.Source = source;

            ShaderManager::AddShaderStage(id, stage);

            m_Shader.Stages.emplace_back(ShaderManager::GetShaderStage(id));

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddTessEvalStage(const string& id, const string &source) {
            ShaderStage stage = eShaderType::TESS_EVAL;
            stage.EntryPoint = "te_main";
            stage.Profile = "tes_5_0";
            stage.Source = source;

            ShaderManager::AddShaderStage(id, stage);

            m_Shader.Stages.emplace_back(ShaderManager::GetShaderStage(id));

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddComputeStage(const string& id, const string &source) {
            ShaderStage stage = eShaderType::COMPUTE;
            stage.EntryPoint = "cs_main";
            stage.Profile = "cs_5_0";
            stage.Source = source;

            ShaderManager::AddShaderStage(id, stage);

            m_Shader.Stages.emplace_back(ShaderManager::GetShaderStage(id));

            return *this;
        }

        Context* ShaderManager::s_Context = nullptr;

        ShaderBuilder ShaderManager::s_ShaderBuilder;

        unordered_map<string, ShaderStage> ShaderManager::s_ShaderStageTable;
        unordered_map<string, Shader> ShaderManager::s_ShaderTable;

        platform::FileWatcher* ShaderManager::s_FileWatcher = nullptr;

        void ShaderManager::Init(Context* context) {
            LogInfo("ShaderManager::Init()");

            s_Context = context;
            s_ShaderBuilder = { context };
            s_FileWatcher = new platform::FileWatcher();

            LogInfo("ShaderManager initialized");
        }

        void ShaderManager::Free() {
            LogInfo("ShaderManager::Free()");

            delete s_FileWatcher;

            for (auto& shader : s_ShaderTable) {
                s_Context->FreeShader(shader.second);
            }
            s_ShaderTable.clear();
            s_ShaderStageTable.clear();
        }

        ShaderBuilder& ShaderManager::Builder() {
            s_ShaderBuilder = { s_Context };
            return s_ShaderBuilder;
        }

        void ShaderManager::AddShaderStage(const string& filepath, const ShaderStage &shaderStage) {
            s_ShaderStageTable.insert({ filepath, shaderStage });
        }

        void ShaderManager::RemoveShaderStage(const string& name) {
            s_ShaderStageTable.erase(name);
        }

        ShaderStage* ShaderManager::GetShaderStage(const string &name) {
            auto it = s_ShaderStageTable.find(name);

            if (it != s_ShaderStageTable.end()) {
                return &it->second;
            }

            return nullptr;
        }

        void ShaderManager::ReloadStage(const char* filepath) {
            auto it = s_ShaderStageTable.find(filepath);

            if (it != s_ShaderStageTable.end()) {
                string src = FileManager::ReadFileWithIncludes(filepath, "#include");

                if (src.empty()) {
                    LogError("Failed to read shader stage source from {}", filepath);
                    return;
                }

                auto& stage = it->second;
                stage.Source = src;
                stage.Compiled = false;
                s_Context->FreeShaderStage(stage);
                s_Context->CreateShaderStage(stage);
            }
        }

        void ShaderManager::AddShader(const string &id, const Shader& shader) {
            s_ShaderTable[id] = shader;
        }

        void ShaderManager::FreeShader(const string &id) {
            auto it = s_ShaderTable.find(id);

            if (it != s_ShaderTable.end()) {
                s_Context->FreeShader(it->second);
            }
        }

        Shader* ShaderManager::GetShader(const string &id) {
            auto it = s_ShaderTable.find(id);

            if (it != s_ShaderTable.end()) {
                return &s_ShaderTable[id];
            }

            return nullptr;
        }

        static void OnShaderFileModified(
                void* thiz,
                platform::FileWatchID watchId,
                const string& dirname,
                const string& filename
        ) {
            std::stringstream ss;
            ss << dirname << "/" << filename;
            std::string filepath = ss.str();

            LogInfo("ShaderManager: Shader file changed {}", filepath);

            ShaderManager::ReloadStage(filepath.c_str());
        }

        void ShaderManager::WatchShaders(const string& dirName, bool recursive) {
            auto* watch = s_FileWatcher->CreateWatch(dirName, recursive);
            watch->FileModifiedEvent = { OnShaderFileModified, 1 };
            watch->FileAddedEvent = { OnShaderFileModified, 1 };
            watch->FileDeletedEvent = { OnShaderFileModified, 1 };
        }

        void ShaderManager::UpdateShaderWatches() {
            s_FileWatcher->Update();
        }

    }

}