#include <rendering/context.hpp>
#include <rendering/shader.h>
#include <rendering/texture.h>

namespace xpe {

    namespace render {

        void Context::CreateShader(Shader& shader)
        {
            for (auto* stage : shader.Stages) {
                CreateShaderStage(*stage);
            }
        }

        void Context::CompileShader(Shader& shader)
        {
            for (auto* stage : shader.Stages) {
                CompileShaderStage(*stage);
            }
        }

        void Context::BindShader(const Shader* shader)
        {
            m_BoundShader = (Shader*)shader;
            for (const auto* stage : shader->Stages) {
                BindShaderStage(*stage);
            }
        }

        void Context::FreeShader(Shader& shader)
        {
            for (auto* stage : shader.Stages) {
                FreeShaderStage(*stage);
            }
            shader.Stages.clear();
        }

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