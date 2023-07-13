#include <json/json.h>

#include <importers/gltf/gltf.hpp>

namespace xpe {

    namespace gltf {

        static vector <Buffer> p_Buffers;

        string GetFileFolderPath(const string &filePath) {
            xpe::core::usize i = filePath.size();
            while (filePath[--i] != '/');

            char str[xpe::core::K_MAX_STRING_BYTE_SIZE] = {};
            memcpy(&str[0], filePath.c_str(), i);

            return str;
        }

        Model3D GLTFImporter::Import(const char *filepath) {
            using namespace core;

            Model3D model;

            std::ifstream file(filepath, std::ios::in | std::ios::binary);

            if (!file.is_open()) {
                LogError("GLTFImporter failed to open file {}", filepath);
                return model;
            }

            file.seekg(0, std::ios::end);
            usize fileByteSize = file.tellg();
            char *str = (char *) malloc(fileByteSize);
            memset(str, 0, fileByteSize);
            file.seekg(0, std::ios::beg);
            file.read(str, fileByteSize);
            file.close();

            Json::Value root;
            Json::Reader reader;
            reader.parse(std::string(str), root);

            if (std::string(root["asset"]["version"].asCString()) != std::string("2.0")) {
                return model;
            }

            auto buffers = root["buffers"];
            for (auto &buffer: buffers) {
                Buffer buff;

                buff.ByteLength = buffer["byteLength"].asInt();
                buff.URI = buffer["uri"].asCString();
                buff.Data = malloc(buff.ByteLength);
                memset(buff.Data, 0, buff.ByteLength);

                string folderPath = GetFileFolderPath(filepath);
                string bufferPath = folderPath + "/" + buff.URI;

                std::ifstream bufferFile(bufferPath.c_str(), std::ios::in | std::ios::binary);
                bufferFile.read((char *) buff.Data, buff.ByteLength);
                bufferFile.close();

                p_Buffers.push_back(buff);
            }

            auto meshes = root["meshes"];
            for (auto &mesh: meshes) {
                // Read data to construct mesh
                auto positionAccessorId = mesh["primitives"][0]["attributes"]["POSITION"].asInt();
                auto texcoordAccessorId = mesh["primitives"][0]["attributes"]["TEXCOORD_0"].asInt();
                auto normalAccessorId = mesh["primitives"][0]["attributes"]["NORMAL"].asInt();
                auto indicesAccessorId = mesh["primitives"][0]["indices"].asInt();

                auto positionBufferViewId = root["accessors"][positionAccessorId]["bufferView"].asInt();
                auto texcoordBufferViewId = root["accessors"][texcoordAccessorId]["bufferView"].asInt();
                auto normalBufferViewId = root["accessors"][normalAccessorId]["bufferView"].asInt();
                auto indicesBufferViewId = root["accessors"][indicesAccessorId]["bufferView"].asInt();

                BufferView positionBufferView;
                BufferView texcoordBufferView;
                BufferView normalBufferView;
                BufferView indicesBufferView;

                positionBufferView.BufferId = root["bufferViews"][positionBufferViewId]["buffer"].asInt();
                positionBufferView.ByteLength = root["bufferViews"][positionBufferViewId]["byteLength"].asInt();
                positionBufferView.ByteOffset = root["bufferViews"][positionBufferViewId]["byteOffset"].asInt();

                texcoordBufferView.BufferId = root["bufferViews"][texcoordBufferViewId]["buffer"].asInt();
                texcoordBufferView.ByteLength = root["bufferViews"][texcoordBufferViewId]["byteLength"].asInt();
                texcoordBufferView.ByteOffset = root["bufferViews"][texcoordBufferViewId]["byteOffset"].asInt();

                normalBufferView.BufferId = root["bufferViews"][normalBufferViewId]["buffer"].asInt();
                normalBufferView.ByteLength = root["bufferViews"][normalBufferViewId]["byteLength"].asInt();
                normalBufferView.ByteOffset = root["bufferViews"][normalBufferViewId]["byteOffset"].asInt();

                indicesBufferView.BufferId = root["bufferViews"][indicesBufferViewId]["buffer"].asInt();
                indicesBufferView.ByteLength = root["bufferViews"][indicesBufferViewId]["byteLength"].asInt();
                indicesBufferView.ByteOffset = root["bufferViews"][indicesBufferViewId]["byteOffset"].asInt();

                // Construct mesh
                Mesh mesh;

                mesh.Vertices.Init(root["accessors"][positionAccessorId]["count"].asInt());

                u32 indexByteSize = sizeof(u32);

                auto indicesComponentType = root["accessors"][indicesAccessorId]["componentType"].asInt();

                if (indicesComponentType == 5123) {
                    indexByteSize = 2;
                }

                mesh.Indices.Init(indicesBufferView.ByteLength / indexByteSize);

                for (usize i = 0; i < mesh.Vertices.Count(); i++) {

                    void* positionPtr = (void*)(
                        (usize) p_Buffers[positionBufferView.BufferId].Data +
                        (usize) positionBufferView.ByteOffset +
                        ((usize) sizeof(glm::vec3) * i)
                    );

                    void* texcoordPtr = (void*)(
                        (usize) p_Buffers[texcoordBufferView.BufferId].Data +
                        (usize) texcoordBufferView.ByteOffset +
                        ((usize) sizeof(glm::vec2) * i)
                    );

                    void* normalPtr = (void*)(
                        (usize) p_Buffers[normalBufferView.BufferId].Data +
                        (usize) normalBufferView.ByteOffset +
                        ((usize) sizeof(glm::vec3) * i)
                    );

                    memcpy(&mesh.Vertices[i].Position, positionPtr, sizeof(glm::vec3));
                    memcpy(&mesh.Vertices[i].UV, texcoordPtr, sizeof(glm::vec2));
                    memcpy(&mesh.Vertices[i].Normal, normalPtr, sizeof(glm::vec3));
                }

                for (usize i = 0; i < mesh.Indices.Count(); i++) {
                    void* indexPtr = (void*)(
                        (usize) p_Buffers[indicesBufferView.BufferId].Data +
                        (usize) indicesBufferView.ByteOffset +
                        ((usize) indexByteSize * i)
                    );

                    memcpy(&mesh.Indices[i], indexPtr, indexByteSize);
                }

                model.Meshes.emplace_back(mesh);
            }

            p_Buffers.clear();

            return model;
        }

    }

}