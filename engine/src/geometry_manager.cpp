#include <geometry_manager.hpp>

namespace xpe
{
    namespace render
    {
        void MGeometry::Init()
        {
            Buffers::Vertex = new sVertexBuffer();
            Buffers::Index  = new sIndexBuffer();
        }

        void MGeometry::Free()
        {
            delete Buffers::Vertex;
            delete Buffers::Index;
        }

        CGeometryInfo MGeometry::Add(const CGeometry& geometry)
        {
            CGeometryInfo geometryInfo;
            geometryInfo.PrimitiveTopology = geometry.PrimitiveTopology;
            geometryInfo.VertexOffset = Buffers::Vertex->AddVertices(geometry.Vertices);
            geometryInfo.IndexOffset = Buffers::Index->AddIndices(geometry.Indices);
            geometryInfo.VertexCount = geometry.Vertices.size();
            geometryInfo.IndexCount = geometry.Indices.size();
            Buffers::Vertex->Flush();
            Buffers::Index->Flush();
            return geometryInfo;
        }

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

            auto& tangent = mesh->mTangents[i];
            vertex.Tangent = { tangent.x, tangent.y, tangent.z };

            return vertex;
        }

        static CGeometry ParseMesh(aiMesh *mesh)
        {
            CGeometry geometry;
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

            CSkeleton skeleton;
            auto& bones = skeleton.Bones;
            int boneCounter = 0;
            for (int i = 0; i < mesh->mNumBones; i++)
            {
                int boneID;
                aiBone* bone = mesh->mBones[i];
                string boneName = bone->mName.C_Str();

                if (bones.find(boneName) == bones.end())
                {
                    sBone newBone;
                    newBone.ID = boneCounter;
                    newBone.Name = boneName;
                    newBone.Offset = MAssimp::ToMat4(bone->mOffsetMatrix);
                    bones.insert({ boneName, newBone });
                    boneID = boneCounter;
                    boneCounter++;
                }

                else
                {
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

        static void ParseGeometry(
                aiNode* node, const aiScene* scene,
                CGeometry& geometry,
                const hstring& directory, u32 flags
        ) {
            auto& meshes = geometry.Children;

            for (u32 i = 0 ; i < node->mNumMeshes ; i++)
            {
                aiMesh* assimpMesh = scene->mMeshes[node->mMeshes[i]];
                CGeometry mesh = ParseMesh(assimpMesh);
                meshes.push_back(mesh);
            }

            for (u32 i = 0 ; i < node->mNumChildren ; i++)
            {
                ParseGeometry(node->mChildren[i], scene, geometry, directory, flags);
            }
        }

        CGeometry MGeometry::Load(const char* filepath, const vector<eLoadOption>& options)
        {
            if (MFile::NotExists(filepath))
            {
                LogError("Model not found in {}", filepath);
                return {};
            }

            hstring directory = MFile::GetDirectory(filepath);
            Assimp::Importer importer;
            u32 flags = MAssimp::GetLoadFlags(options);
            const aiScene* scene = importer.ReadFile(filepath, flags);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                LogError("Failed to import 3D model file {0}", filepath);
                return {};
            }

            CGeometry geometry;
            ParseGeometry(scene->mRootNode, scene, geometry, directory, flags);
            return geometry;
        }

        void MGeometry::Merge(CGeometry& geometry)
        {
            usize vertexCount = 0;
            for (auto& child : geometry.Children)
            {
                for (auto& vertex : child.Vertices)
                {
                    geometry.Vertices.emplace_back(vertex);
                }
                for (auto& index : child.Indices)
                {
                    geometry.Indices.emplace_back(index + vertexCount);
                }
                vertexCount += child.Vertices.size();
            }
            geometry.Children.clear();
        }
    }
}