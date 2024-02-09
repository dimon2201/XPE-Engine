#pragma once

#include <render_context.hpp>

namespace xpe
{
    namespace render
    {
        struct ENGINE_API sIndexBuffer : public sBuffer
        {
            vector<u32> List;

            sIndexBuffer(const usize indexCount = 0, sBuffer::eSubType subtype = sBuffer::eSubType::NONE, sBuffer::eViewType viewtype = sBuffer::eViewType::NONE);
            sIndexBuffer(const vector<u32>& indexArray, sBuffer::eSubType subtype = sBuffer::eSubType::NONE, sBuffer::eViewType viewtype = sBuffer::eViewType::NONE);
            ~sIndexBuffer();

            void Flush();
            void FlushIndices(const vector<u32>& indices);
            void FlushIndex(u32 i, u32 index);

            void Resize(const usize count);
            void Reserve(const usize count);
            void Recreate(const usize count);

            // returns index offset of added indices
            usize AddIndices(const vector<u32>& indices);

            inline u32* GetIndex(const u32 i)
            {
                // check if index is in the size bounds
                // if not, then resize to index + 1
                if (i >= Count())
                {
                    Resize(i + 1);
                }
                return &List[i];
            }

            inline usize Count()
            {
                return List.size();
            }
        };

        struct ENGINE_API sVertexBuffer : public sBuffer
        {
            vector<sVertex> List;

            sVertexBuffer(const usize vertexCount = 0, sBuffer::eSubType subtype = sBuffer::eSubType::NONE, sBuffer::eViewType viewtype = sBuffer::eViewType::NONE);
            sVertexBuffer(const vector<sVertex>& vertices, sBuffer::eSubType subtype = sBuffer::eSubType::NONE, sBuffer::eViewType viewtype = sBuffer::eViewType::NONE);
            ~sVertexBuffer();

            void Flush();
            void FlushVertex(u32 index, const sVertex& item);
            void FlushVertices(const vector<sVertex>& vertices);

            void Resize(const usize count);
            void Reserve(const usize count);
            void Recreate(const usize count);

            void Clear();

            // returns vertex offset of added vertices
            usize AddVertices(const vector<sVertex>& vertices);

            inline sVertex* GetVertex(const u32 index)
            {
                // check if index is in the size bounds
                // if not, then resize to index + 1
                if (index >= Count())
                {
                    Resize(index + 1);
                }
                return &List[index];
            }

            inline usize Count()
            {
                return List.size();
            }
        };


        JSON_ENUM(ePrimitiveTopology,
        {
              { ePrimitiveTopology::DEFAULT, "DEFAULT" },
              { ePrimitiveTopology::TRIANGLE_LIST, "TRIANGLE_LIST" },
              { ePrimitiveTopology::TRIANGLE_STRIP, "TRIANGLE_STRIP" },
              { ePrimitiveTopology::POINT_LIST, "POINT_LIST" },
              { ePrimitiveTopology::LINE_LIST, "LINE_LIST" },
              { ePrimitiveTopology::LINE_STRIP, "LINE_STRIP" },
        })
        JSON(sVertexBuffer, List)
        JSON(sIndexBuffer, List)

        template<typename T>
        class cItemBuffer : public sBuffer
        {
        public:
            T Item;

            cItemBuffer() = default;
            cItemBuffer(u32 slot, sBuffer::eSubType subtype = sBuffer::eSubType::NONE, sBuffer::eViewType viewtype = sBuffer::eViewType::NONE);
            ~cItemBuffer();

            void Flush();
            void FlushItem(const T& item);
        };

        template<typename T>
        cItemBuffer<T>::cItemBuffer(u32 slot, sBuffer::eSubType subtype, sBuffer::eViewType viewtype)
        {
            Type = sBuffer::eType::ITEM;
            ViewType = viewtype;
            SubType = subtype;
            Slot = slot;
            StructureSize = sizeof(T);
            NumElements = 1;
            InitialData = &Item;
            context::CreateBuffer(*this);
            context::CopyBuffer(*this, &Item, StructureSize);
        }

        template<typename T>
        cItemBuffer<T>::~cItemBuffer()
        {
            context::FreeBuffer(*this);
        }

        template<typename T>
        void cItemBuffer<T>::Flush()
        {
            context::CopyBuffer(*this, &Item, StructureSize);
        }

        template<typename T>
        void cItemBuffer<T>::FlushItem(const T &item)
        {
            Item = item;
            context::CopyBuffer(*this, &Item, StructureSize);
        }

        template<typename T>
        class cListBuffer : public sBuffer
        {

        public:
            cListBuffer() = default;
            cListBuffer(usize count, u32 slot, sBuffer::eSubType subtype = sBuffer::eSubType::NONE, sBuffer::eViewType viewtype = sBuffer::eViewType::SRV);
            ~cListBuffer();

            void Flush();
            void FlushItem(u32 index, const T& item);
            void FlushItemAt(u32 index);

            template<typename ... Args>
            u32 Add(Args&&... args);

            void RemoveAt(u32 index);

            void Resize(const usize count);

            void Reserve(const usize count);

            void Recreate(const usize count);

            void Clear();

            bool Empty();

            inline vector<T>& GetList()
            {
                return m_List;
            }

            inline usize Size() const
            {
                return m_List.size();
            }

            inline usize Capacity() const
            {
                return m_List.capacity();
            }

            inline T* Get(u32 index)
            {
                return &m_List[index];
            }

            inline void Set(u32 index, const T& item)
            {
                m_List[index] = item;
            }

            inline T* operator [](u32 index)
            {
                return Get(index);
            }

        protected:
            vector<T> m_List;
        };

        template<typename T>
        cListBuffer<T>::cListBuffer(usize count, u32 slot, sBuffer::eSubType subtype, sBuffer::eViewType viewtype)
        {
            Type = sBuffer::eType::LIST;
            SubType = subtype;
            ViewType = viewtype;
            Slot = slot;
            StructureSize = sizeof(T);
            NumElements = count;
            InitialData = m_List.data();
            if (count > 0)
            {
                m_List.resize(count);
                context::CreateBuffer(*this);
                context::CopyBuffer(*this, m_List.data(), GetByteSize());
            }
        }

        template<typename T>
        cListBuffer<T>::~cListBuffer()
        {
            context::FreeBuffer(*this);
            m_List.clear();
        }

        template<typename T>
        void cListBuffer<T>::Flush()
        {
            usize size = m_List.size();
            if (size != NumElements)
            {
                Recreate(size);
            }
            else
            {
                context::CopyBuffer(*this, m_List.data(), GetByteSize());
            }
        }

        template<typename T>
        void cListBuffer<T>::FlushItem(u32 index, const T& newItem)
        {
            T* item = operator[](index);
            if (item == nullptr)
            {
                return;
            }
            *item = newItem;
            context::CopyBufferOffset(*this, StructureSize * index, item, StructureSize);
        }

        template<typename T>
        void cListBuffer<T>::FlushItemAt(u32 index)
        {
            T* item = operator[](index);
            if (item == nullptr)
            {
                return;
            }
            context::CopyBufferOffset(*this, StructureSize * index, item, StructureSize);
        }

        template<typename T>
        void cListBuffer<T>::Recreate(const usize count)
        {
            m_List.resize(count);
            NumElements = count;
            InitialData = m_List.data();
            context::FreeBuffer(*this);
            context::CreateBuffer(*this);
            context::CopyBuffer(*this, m_List.data(), GetByteSize());
        }

        template<typename T>
        void cListBuffer<T>::Resize(const usize count)
        {
            m_List.resize(count);
        }

        template<typename T>
        void cListBuffer<T>::Reserve(const usize count)
        {
            m_List.reserve(count);
        }

        template<typename T>
        void cListBuffer<T>::Clear()
        {
            m_List.clear();
        }

        template<typename T>
        bool cListBuffer<T>::Empty()
        {
            return m_List.empty();
        }

        template<typename T>
        template<typename... Args>
        u32 cListBuffer<T>::Add(Args &&... args)
        {
            m_List.emplace_back(args...);
            return m_List.size() - 1;
        }

        template<typename T>
        void cListBuffer<T>::RemoveAt(u32 index)
        {
            m_List.erase(m_List.begin() + index);
        }

        struct ENGINE_API sCameraData : public cObject
        {
            glm::vec3 Position;
            float _pad0 = 0;
            glm::mat4 View;
            glm::mat4 Projection;
            sViewport Viewport;
            float Exposure = 1.0f;
            float Gamma = 2.2f;
        };

        struct ENGINE_API sCameraBuffer : public cItemBuffer<sCameraData>
        {
            sCameraBuffer() : cItemBuffer<sCameraData>(K_SLOT_CAMERA) {}
        };

        struct ENGINE_API sRenderInstance final
        {
            glm::mat4 ModelMatrix;  // position world space
            glm::mat4 NormalMatrix; // normal world space
            u32 MaterialIndex = 0;
            u32 SkeletonIndex = 0;
            u32 LightIndex    = 0;
        };

        struct ENGINE_API sInstanceBuffer : public cListBuffer<sRenderInstance>
        {
            sInstanceBuffer(usize count = 0) : cListBuffer<sRenderInstance>(count, K_SLOT_INSTANCES) {}
        };

        struct ENGINE_API sDirectLightData
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
            glm::mat4 ViewProjection;
        };

        struct ENGINE_API sDirectLightBuffer : public cListBuffer<sDirectLightData>
        {
            sDirectLightBuffer(usize size = 0) : cListBuffer<sDirectLightData>(size, K_SLOT_DIRECT_LIGHTS) {}
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

        struct ENGINE_API sPointLightBuffer : public cListBuffer<sPointLightData>
        {
            sPointLightBuffer(usize size = 0) : cListBuffer<sPointLightData>(size, K_SLOT_POINT_LIGHTS) {}
        };

        struct ENGINE_API sSpotLightData
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Direction = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
            float InnerCutoff = 1;
            float OuterCutoff = 1;
            glm::mat4 ViewProjection;
            float Near = 0;
            float Far = 1;
            float Constant = 1.0f;
            float Linear = 0.09f;
            float Quadratic = 0.032f;
        };

        struct ENGINE_API sSpotLightBuffer : public cListBuffer<sSpotLightData>
        {
            sSpotLightBuffer(usize size = 0) : cListBuffer<sSpotLightData>(size, K_SLOT_SPOT_LIGHTS) {}
        };

        struct ENGINE_API sMaterialData
        {
            // albedo mapping
            glm::vec4 Albedo = { 0.5, 0.5, 0.5, 1.0 };
            Boolean EnableAlbedoMap = false;
            // normal mapping
            Boolean EnableNormalMap = false;
            // parallax occlusion mapping
            Boolean EnableParallaxMap = false;
            float ParallaxHeightScale = 0.1;
            float ParallaxMinLayers = 8;
            float ParallaxMaxLayers = 32;
            // metal mapping
            float Metallness = 0.5f;
            Boolean EnableMetalMap = false;
            // roughness mapping
            float Roughness = 0.5f;
            Boolean EnableRoughnessMap = false;
            // ambient occlusion mapping
            float AO = 0.5f;
            Boolean EnableAOMap = false;
            // emission mapping
            glm::vec3 Emission = { 0, 0, 0 };
            Boolean EnableEmissionMap = false;
        };

        struct ENGINE_API sMaterialDataBuffer : public cListBuffer<sMaterialData>
        {
            sMaterialDataBuffer(usize size = 0) : cListBuffer<sMaterialData>(size, K_SLOT_MATERIALS) {}
        };

        struct ENGINE_API sShadowPcf final
        {
            float MinBias = 0.005f;
            float MaxBias = 0.05f;
            Boolean EnablePCF = false;
            float _pad0 = 0;
        };

        struct ENGINE_API sShadowPcfBuffer : public cItemBuffer<sShadowPcf>
        {
            sShadowPcfBuffer() : cItemBuffer<sShadowPcf>(K_SLOT_SHADOW_PCF) {}
        };

        struct ENGINE_API sSkeletonData final
        {
            glm::mat4 Transform;
        };

        struct ENGINE_API sSkeletonBuffer : public cListBuffer<sSkeletonData>
        {
            sSkeletonBuffer(usize size = 0) : cListBuffer<sSkeletonData>(size, K_SLOT_SKELETONS) {}
        };

        struct ENGINE_API sSsaoData final
        {
            int DirectionCount = 4;
            int SampleCount = 4;
            float Intensity = 1.0;
            float _pad0 = 0;
        };

        struct ENGINE_API sSsaoBuffer : public cItemBuffer<sSsaoData>
        {
            sSsaoBuffer() : cItemBuffer<sSsaoData>(K_SLOT_SSAO) {}
        };

        struct ENGINE_API sWidgetData
        {
            glm::mat4 ModelMatrix;
            glm::mat4 Projection;
            glm::vec4 Color = { 1, 1, 1, 1 };
            Boolean HasTexture = false;
            glm::vec2 AtlasUV[4] = {
                glm::vec2(0),
                glm::vec2(0),
                glm::vec2(0),
                glm::vec2(0)
            };
        };

        struct ENGINE_API sWidgetBuffer : public cListBuffer<sWidgetData>
        {
            sWidgetBuffer(usize count = 0) : cListBuffer<sWidgetData>(count, K_SLOT_WIDGETS) {}
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

        struct ENGINE_API sCharBuffer : public cListBuffer<sChar>
        {
            sCharBuffer(usize count = 0) : cListBuffer<sChar>(count, K_SLOT_CHARS) {}
        };

        struct ENGINE_API sText final
        {
            glm::mat4 ModelMatrix;
            glm::mat4 Projection;
            glm::vec4 Color = { 1, 1, 1, 1 };
        };

        struct ENGINE_API sTextBuffer : public cListBuffer<sText>
        {
            sTextBuffer(usize count = 0) : cListBuffer<sText>(count, K_SLOT_TEXTS) {}
        };
    }
}