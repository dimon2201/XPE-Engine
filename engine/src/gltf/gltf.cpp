#include <fstream>
#include <cstdlib>
#include <json/json.h>
#include <glm.hpp>

#include "gltf.hpp"
#include "../core/types.hpp"

std::string GetFileFolderPath(const std::string& filePath)
{
    xpe::core::usize i = filePath.size();
    while (filePath[--i] != '/');

    char str[xpe::core::K_MAX_STRING_BYTE_SIZE] = {};
    memcpy(&str[0], filePath.c_str(), i);

    return std::string(str);
}

#include <iostream>
xpe::gltf::cGLTFModel::cGLTFModel(const char* filePath)
{
    using namespace core;

    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    file.seekg(0, std::ios::end);
    usize fileByteSize = file.tellg();
    char* str = (char*)malloc(fileByteSize);
    memset(str, 0, fileByteSize);
    file.seekg(0, std::ios::beg);
    file.read(str, fileByteSize);
    file.close();

    Json::Value root;
    Json::Reader reader;
    reader.parse(std::string(str), root);

    if (std::string(root["asset"]["version"].asCString()) != std::string("2.0"))
    {
        return;
    }

    auto buffers = root["buffers"];
    for (auto& buffer : buffers)
    {
        xBuffer buff;
        
        buff.ByteLength = buffer["byteLength"].asInt();
        buff.URI = buffer["uri"].asCString();
        buff.Data = malloc(buff.ByteLength);
        memset(buff.Data, 0, buff.ByteLength);

        std::string folderPath = GetFileFolderPath(std::string(filePath));
        std::string bufferPath = folderPath + "/" + buff.URI;

        std::ifstream bufferFile(bufferPath, std::ios::in | std::ios::binary);
        bufferFile.read((char*)buff.Data, buff.ByteLength);
        bufferFile.close();

        _buffs.push_back(buff);
    }

    auto meshes = root["meshes"];
    for (auto& mesh : meshes)
    {
        // Read data to construct mesh
        auto positionAccessorId = mesh["primitives"][0]["attributes"]["POSITION"].asInt();
        auto texcoordAccessorId = mesh["primitives"][0]["attributes"]["TEXCOORD_0"].asInt();
        auto normalAccessorId = mesh["primitives"][0]["attributes"]["NORMAL"].asInt();
        auto indicesAccessorId = mesh["primitives"][0]["indices"].asInt();

        auto positionBufferViewId = root["accessors"][positionAccessorId]["bufferView"].asInt();
        auto texcoordBufferViewId = root["accessors"][texcoordAccessorId]["bufferView"].asInt();
        auto normalBufferViewId = root["accessors"][normalAccessorId]["bufferView"].asInt();
        auto indicesBufferViewId = root["accessors"][indicesAccessorId]["bufferView"].asInt();

        xBufferView positionBufferView;
        xBufferView texcoordBufferView;
        xBufferView normalBufferView;
        xBufferView indicesBufferView;

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
        xMesh mesh;
        
        mesh.VertexCount = root["accessors"][positionAccessorId]["count"].asInt();
        mesh.Vertices = (xVertex*)malloc(mesh.VertexCount * k_vertexSize);
        memset(mesh.Vertices, 0, mesh.VertexCount * k_vertexSize);

        auto indicesComponentType = root["accessors"][indicesAccessorId]["componentType"].asInt();
        if (indicesComponentType == 5123)
        {
            mesh.IndexByteSize = 2;
        }
        else if (indicesComponentType == 5125)
        {
            mesh.IndexByteSize = 4;
        }
        mesh.IndexCount = indicesBufferView.ByteLength / mesh.IndexByteSize;
        mesh.Indices = (core::u32*)malloc(mesh.IndexCount * sizeof(core::u32));
        memset(mesh.Indices, 0, mesh.IndexCount * sizeof(core::u32));

        for (usize i = 0; i < mesh.VertexCount; i++)
        {
            void* positionPtr = 
                (void*)
                (
                    (usize)_buffs[positionBufferView.BufferId].Data +
                    (usize)positionBufferView.ByteOffset +
                    ((usize)sizeof(glm::vec3) * i)
                );
            void* texcoordPtr = 
                (void*)
                (
                    (usize)_buffs[texcoordBufferView.BufferId].Data +
                    (usize)texcoordBufferView.ByteOffset +
                    ((usize)sizeof(glm::vec2) * i)
                );
            void* normalPtr = 
                (void*)
                (
                    (usize)_buffs[normalBufferView.BufferId].Data +
                    (usize)normalBufferView.ByteOffset +
                    ((usize)sizeof(glm::vec3) * i)
                );

            memcpy(&mesh.Vertices[i].Position, positionPtr, sizeof(glm::vec3));
            memcpy(&mesh.Vertices[i].Texcoord, texcoordPtr, sizeof(glm::vec2));
            memcpy(&mesh.Vertices[i].Normal, normalPtr, sizeof(glm::vec3));
        }

        for (usize i = 0; i < mesh.IndexCount; i++)
        {
            void* indexPtr =
                (void*)
                (
                    (usize)_buffs[indicesBufferView.BufferId].Data +
                    (usize)indicesBufferView.ByteOffset +
                    ((usize)mesh.IndexByteSize * i)
                );

            memcpy(&mesh.Indices[i], indexPtr, mesh.IndexByteSize);
        }

        _meshes.push_back(mesh);
    }
}

xpe::gltf::cGLTFModel::~cGLTFModel()
{
    for (auto& buff : _buffs)
    {
        if (buff.Data != nullptr)
        {
            free(buff.Data);
        }
    }

    for (auto& mesh : _meshes)
    {
        if (mesh.Vertices != nullptr)
        {
            free(mesh.Vertices);
        }

        if (mesh.Indices != nullptr)
        {
            free(mesh.Indices);
        }
    }
}