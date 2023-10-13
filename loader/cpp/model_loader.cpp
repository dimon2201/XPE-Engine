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

            if (mesh->mTangents != nullptr) {
                vertex.Tangent = {
                        mesh->mTangents[i].x,
                        mesh->mTangents[i].y,
                        mesh->mTangents[i].z,
                };
            }

            return vertex;
        }

        static Mesh ParseMesh(aiMesh *mesh)
        {
            Mesh result;
            vector<u32> indices;

            result.Vertices.resize(mesh->mNumVertices);

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

            result.Indices.resize(indices.size());
            memcpy(result.Indices.data(), indices.data(), indices.size() * sizeof(u32));

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

        Ref<Model3D> ModelLoader::Load(const char* filepath, const vector<eLoadOption>& options)
        {
            if (m_Map.find(filepath) != m_Map.end()) {
                Ref<Model3D> modelRef;
                modelRef.Create(*m_Map[filepath]);
                return modelRef;
            }

            Model3D model;
            hstring directory = FileManager::GetDirectory(filepath);

            Assimp::Importer importer;
            u32 flags = AssimpManager::GetLoadFlags(options);
            const aiScene* scene = importer.ReadFile(filepath, flags);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                LogError("Failed to import 3D model file {0}", filepath);
                return {};
            }

            ParseMeshes(scene->mRootNode, scene, model, directory, flags);
            Ref<Model3D> modelRef = GeometryManager::AddModel(model);
            m_Map.insert({ filepath, modelRef });
            return modelRef;
        }

    }

}