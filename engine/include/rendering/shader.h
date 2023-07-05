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
        };

        struct ENGINE_API Shader {
            ePrimitiveTopology PrimitiveTopology;
            vector<ShaderStage> Stages;
            Blob* VertexBlob = nullptr;
        };

        class ENGINE_API ShaderBuilder final {

        public:
            ShaderBuilder() = default;

            ShaderBuilder(Context* context, Shader* shader)
            : m_Context(context), m_Shader(shader) {}

            ~ShaderBuilder() = default;

        public:
            Shader* Build(const string& name);

            ShaderBuilder& AddVertexStageFromFile(const char* filepath);
            ShaderBuilder& AddPixelStageFromFile(const char* filepath);
            ShaderBuilder& AddGeometryStageFromFile(const char* filepath);
            ShaderBuilder& AddTessControlStageFromFile(const char* filepath);
            ShaderBuilder& AddTessEvalStageFromFile(const char* filepath);
            ShaderBuilder& AddComputeStageFromFile(const char* filepath);

            ShaderBuilder& AddVertexStage(const string& source);
            ShaderBuilder& AddPixelStage(const string& source);
            ShaderBuilder& AddGeometryStage(const string& source);
            ShaderBuilder& AddTessControlStage(const string& source);
            ShaderBuilder& AddTessEvalStage(const string& source);
            ShaderBuilder& AddComputeStage(const string& source);

        private:
            Context* m_Context = nullptr;
            Shader* m_Shader = nullptr;
        };

        class ENGINE_API ShaderManager final {

        public:
            static void Init(Context* context);
            static void Free();

            static ShaderBuilder& Builder();

            static void AddShader(const string& name, const Shader& shader);
            static void RemoveShader(const string& name);

        private:
            static Context* s_Context;
            static ShaderBuilder s_ShaderBuilder;
            static unordered_map<string, Shader> s_ShaderTable;
            static Shader s_TempShader;
        };

    }

}