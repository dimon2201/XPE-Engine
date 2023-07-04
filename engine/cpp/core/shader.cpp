#include <core/shader.h>

namespace xpe {

    namespace core {

        Shader* ShaderBuilder::Build(const string& name) {
            m_Context->CreateShader(*m_Shader);
            ShaderManager::AddShader(name, *m_Shader);
            return m_Shader;
        }

        ShaderBuilder& ShaderBuilder::AddVertexStageFromFile(const char *filepath) {
            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Vertex stage from filepath {}", filepath);
                return *this;
            }

            return AddVertexStage(src);
        }

        ShaderBuilder& ShaderBuilder::AddPixelStageFromFile(const char *filepath) {
            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Pixel stage from filepath {}", filepath);
                return *this;
            }

            return AddPixelStage(src);
        }

        ShaderBuilder& ShaderBuilder::AddGeometryStageFromFile(const char *filepath) {
            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Geometry stage from filepath {}", filepath);
                return *this;
            }

            return AddGeometryStage(src);
        }

        ShaderBuilder& ShaderBuilder::AddTessControlStageFromFile(const char *filepath) {
            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Tesselation Control stage from filepath {}", filepath);
                return *this;
            }

            return AddTessControlStage(src);
        }

        ShaderBuilder& ShaderBuilder::AddTessEvalStageFromFile(const char *filepath) {
            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Tesselation Evaluation stage from filepath {}", filepath);
                return *this;
            }

            return AddTessEvalStage(src);
        }

        ShaderBuilder& ShaderBuilder::AddComputeStageFromFile(const char *filepath) {
            string src = FileManager::ReadFileWithIncludes(filepath, "#include");

            if (src.empty()) {
                LogError("Failed to add Compute stage from filepath {}", filepath);
                return *this;
            }

            return AddComputeStage(src);
        }

        ShaderBuilder& ShaderBuilder::AddVertexStage(const string &source) {
            ShaderStage stage;
            stage.Type = eShaderType::VERTEX;
            stage.EntryPoint = "vs_main";
            stage.Profile = "vs_4_0";
            stage.Source = source;
            m_Shader->Stages.emplace_back(stage);
            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddPixelStage(const string &source) {
            ShaderStage stage;
            stage.Type = eShaderType::PIXEL;
            stage.EntryPoint = "ps_main";
            stage.Profile = "ps_4_0";
            stage.Source = source;
            m_Shader->Stages.emplace_back(stage);
            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddGeometryStage(const string &source) {
            ShaderStage stage;
            stage.Type = eShaderType::GEOMETRY;
            stage.EntryPoint = "gs_main";
            stage.Profile = "gs_4_0";
            stage.Source = source;
            m_Shader->Stages.emplace_back(stage);
            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddTessControlStage(const string &source) {
            ShaderStage stage;
            stage.Type = eShaderType::TESS_CONTROL;
            stage.EntryPoint = "tc_main";
            stage.Profile = "tcs_4_0";
            stage.Source = source;
            m_Shader->Stages.emplace_back(stage);
            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddTessEvalStage(const string &source) {
            ShaderStage stage;
            stage.Type = eShaderType::TESS_EVAL;
            stage.EntryPoint = "te_main";
            stage.Profile = "tes_4_0";
            stage.Source = source;
            m_Shader->Stages.emplace_back(stage);
            return *this;
        }

        ShaderBuilder& ShaderBuilder::AddComputeStage(const string &source) {
            ShaderStage stage;
            stage.Type = eShaderType::COMPUTE;
            stage.EntryPoint = "cs_main";
            stage.Profile = "cs_4_0";
            stage.Source = source;
            m_Shader->Stages.emplace_back(stage);
            return *this;
        }

        RenderingContext_Interface* ShaderManager::s_Context = nullptr;
        ShaderBuilder ShaderManager::s_ShaderBuilder;
        unordered_map<string, Shader> ShaderManager::s_ShaderTable;
        Shader ShaderManager::s_TempShader;

        void ShaderManager::Init(RenderingContext_Interface* context) {
            s_Context = context;
            s_ShaderBuilder = { context, &s_TempShader };
        }

        void ShaderManager::Free() {
            for (const auto& shader : s_ShaderTable) {
                s_Context->FreeShader(shader.second);
            }
            s_ShaderTable.clear();
        }

        ShaderBuilder& ShaderManager::Builder() {
            s_TempShader = {};
            s_ShaderBuilder = { s_Context, &s_TempShader };
            return s_ShaderBuilder;
        }

        void ShaderManager::AddShader(const string& name, const Shader &shader) {
            s_ShaderTable[name] = shader;
        }

        void ShaderManager::RemoveShader(const string& name) {
            s_ShaderTable.erase(name);
        }

    }

}