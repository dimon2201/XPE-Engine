#pragma once

#include <rendering/buffers/vertex_buffer.h>
#include <rendering/buffers/index_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace render;

        static void InitUV(Vertex* v0, Vertex* v1, Vertex* v2, Vertex* v3)
        {
            v0->UV = { 0, 0 };
            v1->UV = { 0, 1 };
            v2->UV = { 1, 1 };
            v3->UV = { 1, 0 };
        }

        static void InitNormal(Vertex* v0, Vertex* v1, Vertex* v2)
        {
            glm::vec3 x1 = v1->Position - v0->Position;
            glm::vec3 x2 = v2->Position - v0->Position;
            glm::vec3 normal = -glm::normalize(glm::cross(x1, x2));
            v0->Normal = normal;
            v1->Normal = normal;
            v2->Normal = normal;
        }

        static void InitNormal(Vertex* v0, Vertex* v1, Vertex* v2, Vertex* v3)
        {
            glm::vec3 x1 = v1->Position - v0->Position;
            glm::vec3 x2 = v3->Position - v0->Position;
            glm::vec3 normal = -glm::normalize(glm::cross(x1, x2));
            v0->Normal = normal;
            v1->Normal = normal;
            v2->Normal = normal;
            v3->Normal = normal;
        }

        static void InitTangent(Vertex* v0, Vertex* v1, Vertex* v2, Vertex* v3)
        {
            glm::vec3& pos0 = v0->Position;
            glm::vec3& pos1 = v1->Position;
            glm::vec3& pos2 = v2->Position;

            glm::vec2& uv0 = v0->UV;
            glm::vec2& uv1 = v1->UV;
            glm::vec2& uv2 = v2->UV;

            // Calculate Triangle edges
            glm::vec3 dpos_1 = pos1 - pos0;
            glm::vec3 dpos_2 = pos2 - pos0;

            // Calculate delta UV
            glm::vec2 duv_1 = uv1 - uv0;
            glm::vec2 duv_2 = uv2 - uv0;

            // Calculate Tangents
            float r = 1.0f / (duv_1.x * duv_2.y - duv_1.y * duv_2.x);
            glm::vec3 tangent = (dpos_1 * duv_2.y - dpos_2 * duv_1.y) * r;
            v0->Tangent = tangent;
            v1->Tangent = tangent;
            v2->Tangent = tangent;
            v3->Tangent = tangent;
        }

        struct Geometry : public Object
        {
            render::ePrimitiveTopology PrimitiveTopology = render::ePrimitiveTopology::DEFAULT;
            usize VertexOffset = 0;
            vector<Vertex> Vertices;
            usize IndexOffset = 0;
            vector<u32> Indices;
        };

    }

}