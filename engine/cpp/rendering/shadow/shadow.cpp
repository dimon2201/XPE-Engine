#include <rendering/shadow/shadow.h>

namespace xpe {

    namespace render {

        Shadow* Shadow::s_Instance = nullptr;

        void Shadow::Init() {
            s_Instance = new Shadow();
        }

        void Shadow::Free() {
            delete s_Instance;
        }

        Shadow& Shadow::Get() {
            if (s_Instance == nullptr) {
                Init();
            }
            return *s_Instance;
        }

        Shadow::Shadow() {
            m_FilterBuffer = new ShadowFilterBuffer();
            m_Sampler = new TextureSampler();
            m_Map = new Texture();
            m_Coords = new Texture();
        }

        Shadow::~Shadow() {
            delete m_FilterBuffer;
            delete m_Sampler;
            delete m_Map;
            delete m_Coords;
        }

        ShadowFilterBuffer* Shadow::GetBuffer() {
            return m_FilterBuffer;
        }

        TextureSampler* Shadow::GetSampler() {
            return m_Sampler;
        }

        Texture* Shadow::GetMap() {
            return m_Map;
        }

        Texture* Shadow::GetCoords() {
            return m_Coords;
        }

        void Shadow::SetFilter(const ShadowFilter &filter) {
            m_FilterBuffer->FlushItem(filter);
        }

    }

}