#pragma once

#include <rendering/core/render_target.hpp>

namespace xpe {

    namespace render {

        struct sBinding
        {
            sBinding() {}
            sBinding(cResource* resource, void* view, const cResource::eType& resourceType, const cResource::eViewType& viewType)
                : Resource(resource), View(view), ResourceType(resourceType), ViewType(viewType) {}

            cResource* Resource;
            void* View;
            cResource::eType ResourceType;
            cResource::eViewType ViewType;
        };

        struct ENGINE_API sShaderStage : public cResource
        {
            enum class eType
            {
                NONE = -1,
                VERTEX = 0,
                PIXEL = 1,
                GEOMETRY = 2,
                COMPUTE = 3
            };

            sShaderStage() = default;
            sShaderStage(u64 id, eType type) : ID(id), Type(type) {}

            void SetBufferBinding(cBuffer* buffer, const cResource::eViewType& viewType, u32 slot = K_SLOT_DEFAULT)
            {
                if (slot != K_SLOT_DEFAULT) {
                    buffer->SetSlot(slot);
                }
                if (viewType == cResource::eViewType::SRV)
                {
                    sBinding binding((cResource*)buffer, buffer->GetSRVInstance(), cResource::eType::BUFFER, cResource::eViewType::SRV);
                    BufferBindings.emplace_back(binding);
                }
                else if (viewType == cResource::eViewType::UAV)
                {
                    sBinding binding((cResource*)buffer, buffer->GetUAVInstance(), cResource::eType::BUFFER, cResource::eViewType::UAV);
                    BufferBindings.emplace_back(binding);
                }
            }

            void SetTextureBinding(cTexture* texture, const cResource::eViewType& viewType, u32 slot = K_SLOT_DEFAULT)
            {
                if (slot != K_SLOT_DEFAULT) {
                    texture->SetSlot(slot);
                }
                if (viewType == cResource::eViewType::SRV)
                {
                    sBinding binding((cResource*)texture, texture->GetSRVInstance(), cResource::eType::BUFFER, cResource::eViewType::SRV);
                    TextureBindings.emplace_back(binding);
                }
                else if (viewType == cResource::eViewType::UAV)
                {
                    sBinding binding((cResource*)texture, texture->GetUAVInstance(), cResource::eType::BUFFER, cResource::eViewType::UAV);
                    TextureBindings.emplace_back(binding);
                }
            }

            inline void SetSampler(sSampler* sampler, u32 slot = K_SLOT_DEFAULT) {
                if (slot != K_SLOT_DEFAULT) {
                    sampler->Slot = slot;
                }
                Samplers.emplace_back(sampler);
            }

            u64 ID;
            eType Type;
            sBlob Blob;
            uword Flag = 0;
            bool Compiled = false;
            string Profile;
            string EntryPoint;
            string Source;
            vector<sBinding> BufferBindings;
            vector<sBinding> TextureBindings;
            vector<sSampler*> Samplers;
        };

        class ENGINE_API cShader : public cObject
        {
        public:
            enum eCategory
            {
                NONE = 0,
                PREPASS = 1,
                OPAQUE_GEOMETRY = 2,
                TRANSPARENT_GEOMETRY = 3,
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
            dual Enable = true;
            eCategory Category = eCategory::NONE;
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
            cComputeShader(const eCategory& category, const string& name, const glm::vec3& threadGroups = { 1, 1, 1 })
            : cShader(category, name), m_ThreadGroups(threadGroups) {}

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