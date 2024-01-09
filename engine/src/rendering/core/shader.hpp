#pragma once

#include <rendering/core/render_target.hpp>

namespace xpe {

    namespace render {

        struct sBinding
        {
            sBinding() {}
            sBinding(cResource* resource, void* view, const cResource::eType& resourceType, const cResource::eViewType& viewType, u32 slot)
                : Resource(resource), View(view), ResourceType(resourceType), ViewType(viewType), Slot(slot) {}

            cResource* Resource;
            void* View;
            cResource::eType ResourceType;
            cResource::eViewType ViewType;
            u32 Slot;
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

            void SetBufferBinding(cBuffer* buffer, const cResource::eViewType& viewType, u32 slot)
            {
                if (viewType == cResource::eViewType::SRV)
                {
                    sBinding binding((cResource*)buffer, buffer->GetSRVInstance(), cResource::eType::BUFFER, cResource::eViewType::SRV, slot);
                    BufferBindings.emplace_back(binding);
                }
                else if (viewType == cResource::eViewType::UAV)
                {
                    sBinding binding((cResource*)buffer, buffer->GetUAVInstance(), cResource::eType::BUFFER, cResource::eViewType::UAV, slot);
                    BufferBindings.emplace_back(binding);
                }
            }

            void SetTextureBinding(cTexture* texture, const cResource::eViewType& viewType, u32 slot)
            {
                if (viewType == cResource::eViewType::SRV)
                {
                    sBinding binding((cResource*)texture, texture->GetSRVInstance(), cResource::eType::TEXTURE, cResource::eViewType::SRV, slot);
                    TextureBindings.emplace_back(binding);
                }
                else if (viewType == cResource::eViewType::UAV)
                {
                    sBinding binding((cResource*)texture, texture->GetUAVInstance(), cResource::eType::TEXTURE, cResource::eViewType::UAV, slot);
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
                FINAL = 6
            };

            cShader(cShader::eCategory category, const string& name) : Category(category), Name(name) {}
            virtual ~cShader() {}
            virtual void Draw(ecs::cScene* scene) {}
            virtual void Bind() = 0;
            virtual void Unbind() = 0;

            string Name = "";
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
            cRenderTarget* RenderTarget = nullptr;
            sViewport* Viewport = nullptr;
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
            : cShader(category, name) {}

            void Bind() override;
            void Unbind() override;

            virtual void Draw(ecs::cScene* scene) {}

            sShaderStage* ComputeStage = nullptr;
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

            static void SetShader(cShader* shader, u32 slot);
            static vector<cShader*>* GetShaders(const cShader::eCategory& category);
            static cShader* GetShader(const cShader::eCategory& category, const string& name);

            static void ReloadStage(const char* filepath);

        private:
            static void FreeStages();
            static void FreeShaders(vector<cShader*>* shaders);
            static void WriteGeneratedShader(const char* filepath, const string& src);

            static cShader* GetShader(const vector<cShader*>* shaders, const string& name);

            static unordered_map<string, sShaderStage>* s_Stages;
            static const usize s_MaxShaderCount = 64;
            static vector<cShader*>* s_ComputeShaders;
            static vector<cShader*>* s_PrepassShaders;
            static vector<cShader*>* s_OpaqueShaders;
            static vector<cShader*>* s_TransparentShaders;
            static vector<cShader*>* s_PostfxShaders;
            static vector<cShader*>* s_UiShaders;
            static vector<cShader*>* s_FinalShaders;
        };

    }

}