#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        namespace context {

            void* SwapchainTargetView = nullptr;
            Viewport* BoundViewport = nullptr;
            void* BoundColorTargetView = nullptr;
            void* BoundDepthTargetView = nullptr;
            Shader* BoundShader = nullptr;
            Pipeline* BoundPipeline = nullptr;

            void CreateShader(Shader& shader)
            {
                for (auto* stage : shader.Stages)
                {
                    CreateShaderStage(*stage);
                }
            }

            void CompileShader(Shader& shader)
            {
                for (auto* stage : shader.Stages)
                {
                    CompileShaderStage(*stage);
                }
            }

            void BindShader(const Shader* shader)
            {
                BoundShader = (Shader*)shader;
                for (const auto* stage : shader->Stages)
                {
                    BindShaderStage(*stage);
                }
            }

            void FreeShader(Shader& shader)
            {
                for (auto* stage : shader.Stages)
                {
                    FreeShaderStage(*stage);
                }
                shader.Stages.clear();
            }

            void CreateTexture(Texture& texture)
            {

                switch (texture.Type)
                {

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

}