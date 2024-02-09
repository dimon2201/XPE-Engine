#pragma once

#include <render_target.hpp>
#include <ecs.hpp>

namespace xpe
{
    namespace render
    {
        class ENGINE_API cShaderStage : public cObject
        {

        public:
            enum class eType
            {
                NONE = -1,
                VERTEX = 0,
                PIXEL = 1,
                GEOMETRY = 2,
                COMPUTE = 3,
            };

            void* Instance = nullptr;
            u64 ID;
            eType Type;
            sBlob Blob;
            uword Flag = 0;
            bool Compiled = false;
            string Profile;
            string EntryPoint;
            string Source;
            vector<sBuffer*>  Buffers;
            vector<cTexture*> Textures;
            vector<cSampler*> Samplers;

            cShaderStage() = default;
            cShaderStage(u64 id, eType type) : ID(id), Type(type) {}

            inline void SetBuffer(sBuffer* buffer, u32 slot = K_SLOT_DEFAULT)
            {
                if (slot != K_SLOT_DEFAULT)
                {
                    buffer->Slot = slot;
                }
                Buffers.emplace_back(buffer);
            }

            inline void SetTexture(cTexture* texture, u32 slot = K_SLOT_DEFAULT)
            {
                if (slot != K_SLOT_DEFAULT)
                {
                    texture->Slot = slot;
                }
                Textures.emplace_back(texture);
            }

            inline void SetSampler(cSampler* sampler, u32 slot = K_SLOT_DEFAULT)
            {
                if (slot != K_SLOT_DEFAULT)
                {
                    sampler->Slot = slot;
                }
                Samplers.emplace_back(sampler);
            }
        };

        class ENGINE_API cShader : public cObject
        {

        public:
            enum eCategory
            {
                NONE = 0,
                PREPASS = 1,
                OPAQUE = 2,
                TRANSPARENT = 3,
                POSTFX = 4,
                UI = 5,
                FINAL = 6,
                COMPUTE = 7
            };

            cShader(cShader::eCategory category, const string& name) : Category(category), ID(Hash(name)) {}
            virtual ~cShader() {}
            virtual void Draw(cScene* scene) {}
            virtual void Bind() = 0;
            virtual void Unbind() = 0;

            u64 ID = 0;
            bool Enable = true;
            eCategory Category = eCategory::PREPASS;
            cShader* Next = nullptr;
        };

        class ENGINE_API cDefaultShader : public cShader
        {

        public:
            ePrimitiveTopology PrimitiveTopology = ePrimitiveTopology::DEFAULT;
            sInputLayout InputLayout = sVertex::k_Format;
            cShaderStage* VertexStage = nullptr;
            cShaderStage* PixelStage = nullptr;
            cShaderStage* GeometryStage = nullptr;
            cRenderTarget* RenderTarget = nullptr;
            sRasterizer Rasterizer;
            sDepthStencilMode DepthStencilMode;
            sBlendMode BlendMode;

            cDefaultShader(cShader::eCategory category, const string& name);
            ~cDefaultShader() override;

            void Bind() override;
            void Unbind() override;

        protected:
            void Init();
            virtual void InitPrepass();
            virtual void InitOpaque();
            virtual void InitTransparent();
            virtual void InitPostFX();
            virtual void InitUI();
            virtual void InitFinal();
        };

        class ENGINE_API cComputeShader : public cShader
        {

        public:
            cComputeShader(const string& name, const glm::vec3& threadGroups = { 1, 1, 1 })
            : cShader(eCategory::COMPUTE, name), m_ThreadGroups(threadGroups) {}

            void Bind() override;
            void Unbind() override;

            void Draw(ecs::cScene *scene) override;

            cShaderStage* ComputeStage = nullptr;
            glm::ivec3 m_ThreadGroups;
        };

        class ENGINE_API MShader final
        {

        public:
            static void Init();
            static void Free();

            static cShaderStage* GetFromFile(const cShaderStage::eType& type, const char* filepath);
            static cShaderStage* GetFromSrc(
                    const cShaderStage::eType& type,
                    const string& id,
                    const string& src
            );

            static void SetShader(cShader* shader);
            static void SetShaderAfter(cShader* shader, const string& name);
            static cShader* GetShaders(cShader::eCategory category);
            static cShader* GetShader(const string& name);

            static void ReloadStage(const char* filepath);

        private:
            static void FreeStages();
            static void WriteGeneratedShader(const char* filepath, const string& src);

            static void SetShader(cShader*& dest, cShader* src);
            static void SetShader(cShader*& dest, cShader* src, u64 id);
            static cShader* GetShader(cShader*& src, u64 id);

            static unordered_map<string, cShaderStage>* s_Stages;
            static cShader* s_ComputeShaders;
            static cShader* s_PrepassShaders;
            static cShader* s_OpaqueShaders;
            static cShader* s_TransparentShaders;
            static cShader* s_PostfxShaders;
            static cShader* s_UiShaders;
            static cShader* s_FinalShaders;
        };
    }
}