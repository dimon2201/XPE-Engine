#pragma once

#include <rendering/core/context.hpp>
#include <particle/particle_manager.hpp>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace math;
        using namespace particle;

        struct ENGINE_API cIndexBuffer : public cBuffer
        {

        public:
            cIndexBuffer(const usize indexCount = 0, cBuffer::eViewType viewtype = cBuffer::eViewType::NONE);
            cIndexBuffer(const vector<u32>& indexArray, cBuffer::eViewType viewtype = cBuffer::eViewType::NONE);
            ~cIndexBuffer();

            void Flush();
            void FlushIndices(const vector<u32>& indices);
            void FlushIndex(u32 i, u32 index);

            void Resize(const usize count);
            void Reserve(const usize count);
            void Recreate(const usize count);

            usize AddIndices(const vector<u32>& indices);

            inline u32* GetIndex(const u32 i)
            {
                if (i >= Count()) { Resize(i + 1); }
                return &m_List[i];
            }

            inline usize Count() { return m_List.size(); }

        private:
            vector<u32> m_List;

        };

        struct ENGINE_API cVertexBuffer : public cBuffer
        {

        public:
            cVertexBuffer(const usize vertexCount = 0, cBuffer::eViewType viewtype = cBuffer::eViewType::NONE);
            cVertexBuffer(const vector<sVertex>& vertices, cBuffer::eViewType viewtype = cBuffer::eViewType::NONE);
            ~cVertexBuffer();

            void Flush();
            void FlushVertex(u32 index, const sVertex& item);
            void FlushVertices(const vector<sVertex>& vertices);

            void Resize(const usize count);
            void Reserve(const usize count);
            void Recreate(const usize count);

            void Clear();

            usize AddVertices(const vector<sVertex>& vertices);

            inline sVertex* GetVertex(const u32 index)
            {
                if (index >= Count()) { Resize(index + 1); }
                return &m_List[index];
            }

            inline usize Count() { return m_List.size(); }

        private:
            vector<sVertex> m_List;

        };

        template<typename T>
        class cConstantBuffer : public cBuffer
        {

        public:
            cConstantBuffer(u32 slot, cBuffer::eViewType viewType = cBuffer::eViewType::NONE);
            ~cConstantBuffer();

            void Flush();
            void FlushItem(const T& item);

            inline T& GetItem() { return m_Item; }

        private:
            T m_Item;

        };

        template<typename T>
        cConstantBuffer<T>::cConstantBuffer(u32 slot, cBuffer::eViewType viewType)
        {
            m_Type = cBuffer::eType::CONSTANT;
            m_ViewType = viewType;
            m_Slot = slot;
            m_StructureSize = sizeof(T);
            m_NumElements = 1;
            m_InitialData = &m_Item;
            m_Usage = cBuffer::eUsage::DYNAMIC;
            context::CreateBuffer(*this);
            context::WriteBuffer(*this, &m_Item, m_StructureSize);
        }

        template<typename T>
        cConstantBuffer<T>::~cConstantBuffer()
        {
            context::FreeBuffer(*this);
        }

        template<typename T>
        void cConstantBuffer<T>::Flush()
        {
            context::WriteBuffer(*this, &m_Item, m_StructureSize);
        }

        template<typename T>
        void cConstantBuffer<T>::FlushItem(const T &item)
        {
            m_Item = item;
            context::WriteBuffer(*this, &m_Item, m_StructureSize);
        }

        template<typename T>
        class cStructuredBuffer : public cBuffer
        {

        public:
            cStructuredBuffer(usize count, u32 slot, cBuffer::eViewType viewType = cBuffer::eViewType::SRV);
            ~cStructuredBuffer();

            void Flush();
            void FlushItem(u32 index, const T& newItem);

            template<typename ... Args>
            u32 Add(Args&&... args);

            inline void Clear() { m_List.clear(); }
            inline usize Size() const { return m_List.size(); }
            inline vector<T>& GetList() { return m_List; }
            inline usize& GetCounterRef() { return m_Counter; }
            inline T* operator [](u32 index) { return &m_List[index]; }

        protected:
            usize m_Counter = 0;
            vector<T> m_List;

        };

        template<typename T>
        cStructuredBuffer<T>::cStructuredBuffer(usize count, u32 slot, cBuffer::eViewType viewType)
        {
            m_Type = cBuffer::eType::STRUCTURED;
            m_ViewType = viewType;
            m_Slot = slot;
            m_StructureSize = sizeof(T);
            m_NumElements = count;
            m_InitialData = nullptr;
            m_Usage = cBuffer::eUsage::DYNAMIC;
            m_List.resize(count);
            context::CreateBuffer(*this);
        }

        template<typename T>
        cStructuredBuffer<T>::~cStructuredBuffer()
        {
            context::FreeBuffer(*this);
            m_List.clear();
        }

        template<typename T>
        void cStructuredBuffer<T>::Flush()
        {
            context::WriteBuffer(*this, m_List.data(), GetByteSize());
        }

        template<typename T>
        void cStructuredBuffer<T>::FlushItem(u32 index, const T& newItem)
        {
            T* item = operator[](index);
            if (item == nullptr) {
                return;
            }
            *item = newItem;
            context::CopyBufferOffset(*this, m_StructureSize * index, item, m_StructureSize);
        }

        template<typename T>
        template<typename... Args>
        u32 cStructuredBuffer<T>::Add(Args &&... args)
        {
            m_List.emplace_back(args...);
            return m_List.size() - 1;
        }

        template<typename T>
        class cRWBuffer : public cBuffer
        {

        public:
            cRWBuffer() = default;
            cRWBuffer(usize count, u32 slot, cBuffer::eViewType viewtype = cBuffer::eViewType::UAV);
            ~cRWBuffer();

            void Recreate(const usize count);

        };

        template<typename T>
        cRWBuffer<T>::cRWBuffer(usize count, u32 slot, cBuffer::eViewType viewType)
        {
            m_Type = cBuffer::eType::RW;
            m_ViewType = viewType;
            m_Slot = slot;
            m_StructureSize = sizeof(T);
            m_NumElements = count;
            m_InitialData = nullptr;
            m_Usage = cBuffer::eUsage::DEFAULT;
            context::CreateBuffer(*this);
        }

        template<typename T>
        cRWBuffer<T>::~cRWBuffer()
        {
            context::FreeBuffer(*this);
        }

        template<typename T>
        void cRWBuffer<T>::Recreate(const usize count)
        {
            m_NumElements = count;
            m_InitialData = nullptr;
            context::FreeBuffer(*this);
            context::CreateBuffer(*this);
        }

        struct ENGINE_API sCameraData : public cObject
        {
            glm::vec3 Position;
            float _pad0 = 0;
            glm::mat4 View;
            glm::mat4 Projection;
            sViewport Viewport { glm::vec4(0.0f), glm::vec2(0.0f) };
            float Exposure = 1.0f;
            float Gamma = 2.2f;
        };

        struct ENGINE_API cCameraBuffer : public cConstantBuffer<sCameraData>
        {
            cCameraBuffer() : cConstantBuffer<sCameraData>(K_SLOT_CAMERA) {}
        };

        struct ENGINE_API sRenderInstance final
        {
            glm::mat4 ModelMatrix;  // position world space
            glm::mat4 NormalMatrix; // normal world space
            u32 MaterialIndex = 0;
            u32 SkeletonIndex = 0;
            u32 LightIndex    = 0;
            u32 ShadowCasterCount = 0;
            glm::vec4 ShadowCasters[5];
        };

        struct ENGINE_API cInstanceBuffer : public cStructuredBuffer<sRenderInstance>
        {
            cInstanceBuffer(usize count = 0) : cStructuredBuffer<sRenderInstance>(count, K_SLOT_INSTANCES) {}
        };

        struct ENGINE_API sDirectLightData
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
            glm::mat4 ViewProjection;
        };

        struct ENGINE_API cDirectLightBuffer : public cStructuredBuffer<sDirectLightData>
        {
            cDirectLightBuffer(usize count = 0) : cStructuredBuffer<sDirectLightData>(count, K_SLOT_DIRECT_LIGHTS) {}
        };

        struct ENGINE_API sLightData
        {
            glm::vec3 Position = glm::vec3(0.0f);
            glm::vec3 Color = glm::vec3(0.0f);
            glm::mat4 View = glm::mat4(1.0f);
            glm::mat4 Projection = glm::mat4(1.0f);
            glm::vec4 ShadowCaster = glm::vec4(0.0f);
        };

        struct ENGINE_API cLightBuffer : public cStructuredBuffer<sLightData>
        {
            cLightBuffer(usize count = 0) : cStructuredBuffer<sLightData>(count, K_SLOT_DIRECT_LIGHTS) {}
        };

        struct ENGINE_API sPointLightData
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
            float Constant = 1.0f;
            float Linear = 0.09f;
            float Quadratic = 0.032f;
            glm::mat4 ViewProjection;
        };

        struct ENGINE_API cPointLightBuffer : public cStructuredBuffer<sPointLightData>
        {
            cPointLightBuffer(usize count = 0) : cStructuredBuffer<sPointLightData>(count, K_SLOT_POINT_LIGHTS) {}
        };

        struct ENGINE_API sSpotLightData
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Direction = { 1, 1, 1 };
            glm::vec3 Color = { 1, 1, 1 };
            float Cutoff = 1;
            float Outer = 1;
            glm::mat4 ViewProjection;
            float Near = 0;
            float Far = 1;
        };

        struct ENGINE_API cSpotLightBuffer : public cStructuredBuffer<sSpotLightData>
        {
            cSpotLightBuffer(usize count = 0) : cStructuredBuffer<sSpotLightData>(count, K_SLOT_SPOT_LIGHTS) {}
        };

        struct ENGINE_API sMaterialData
        {
            // albedo mapping
            glm::vec4 Albedo = { 0.5, 0.5, 0.5, 1.0 };
            dual EnableAlbedoMap = false;
            // normal mapping
            dual EnableNormalMap = false;
            // parallax occlusion mapping
            dual EnableParallaxMap = false;
            float ParallaxHeightScale = 0.1;
            float ParallaxMinLayers = 8;
            float ParallaxMaxLayers = 32;
            // metal mapping
            float Metallness = 0.5f;
            dual EnableMetalMap = false;
            // roughness mapping
            float Roughness = 0.5f;
            dual EnableRoughnessMap = false;
            // ambient occlusion mapping
            float AO = 0.5f;
            dual EnableAOMap = false;
            // emission mapping
            glm::vec3 Emission = { 0, 0, 0 };
            dual EnableEmissionMap = false;
        };

        struct ENGINE_API cMaterialDataBuffer : public cStructuredBuffer<sMaterialData>
        {
            cMaterialDataBuffer(usize size = 0) : cStructuredBuffer<sMaterialData>(size, K_SLOT_MATERIALS) {}
        };

        struct ENGINE_API sShadowPcf final
        {
            int FilterSize = 0;
            float MinBias = 0.005f;
            float MaxBias = 0.05f;
            float _pad0 = 0;
        };

        struct ENGINE_API cShadowPcfBuffer : public cConstantBuffer<sShadowPcf>
        {
            cShadowPcfBuffer() : cConstantBuffer<sShadowPcf>(K_SLOT_SHADOW_PCF) {}
        };

        struct ENGINE_API sSkeletonData final
        {
            glm::mat4 Transform;
        };

        struct ENGINE_API cSkeletonBuffer : public cStructuredBuffer<sSkeletonData>
        {
            cSkeletonBuffer(usize count = 0) : cStructuredBuffer<sSkeletonData>(count, K_SLOT_SKELETONS) {}
        };

        struct ENGINE_API sSsaoData final
        {
            int DirectionCount = 4;
            int SampleCount = 4;
            float Intensity = 1.0;
            float _pad0 = 0;
        };

        struct ENGINE_API cSsaoBuffer : public cConstantBuffer<sSsaoData>
        {
            cSsaoBuffer() : cConstantBuffer<sSsaoData>(K_SLOT_SSAO) {}
        };

        struct ENGINE_API sWidgetData
        {
            glm::mat4 ModelMatrix;
            glm::mat4 Projection;
            glm::vec4 Color = { 1, 1, 1, 1 };
            dual HasTexture = false;
            glm::vec2 AtlasUV[4] = {
                    glm::vec2(0),
                    glm::vec2(0),
                    glm::vec2(0),
                    glm::vec2(0)
            };
        };

        struct ENGINE_API cWidgetBuffer : public cStructuredBuffer<sWidgetData>
        {
            cWidgetBuffer(usize count = 0) : cStructuredBuffer<sWidgetData>(count, K_SLOT_WIDGETS) {}
        };

        struct ENGINE_API sChar final
        {
            u32 TextIndex = 0;
            f32 GlyphSize = 0;
            f32 Width = 0;
            f32 Height = 0;
            f32 Left = 0;
            f32 Top = 0;
            f32 AdvanceX = 0;
            f32 AdvanceY = 0;
            f32 AtlasXOffset = 0;
            f32 AtlasYOffset = 0;
        };

        struct ENGINE_API cCharBuffer : public cStructuredBuffer<sChar>
        {
            cCharBuffer(usize count = 0) : cStructuredBuffer<sChar>(count, K_SLOT_CHARS) {}
        };

        struct ENGINE_API sText final
        {
            glm::mat4 ModelMatrix;
            glm::mat4 Projection;
            glm::vec4 Color = { 1, 1, 1, 1 };
        };

        struct ENGINE_API cTextBuffer : public cStructuredBuffer<sText>
        {
            cTextBuffer(usize count = 0) : cStructuredBuffer<sText>(count, K_SLOT_TEXTS) {}
        };

        struct ENGINE_API sParticlePassInfo
        {
            u32 ParticleCount;
        };

        struct ENGINE_API cParticleBuffer : public cRWBuffer<sParticle>
        {
            cParticleBuffer(usize count = 0)
                : cRWBuffer<sParticle>(count, K_SLOT_BUFFER_PARTICLE_UAV, cResource::eViewType::SRV_UAV) {}
        };

        struct ENGINE_API cParticleEmitterBuffer : public cStructuredBuffer<sParticleEmitter>
        {
            cParticleEmitterBuffer(usize count = 0) : cStructuredBuffer<sParticleEmitter>(count, K_SLOT_BUFFER_PARTICLE_EMITTER) {}
        };

        struct ENGINE_API cParticlePassInfoBuffer : public cRWBuffer<sParticlePassInfo>
        {
            cParticlePassInfoBuffer(usize count = 0) : cRWBuffer<sParticlePassInfo>(count, K_SLOT_BUFFER_PARTICLE_PASS_INFO) {}
        };

    }

}