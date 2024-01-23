#include <core/types.hpp>
#include <rendering/bindings.hpp>
#include <rendering/material/material_manager.hpp>

namespace xpe
{
    namespace render
    {
        void MMaterial::Init()
        {
            Buffers::Material = new cMaterialDataBuffer(1024);
        }

        void MMaterial::Free()
        {
            delete Buffers::Material;
        }

        void MMaterial::Update(cScene* scene)
        {
            s32 index = 0;
            auto components = scene->GetComponents<CMaterial2>();
            for (auto [entity, material] : components.each())
            {
                material.BufferIndex = index;

                sMaterialData data;
                data.DiffuseColor = material.DiffuseColor;
                data.DiffuseTextureOffsets = material.DiffuseTexture.Offsets;

                Buffers::Material->GetList()[index] = data;

                index += 1;
            }

            Buffers::Material->Flush();
        }

        CMaterial2 MMaterial::AddMaterial(const sAtlas2DTexture& diffuse)
        {
            CMaterial2 material(diffuse);

            return material;
        }
    }

}