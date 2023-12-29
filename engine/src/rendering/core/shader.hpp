#pragma once

#include <rendering/core/render_target.hpp>

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
                COMPUTE = 3,
            };

            u64 ID;
            eType Type;
            sBlob Blob;
            uword Flag = 0;
            bool Compiled = false;
            string Profile;
            string EntryPoint;
            string Source;
            vector<sBuffer*>  Buffers;
            vector<sTexture*> Textures;
            vector<sSampler*> Samplers;

            sShaderStage() = default;
            sShaderStage(u64 id, eType type) : ID(id), Type(type) {}

            inline void SetBuffer(sBuffer* buffer, u32 slot = K_SLOT_DEFAULT) {
                if (slot != K_SLOT_DEFAULT) {
                    buffer->Slot = slot;
                }
                Buffers.emplace_back(buffer);
            }

            inline void SetTexture(sTexture* texture, u32 slot = K_SLOT_DEFAULT) {
                if (slot != K_SLOT_DEFAULT) {
                    texture->Slot = slot;
                }
                Textures.emplace_back(texture);
            }

            inline void SetSampler(sSampler* sampler, u32 slot = K_SLOT_DEFAULT) {
                if (slot != K_SLOT_DEFAULT) {
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
            virtual void Draw(ecs::cScene* scene) {}
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
            cDefaultShader(cShader::eCategory category, const string& name);
            ~cDefaultShader() override;

            void Bind() override;
            void Unbind() override;

            ePrimitiveTopology PrimitiveTopology = ePrimitiveTopology::DEFAULT;
            sInputLayout InputLayout = sVertex::k_Format;
            sShaderStage* VertexStage = nullptr;
            sShaderStage* PixelStage = nullptr;
            sShaderStage* GeometryStage = nullptr;
            sRenderTarget* RenderTarget = nullptr;
            sRasterizer Rasterizer;
            sDepthStencilMode DepthStencilMode;
            sBlendMode BlendMode;

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

            sShaderStage* ComputeStage = nullptr;
            glm::ivec3 m_ThreadGroups;
        };

        class ENGINE_API cShaderManager final
        {

        public:
            static void Init();
            static void Free();

            static sShaderStage* GetFromFile(const sShaderStage::eType& type, const char* filepath);
            static sShaderStage* GetFromSrc(
                    const sShaderStage::eType& type,
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

            static unordered_map<string, sShaderStage>* s_Stages;
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