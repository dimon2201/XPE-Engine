#pragma once

#include <geometry/geometry.h>
#include <geometry/vertices.h>

#include <rendering/materials/material.h>

namespace xpe {

    namespace math {

        using namespace render;

        JsonEnum(ePrimitiveTopology, {
            { ePrimitiveTopology::DEFAULT, "DEFAULT" },
            { ePrimitiveTopology::TRIANGLE_LIST, "TRIANGLE_LIST" },
            { ePrimitiveTopology::TRIANGLE_STRIP, "TRIANGLE_STRIP" },
            { ePrimitiveTopology::POINT_LIST, "POINT_LIST" },
            { ePrimitiveTopology::LINE_LIST, "LINE_LIST" },
            { ePrimitiveTopology::LINE_STRIP, "LINE_STRIP" },
        })

        Json(VertexBuffer<Vertex2D>, List)
        Json(VertexBuffer<Vertex3D>, List)
        Json(VertexBuffer<VertexSkeletal>, List)

        Json(IndexBuffer, List)

        Json(Geometry<Vertex2D>, PrimitiveTopology, VertexOffset, Vertices, IndexOffset, Indices)
        Json(Geometry<Vertex3D>, PrimitiveTopology, VertexOffset, Vertices, IndexOffset, Indices)
        Json(Geometry<VertexSkeletal>, PrimitiveTopology, VertexOffset, Vertices, IndexOffset, Indices)

        struct ENGINE_API Triangle2D : public Geometry<Vertex2D> {
            Triangle2D();
        };

        struct ENGINE_API Triangle : public Geometry<Vertex3D> {
            Triangle();
        };

        struct ENGINE_API Quad2D : public Geometry<Vertex2D> {
            Quad2D();
        };

        struct ENGINE_API Quad : public Geometry<Vertex3D> {
            Quad();
        };

        struct ENGINE_API Cube : public Geometry<Vertex3D> {
            Cube();
        };

        struct ENGINE_API Plane : public Geometry<Vertex3D> {
            Plane(s32 size = 16);
        };

        struct ENGINE_API Sphere : public Geometry<Vertex3D> {
            glm::vec3 Center;
            float Radius;

            Sphere(s32 xSegments = 64, s32 ySegments = 64);
        };

        template<typename T>
        static void InitUV(vector<T>& vertices)
        {
            vertices[0].UV = { 0, 0 };
            vertices[1].UV = { 0, 1 };
            vertices[2].UV = { 1, 1 };
            vertices[3].UV = { 1, 0 };

            vertices[4].UV = { 0, 0 };
            vertices[5].UV = { 0, 1 };
            vertices[6].UV = { 1, 1 };
            vertices[7].UV = { 1, 0 };

            vertices[8].UV = { 0, 0 };
            vertices[9].UV = { 0, 1 };
            vertices[10].UV = { 1, 1 };
            vertices[11].UV = { 1, 0 };

            vertices[12].UV = { 0, 0 };
            vertices[13].UV = { 0, 1 };
            vertices[14].UV = { 1, 1 };
            vertices[15].UV = { 1, 0 };

            vertices[16].UV = { 0, 1 };
            vertices[17].UV = { 0, 0 };
            vertices[18].UV = { 1, 1 };
            vertices[19].UV = { 1, 0 };

            vertices[20].UV = { 0, 0 };
            vertices[21].UV = { 0, 1 };
            vertices[22].UV = { 1, 1 };
            vertices[23].UV = { 1, 0 };
        }

        template<typename T>
        static void InitNormal(vector<T>& vertices)
        {
            glm::vec3 normal1 = -glm::normalize(glm::cross(
                    vertices[1].Position - vertices[0].Position,
                    vertices[3].Position - vertices[0].Position
            ));

            glm::vec3 normal2 = glm::normalize(glm::cross(
                    vertices[5].Position - vertices[4].Position,
                    vertices[7].Position - vertices[4].Position
            ));

            glm::vec3 normal3 = -glm::normalize(glm::cross(
                    vertices[9].Position - vertices[8].Position,
                    vertices[11].Position - vertices[8].Position
            ));

            glm::vec3 normal4 = glm::normalize(glm::cross(
                    vertices[13].Position - vertices[12].Position,
                    vertices[15].Position - vertices[12].Position
            ));

            glm::vec3 normal5 = -glm::normalize(glm::cross(
                    vertices[17].Position - vertices[16].Position,
                    vertices[19].Position - vertices[16].Position
            ));

            glm::vec3 normal6 = glm::normalize(glm::cross(
                    vertices[21].Position - vertices[20].Position,
                    vertices[23].Position - vertices[20].Position
            ));

            vertices[0].Normal = normal1;
            vertices[1].Normal = normal1;
            vertices[2].Normal = normal1;
            vertices[3].Normal = normal1;

            vertices[4].Normal = normal2;
            vertices[5].Normal = normal2;
            vertices[6].Normal = normal2;
            vertices[7].Normal = normal2;

            vertices[8].Normal = normal3;
            vertices[9].Normal = normal3;
            vertices[10].Normal = normal3;
            vertices[11].Normal = normal3;

            vertices[12].Normal = normal4;
            vertices[13].Normal = normal4;
            vertices[14].Normal = normal4;
            vertices[15].Normal = normal4;

            vertices[16].Normal = normal5;
            vertices[17].Normal = normal5;
            vertices[18].Normal = normal5;
            vertices[19].Normal = normal5;

            vertices[20].Normal = normal6;
            vertices[21].Normal = normal6;
            vertices[22].Normal = normal6;
            vertices[23].Normal = normal6;
        }

        template<typename T>
        static void InitTangent(vector<T>& vertices)
        {
            InitTangent(&vertices[0], &vertices[1], &vertices[2], &vertices[3]);
            InitTangent(&vertices[4], &vertices[5], &vertices[6], &vertices[7]);
            InitTangent(&vertices[8], &vertices[9], &vertices[10], &vertices[11]);
            InitTangent(&vertices[12], &vertices[13], &vertices[14], &vertices[15]);
            InitTangent(&vertices[16], &vertices[17], &vertices[18], &vertices[19]);
            InitTangent(&vertices[20], &vertices[21], &vertices[22], &vertices[23]);
        }

        typedef Geometry<Vertex3D> Mesh;

        struct ENGINE_API Model3D : public Object
        {
            ePrimitiveTopology PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;
            vector<Mesh> Meshes;
            usize VertexOffset = 0;
            usize VertexCount = 0;
            usize IndexOffset = 0;
            vector<u32> Indices;

            inline Mesh& operator [](u32 i) { return Meshes[i]; }
        };

        Json(Model3D, PrimitiveTopology, Meshes, VertexOffset, VertexCount, IndexOffset, Indices)

    }

}