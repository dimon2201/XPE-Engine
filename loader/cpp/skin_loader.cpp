#include <skin_loader.h>

#include <anim/skelet.h>

namespace xpe {

    namespace res {

        static void SetSkeletalVertex(SkeletalVertex& vertex, int boneID, float weight)
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

        static SkeletalVertex ParseVertex(aiMesh* mesh, u32 i)
        {
            SkeletalVertex vertex;

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

        static Skin ParseSkin(aiMesh *mesh)
        {
            Skin skin;
            vector<u32> indices;
            int boneCounter = 0;

            skin.Vertices.Init(mesh->mNumVertices);

            for (int i = 0 ; i < mesh->mNumVertices ; i++)
            {
                skin.Vertices[i] = ParseVertex(mesh, i);
            }

            for (u32 i = 0 ; i < mesh->mNumFaces ; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (u32 j = 0 ; j < face.mNumIndices ; j++)
                {
                    indices.push_back(face.mIndices[j]);
                }
            }

            skin.Indices.Init((int) indices.size());
            for (int i = 0 ; i < indices.size() ; i++)
            {
                skin.Indices[i] = indices[i];
            }

            Skelet skelet;
            auto& bones = skelet.Bones;
            for (int i = 0; i < mesh->mNumBones; i++)
            {
                int boneID;
                string boneName = mesh->mBones[i]->mName.C_Str();

                if (bones.find(boneName) == bones.end())
                {
                    Bone newBone;
                    newBone.ID = boneCounter;
                    newBone.Name = boneName;
                    newBone.Offset = AssimpConversion::ToMat4(mesh->mBones[i]->mOffsetMatrix);
                    bones.insert({ boneName, newBone });
                    boneID = boneCounter;
                    boneCounter++;
                }

                else {
                    boneID = bones[boneName].ID;
                }

                auto weights = mesh->mBones[i]->mWeights;
                int numWeights = mesh->mBones[i]->mNumWeights;

                for (int wi = 0; wi < numWeights; wi++)
                {
                    int vertexId = weights[wi].mVertexId;
                    float weight = weights[wi].mWeight;
                    SetSkeletalVertex(skin.Vertices[vertexId], boneID, weight);
                }
            }

            return skin;
        }

        static void ParseSkins(
                aiNode* node, const aiScene* scene,
                SkinModel& model,
                const hstring& directory, u32 flags
        ) {
            auto& skins = model.Skins;

            for (u32 i = 0 ; i < node->mNumMeshes ; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                Skin skin = ParseSkin(mesh);
                skins.push_back(skin);
            }

            for (u32 i = 0 ; i < node->mNumChildren ; i++)
            {
                ParseSkins(node->mChildren[i], scene, model, directory, flags);
            }
        }

        static const std::unordered_map<SkinLoader::eOption, aiPostProcessSteps> s_SkinOptions =
        {
            { SkinLoader::eOption::TRIANGULATE, aiProcess_Triangulate },
            { SkinLoader::eOption::FLIP_UV, aiProcess_FlipUVs },
            { SkinLoader::eOption::CALC_TANGENTS, aiProcess_CalcTangentSpace },
            { SkinLoader::eOption::OPTIMIZE_MESHES, aiProcess_OptimizeMeshes }
        };

        static u32 GetSkinFlags(const vector<SkinLoader::eOption>& options)
        {
            u32 flags = 0;
            for (const auto& option : options)
            {
                flags |= s_SkinOptions.at(option);
            }
            return flags;
        }

        Ref<SkinModel> SkinLoader::Load(const char* filepath, const vector<SkinLoader::eOption>& options)
        {
            SkinModel model;
            hstring directory = os::FileManager::GetDirectory(filepath);

            Assimp::Importer importer;
            u32 flags = GetSkinFlags(options);
            const aiScene* scene = importer.ReadFile(filepath, flags);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                LogError("Failed to import 3D skin model file {0}", filepath);
                return {};
            }

            ParseSkins(scene->mRootNode, scene, model, directory, flags);

            return m_Storage->AddModel(filepath, model);
        }

    }

}