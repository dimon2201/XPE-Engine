#pragma once

#include <rendering/core/core.h>
#include <rendering/core/texture.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sShaderStage : public sResource
        {
            enum class eType
            {
                NONE = -1,
                VERTEX = 0,
                PIXEL = 1,
                GEOMETRY = 2,
                TESS_EVAL = 3,
                TESS_CONTROL = 4,
                COMPUTE = 5,
            };

            eType Type;
            render::sBlob Blob;
            string Source;
            const char* EntryPoint = nullptr;
            const char* Profile = nullptr;
            uword Flag = 0;
            bool Compiled = false;
            string Name;
            sBuffer* VertexBuffer = nullptr;
            sBuffer* IndexBuffer = nullptr;
            vector<sBuffer*>  Buffers;
            vector<sTexture*> Textures;
            vector<sSampler*> Samplers;

            sShaderStage() = default;
            sShaderStage(eType type) : Type(type) {}
            sShaderStage(const string& name, eType type) : Type(type), Name(name) {}
        };

        struct ENGINE_API sShader : public cObject
        {
            string Name = "Untitled";
            vector<sShaderStage*> Stages;
        };

        struct ENGINE_API sShaderStorage : public cObject
        {
            unordered_map<string, sShaderStage> ShaderStages;
            unordered_map<string, sShader> Shaders;

            ~sShaderStorage();
        };

        class ENGINE_API cShaderManager final
        {

        public:
            static void Init();
            static void Free();

            static sShader* CreateShader(const string& id);

            static void AddVertexStageFromFile(sShader* const shader, const char* filepath);
            static void AddPixelStageFromFile(sShader* const shader, const char* filepath);
            static void AddGeometryStageFromFile(sShader* const shader, const char* filepath);
            static void AddTessControlStageFromFile(sShader* const shader, const char* filepath);
            static void AddTessEvalStageFromFile(sShader* const shader, const char* filepath);
            static void AddComputeStageFromFile(sShader* const shader, const char* filepath);

            static void AddVertexStage(sShader* const shader, const string& id, const string& source);
            static void AddPixelStage(sShader* const shader, const string& id, const string& source);
            static void AddGeometryStage(sShader* const shader, const string& id, const string& source);
            static void AddTessControlStage(sShader* const shader, const string& id, const string& source);
            static void AddTessEvalStage(sShader* const shader, const string& id, const string& source);
            static void AddComputeStage(sShader* const shader, const string& id, const string& source);

            static void BuildShader(sShader* const shader);
            static void BuildShader(const string& id);

            static sShader* GetShader(const string& id);
            static sShaderStage* GetShaderStage(const string& filepath);

            static void ReloadStage(const char* filepath);

        private:
            static void AddShader(const string& id, const sShader& shader);
            static void FreeShader(const string& id);
            static void FreeShaders();

            static void AddShaderStage(const string& filepath, const sShaderStage& shaderStage);
            static void RemoveShaderStage(const string& filepath);

            static void WriteGeneratedShader(const char* filepath, const string& src);

        private:
            static sShaderStorage* s_Storage;
        };

    }

}