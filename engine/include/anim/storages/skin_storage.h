#pragma once

#include <anim/skin.h>

#include <rendering/buffers/vertex_buffer.h>
#include <rendering/buffers/index_buffer.h>

namespace xpe {

    namespace anim {

        using namespace render;

        struct ENGINE_API SkinStorage : public Object
        {
            Ref<Skin> AddSkin(const string& name, const Skin& skin);
            Ref<SkinModel> AddModel(const string& name, const SkinModel& model);

            void RemoveSkin(const string& name);
            void RemoveModel(const string& name);

            inline Ref<Skin>& GetSkin(const string& name) { return m_Skins[name]; }
            inline Ref<SkinModel>& GetModel(const string& name) { return m_Models[name]; }

            void Clear();

        private:
            unordered_map<string, Ref<Skin>> m_Skins;
            unordered_map<string, Ref<SkinModel>> m_Models;
        };

    }

}