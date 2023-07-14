#include <rendering/context.hpp>
#include <rendering/shader.h>

namespace xpe {

    namespace render {

        Shader* ShaderBuilder::Build() {
            m_Context->CreateShader(*m_Shader);
            return m_Shader;
        }

        ShaderBuilder& ShaderBuilder::AddVertexStageFromFile(const char *filepath) {
            ShaderStage* stage = ShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                m_Shader->Stages.emplace_back(*stage);
                return *this;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Vertex stage from filepath {}", filepath);
                return *this;
            }

            AddVertexStage(src);

            ShaderManager::AddShaderStage(filepath, m_Shader->Stages.back());

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddPixelStageFromFile(const char *filepath) {
            ShaderStage* stage = ShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                m_Shader->Stages.emplace_back(*stage);
                return *this;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Pixel stage from filepath {}", filepath);
                return *this;
            }

            AddPixelStage(src);

            ShaderManager::AddShaderStage(filepath, m_Shader->Stages.back());

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddGeometryStageFromFile(const char *filepath) {
            ShaderStage* stage = ShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                m_Shader->Stages.emplace_back(*stage);
                return *this;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add GeometryInstances stage from filepath {}", filepath);
                return *this;
            }

            AddGeometryStage(src);

            ShaderManager::AddShaderStage(filepath, m_Shader->Stages.back());

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddTessControlStageFromFile(const char *filepath) {
            ShaderStage* stage = ShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                m_Shader->Stages.emplace_back(*stage);
                return *this;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Tesselation Control stage from filepath {}", filepath);
                return *this;
            }

            AddTessControlStage(src);

            ShaderManager::AddShaderStage(filepath, m_Shader->Stages.back());

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddTessEvalStageFromFile(const char *filepath) {
            ShaderStage* stage = ShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                m_Shader->Stages.emplace_back(*stage);
                return *this;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Tesselation Evaluation stage from filepath {}", filepath);
                return *this;
            }

            AddTessEvalStage(src);

            ShaderManager::AddShaderStage(filepath, m_Shader->Stages.back());

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddComputeStageFromFile(const char *filepath) {
            ShaderStage* stage = ShaderManager::GetShaderStage(filepath);

            if (stage != nullptr) {
                m_Shader->Stages.emplace_back(*stage);
                return *this;
            }

            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Compute stage from filepath {}", filepath);
                return *this;
            }

            AddComputeStage(src);

            ShaderManager::AddShaderStage(filepath, m_Shader->Stages.back());

            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddVertexStage(const string &source) {
            ShaderStage stage;
            stage.Type = eShaderType::VERTEX;
            stage.EntryPoint = "vs_main";
            stage.Profile = "vs_5_0";
            stage.Source = source;
            m_Shader->Stages.emplace_back(stage);
            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddPixelStage(const string &source) {
            ShaderStage stage;
            stage.Type = eShaderType::PIXEL;
            stage.EntryPoint = "ps_main";
            stage.Profile = "ps_5_0";
            stage.Source = source;
            m_Shader->Stages.emplace_back(stage);
            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddGeometryStage(const string &source) {
            ShaderStage stage;
            stage.Type = eShaderType::GEOMETRY;
            stage.EntryPoint = "gs_main";
            stage.Profile = "gs_5_0";
            stage.Source = source;
            m_Shader->Stages.emplace_back(stage);
            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddTessControlStage(const string &source) {
            ShaderStage stage;
            stage.Type = eShaderType::TESS_CONTROL;
            stage.EntryPoint = "tc_main";
            stage.Profile = "tcs_5_0";
            stage.Source = source;
            m_Shader->Stages.emplace_back(stage);
            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddTessEvalStage(const string &source) {
            ShaderStage stage;
            stage.Type = eShaderType::TESS_EVAL;
            stage.EntryPoint = "te_main";
            stage.Profile = "tes_5_0";
            stage.Source = source;
            m_Shader->Stages.emplace_back(stage);
            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddComputeStage(const string &source) {
            ShaderStage stage;
            stage.Type = eShaderType::COMPUTE;
            stage.EntryPoint = "cs_main";
            stage.Profile = "cs_5_0";
            stage.Source = source;
            m_Shader->Stages.emplace_back(stage);
            return *this;
        }

        Context* ShaderManager::s_Context = nullptr;
        ShaderBuilder ShaderManager::s_ShaderBuilder;
        unordered_map<string, ShaderStage> ShaderManager::s_ShaderStageTable;
        Shader ShaderManager::s_TempShader;

        void ShaderManager::Init(Context* context) {
            s_Context = context;
            s_ShaderBuilder = { context, &s_TempShader };
        }

        void ShaderManager::Free() {
            Shader shader;
            for (auto& stage : s_ShaderStageTable) {
                shader.Stages.emplace_back(stage.second);
            }
            s_Context->FreeShader(shader);
            s_ShaderStageTable.clear();
        }

        ShaderBuilder& ShaderManager::Builder() {
            s_TempShader = {};
            s_ShaderBuilder = { s_Context, &s_TempShader };
            return s_ShaderBuilder;
        }

        void ShaderManager::AddShaderStage(const string& name, const ShaderStage &shaderStage) {
            s_ShaderStageTable[name] = shaderStage;
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

    }

}