#include <model_loader.h>

namespace xpe {

    namespace res {

        static Vertex3D ParseVertex(aiMesh* mesh, u32 i)
        {
            Vertex3D vertex;

            vertex.Position = {
                    mesh->mVertices[i].x,
                    mesh->mVertices[i].y,
                    mesh->mVertices[i].z
            };

            if (mesh->mTextureCoords[0]) {
                vertex.UV = {
                        mesh->mTextureCoords[0][i].x,
                        mesh->mTextureCoords[0][i].y
                };
            }

            vertex.Normal = {
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z
            };

            vertex.Tangent = {
                    mesh->mTangents[i].x,
                    mesh->mTangents[i].y,
                    mesh->mTangents[i].z,
            };

            return vertex;
        }

        static Mesh ParseMesh(aiMesh *mesh)
        {
            Mesh result;
            vector<u32> indices;

            result.Vertices.Init(mesh->mNumVertices);

            for (int i = 0 ; i < mesh->mNumVertices ; i++)
            {
                result.Vertices[i] = ParseVertex(mesh, i);
            }

            for (u32 i = 0 ; i < mesh->mNumFaces ; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (u32 j = 0 ; j < face.mNumIndices ; j++)
                {
                    indices.push_back(face.mIndices[j]);
                }
            }

            result.Indices.Init((int) indices.size());
            for (int i = 0 ; i < indices.size() ; i++)
            {
                result.Indices[i] = indices[i];
            }

            return result;
        }

        static void ParseMeshes(
                aiNode* node, const aiScene* scene,
                Model3D& model,
                const hstring& directory, u32 flags
        ) {
            auto& meshes = model.Meshes;

            for (u32 i = 0 ; i < node->mNumMeshes ; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                Mesh result = ParseMesh(mesh);
                meshes.push_back(result);
            }

            for (u32 i = 0 ; i < node->mNumChildren ; i++)
            {
                ParseMeshes(node->mChildren[i], scene, model, directory, flags);
            }
        }

        static const std::unordered_map<ModelLoader::eOption, aiPostProcessSteps> s_ModelOptions =
        {
                { ModelLoader::eOption::TRIANGULATE, aiProcess_Triangulate },
                { ModelLoader::eOption::FLIP_UV, aiProcess_FlipUVs },
                { ModelLoader::eOption::CALC_TANGENTS, aiProcess_CalcTangentSpace },
                { ModelLoader::eOption::OPTIMIZE_MESHES, aiProcess_OptimizeMeshes }
        };

        static u32 GetModelFlags(const vector<ModelLoader::eOption>& options)
        {
            u32 flags = 0;
            for (const auto& option : options)
            {
                flags |= s_ModelOptions.at(option);
            }
            return flags;
        }

        Ref<Model3D> ModelLoader::Load(const char* filepath, const vector<ModelLoader::eOption>& options)
        {
            Model3D model;
            hstring directory = os::FileManager::GetDirectory(filepath);

            Assimp::Importer importer;
            u32 flags = GetModelFlags(options);
            const aiScene* scene = importer.ReadFile(filepath, flags);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                LogError("Failed to import 3D model file {0}", filepath);
                return {};
            }

            ParseMeshes(scene->mRootNode, scene, model, directory, flags);

            m_Scenes.insert({ filepath, scene });

            return m_Storage->AddModel(filepath, model);
        }

    }

}