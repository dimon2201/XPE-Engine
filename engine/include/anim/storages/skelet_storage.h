#pragma once

#include <anim/skelet.h>

#include <rendering/buffers/bone_buffer.h>

namespace xpe {

    namespace anim {

        using namespace render;

        struct ENGINE_API SkeletStorage : public Object
        {
            inline BoneBuffer& GetBoneBuffer(void* skeletAddress) { return *m_BoneBuffers[skeletAddress]; }

            Ref<Skelet> Add(const string& name, const Skelet& skelet);

            void Remove(const string& name);

            inline Ref<Skelet>& Get(const string& name) { return m_Skelets[name]; }

            void Clear();

        private:
            unordered_map<string, Ref<Skelet>> m_Skelets;
            unordered_map<void*, Ref<BoneBuffer>> m_BoneBuffers;
        };

    }

}