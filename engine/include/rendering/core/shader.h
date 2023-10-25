#pragma once

#include <rendering/core/core.h>

namespace xpe {

    namespace render {

        enum class eShaderType
        {
            VERTEX = 0,
            PIXEL = 1,
            GEOMETRY = 2,
            TESS_EVAL = 3,
            TESS_CONTROL = 4,
            COMPUTE = 5,
        };

        struct ENGINE_API ShaderStage : public GPUResource
        {
            eShaderType Type;
            render::Blob Blob;
            string Source;
            const char* EntryPoint = nullptr;
            const char* Profile = nullptr;
            uword Flag = 0;
            bool Compiled = false;
            string Name;

            ShaderStage() = default;
            ShaderStage(eShaderType type) : Type(type) {}
            ShaderStage(const string& name, eShaderType type) : Type(type), Name(name) {}
        };

        struct ENGINE_API Shader : public Object
        {
            string Name = "Untitled";
            vector<ShaderStage*> Stages;
        };

        struct ENGINE_API ShaderStorage : public Object
        {
            unordered_map<string, ShaderStage> ShaderStages;
            unordered_map<string, Shader> Shaders;

            ~ShaderStorage();
        };

        class ENGINE_API ShaderManager final
        {

        public:
            static void Init();
            static void Free();

            static Shader* CreateShader(const string& id);

            static void AddVertexStageFromFile(Shader* const shader, const char* filepath);
            static void AddPixelStageFromFile(Shader* const shader, const char* filepath);
            static void AddGeometryStageFromFile(Shader* const shader, const char* filepath);
            static void AddTessControlStageFromFile(Shader* const shader, const char* filepath);
            static void AddTessEvalStageFromFile(Shader* const shader, const char* filepath);
            static void AddComputeStageFromFile(Shader* const shader, const char* filepath);

            static void AddVertexStage(Shader* const shader, const string& id, const string& source);
            static void AddPixelStage(Shader* const shader, const string& id, const string& source);
            static void AddGeometryStage(Shader* const shader, const string& id, const string& source);
            static void AddTessControlStage(Shader* const shader, const string& id, const string& source);
            static void AddTessEvalStage(Shader* const shader, const string& id, const string& source);
            static void AddComputeStage(Shader* const shader, const string& id, const string& source);

            static void BuildShader(Shader* const shader);
            static void BuildShader(const string& id);

            static Shader* GetShader(const string& id);
            static ShaderStage* GetShaderStage(const string& filepath);

            static void ReloadStage(const char* filepath);

        private:
            static void AddShader(const string& id, const Shader& shader);
            static void FreeShader(const string& id);
            static void FreeShaders();

            static void AddShaderStage(const string& filepath, const ShaderStage& shaderStage);
            static void RemoveShaderStage(const string& filepath);

            static void WriteGeneratedShader(const char* filepath, const string& src);

        private:
            static ShaderStorage* s_Storage;
        };

    }

}