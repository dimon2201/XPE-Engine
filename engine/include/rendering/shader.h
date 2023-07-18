#pragma once

namespace xpe {

    namespace render {

        class Context;

        enum class eShaderType {
            VERTEX = 0,
            PIXEL = 1,
            GEOMETRY = 2,
            TESS_EVAL = 3,
            TESS_CONTROL = 4,
            COMPUTE = 5,
        };

        struct ENGINE_API ShaderStage final {
            eShaderType Type;
            GPUResource Resource;
            Blob Blob;
            string Source;
            const char* EntryPoint = nullptr;
            const char* Profile = nullptr;
            uword Flag = 0;
            bool Compiled = false;

            ShaderStage() = default;
            ShaderStage(eShaderType type) : Type(type) {}
        };

        struct ENGINE_API Shader {
            string ID = "Untitled";
            vector<ShaderStage*> Stages;
        };

        class ENGINE_API ShaderBuilder : public Object {

        public:
            ShaderBuilder() = default;

            ShaderBuilder(Context* context) : m_Context(context) {}

            ~ShaderBuilder() = default;

        public:
            Shader* Build(const string& shaderId);

            ShaderBuilder& AddVertexStageFromFile(const char* filepath);
            ShaderBuilder& AddPixelStageFromFile(const char* filepath);
            ShaderBuilder& AddGeometryStageFromFile(const char* filepath);
            ShaderBuilder& AddTessControlStageFromFile(const char* filepath);
            ShaderBuilder& AddTessEvalStageFromFile(const char* filepath);
            ShaderBuilder& AddComputeStageFromFile(const char* filepath);

            ShaderBuilder& AddVertexStage(const string& id, const string& source);
            ShaderBuilder& AddPixelStage(const string& id, const string& source);
            ShaderBuilder& AddGeometryStage(const string& id, const string& source);
            ShaderBuilder& AddTessControlStage(const string& id, const string& source);
            ShaderBuilder& AddTessEvalStage(const string& id, const string& source);
            ShaderBuilder& AddComputeStage(const string& id, const string& source);

        private:
            Context* m_Context = nullptr;
            Shader m_Shader;
        };

        struct ENGINE_API ShaderStorage : public Object {
            unordered_map<string, ShaderStage> ShaderStages;
            unordered_map<string, Shader> Shaders;

            ~ShaderStorage();
        };

        class ENGINE_API ShaderManager final {

        public:
            static void Init(Context* context);
            static void Free();

            static ShaderBuilder& Builder();

            static void AddShaderStage(const string& filepath, const ShaderStage& shaderStage);
            static void RemoveShaderStage(const string& filepath);
            static ShaderStage* GetShaderStage(const string& filepath);

            static void ReloadStage(const char* filepath);

            static void AddShader(const string& id, const Shader& shader);
            static void FreeShader(const string& id);
            static Shader* GetShader(const string& id);

            static void WatchShaders(const string& dirName, bool recursive = false);
            static void UpdateShaderWatches();

        private:
            static Context* s_Context;
            static ShaderStorage* s_Storage;
            static ShaderBuilder* s_ShaderBuilder;
            static platform::FileWatcher* s_FileWatcher;
        };

    }

}