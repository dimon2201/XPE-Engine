#include <model_loader.hpp>

#include <anim/skeleton.hpp>

namespace xpe {

    namespace res {

        static void SetVertexBoneData(sVertex& vertex, int boneID, float weight)
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

        static sVertex ParseVertex(aiMesh* mesh, u32 i)
        {
            sVertex vertex;

            auto& pos = mesh->mVertices[i];
            vertex.Position = { pos.x, pos.y, pos.z };

            if (mesh->mTextureCoords[0]) {
                auto& uv = mesh->mTextureCoords[0][i];
                vertex.UV = { uv.x, uv.y };
            }

            auto& normal = mesh->mNormals[i];
            vertex.Normal = { normal.x, normal.y, normal.z };

            if (mesh->mTangents != nullptr)
            {
                auto& tangent = mesh->mTangents[i];
                vertex.Tangent = { tangent.x, tangent.y, tangent.z };
            }

            return vertex;
        }

        static sGeometry ParseMesh(aiMesh *mesh)
        {
            sGeometry geometry;
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
                    newBone.Offset = cAssimpManager::ToMat4(bone->mOffsetMatrix);
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
                sModel& model,
                const hstring& directory, u32 flags
        ) {
            auto& meshes = model.Geometries;

            for (u32 i = 0 ; i < node->mNumMeshes ; i++)
            {
                aiMesh* assimpMesh = scene->mMeshes[node->mMeshes[i]];
                sGeometry mesh = ParseMesh(assimpMesh);
                meshes.push_back(mesh);
            }

            for (u32 i = 0 ; i < node->mNumChildren ; i++)
            {
                ParseMeshes(node->mChildren[i], scene, model, directory, flags);
            }
        }

        sModel cModelLoader::Load(const char* filepath, const vector<eLoadOption>& options)
        {
            sModel model;
            hstring directory = cFileManager::GetDirectory(filepath);

            Assimp::Importer importer;
            u32 flags = cAssimpManager::GetLoadFlags(options);
            const aiScene* scene = importer.ReadFile(filepath, flags);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                LogError("Failed to import 3D model file {0}", filepath);
                return {};
            }

            ParseMeshes(scene->mRootNode, scene, model, directory, flags);

            return model;
        }

    }

}