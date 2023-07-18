#include <rendering/context.hpp>
#include <rendering/texture.h>

namespace xpe {

    namespace render {

        void Context::CreateTexture(Texture& texture) {

            switch (texture.Type) {

                case Texture::eType::TEXTURE_1D:
                    CreateTexture1D(texture);
                    break;

                case Texture::eType::TEXTURE_2D:
                    CreateTexture2D(texture);
                    break;

                case Texture::eType::TEXTURE_2D_ARRAY:
                    CreateTexture2DArray(texture);
                    break;

                case Texture::eType::TEXTURE_3D:
                    CreateTexture3D(texture);
                    break;

                case Texture::eType::TEXTURE_CUBE:
                    CreateTextureCube(texture);
                    break;

            }

        }

    }

}