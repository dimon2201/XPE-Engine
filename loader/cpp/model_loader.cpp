#include <model_loader.h>

#include <anim/skeleton.h>

namespace xpe {

    namespace res {

        static void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
        {
            for (int i = 0; i < 4; ++i)
            {
                if (vertex.BoneIDs[i] < 0)
                {
                    vertex.BoneWeights[i] = weight;
                    vertex.BoneIDs[i] = boneID;
                    break;
                }
            }
        }

        static Vertex ParseVertex(aiMesh* mesh, u32 i)
        {
            Vertex vertex;

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

            if (mesh->mNormals) {
                vertex.Normal = {
                        mesh->mNormals[i].x,
                        mesh->mNormals[i].y,
                        mesh->mNormals[i].z
                };
            }

            if (mesh->mTangents) {
                vertex.Tangent = {
                        mesh->mTangents[i].x,
                        mesh->mTangents[i].y,
                        mesh->mTangents[i].z,
                };
            }

            return vertex;
        }

        static Geometry ParseMesh(aiMesh *mesh)
        {
            Geometry geometry;
            vector<u32> indices;

            geometry.Vertices.resize(mesh->mNumVertices);

            for (int i = 0 ; i < mesh->mNumVertices ; i++)
            {
                geometry.Vertices[i] = ParseVertex(mesh, i);
            }

            for (u32 i = 0 ; i < mesh->mNumFaces ; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (u32 j = 0 ; j < face.mNumIndices ; j++)
                {
                    indices.push_back(face.mIndices[j]);
                }
            }

            geometry.Indices.resize(indices.size());
            memcpy(geometry.Indices.data(), indices.data(), indices.size() * sizeof(u32));

            anim::sSkeleton skeleton;
            auto& bones = skeleton.Bones;
            int boneCounter = 0;
            for (int i = 0; i < mesh->mNumBones; i++)
            {
                int boneID;
                aiBone* bone = mesh->mBones[i];
                string boneName = bone->mName.C_Str();

                if (bones.find(boneName) == bones.end())
                {
                    anim::sBone newBone;
                    newBone.ID = boneCounter;
                    newBone.Name = boneName;
                    newBone.Offset = AssimpManager::ToMat4(bone->mOffsetMatrix);
                    bones.insert({ boneName, newBone });
                    boneID = boneCounter;
                    boneCounter++;
                }

                else {
                    boneID = bones[boneName].ID;
                }

                auto weights = bone->mWeights;
                int numWeights = bone->mNumWeights;

                for (int wi = 0; wi < numWeights; wi++)
                {
                    int vertexId = weights[wi].mVertexId;
                    float weight = weights[wi].mWeight;
                    SetVertexBoneData(geometry.Vertices[vertexId], boneID, weight);
                }
            }

            return geometry;
        }

        static void ParseMeshes(
            aiNode* node, const aiScene* scene,
            Model& model,
            const hstring& directory, u32 flags
        ) {
            auto& meshes = model.Meshes;

            for (u32 i = 0 ; i < node->mNumMeshes ; i++)
            {
                aiMesh* assimpMesh = scene->mMeshes[node->mMeshes[i]];
                Geometry mesh = ParseMesh(assimpMesh);
                meshes.push_back(mesh);
            }

            for (u32 i = 0 ; i < node->mNumChildren ; i++)
            {
                ParseMeshes(node->mChildren[i], scene, model, directory, flags);
            }
        }

        Model ModelLoader::Load(const char* filepath, const vector<eLoadOption>& options)
        {
            Model model;
            hstring directory = FileManager::GetDirectory(filepath);

            Assimp::Importer importer;
            u32 flags = AssimpManager::GetLoadFlags(options);
            const aiScene* scene = importer.ReadFile(filepath, flags);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                LogError("Failed to import 3D model file {0}", filepath);
                return {};
            }

            ParseMeshes(scene->mRootNode, scene, model, directory, flags);

            return GeometryManager::AddModel(model);
        }

    }

}