#include <res/json.h>

namespace xpe {

    namespace res {

        bool ReadJsonFile(const char* filepath, cJson& object)
        {
            std::ifstream file(filepath, std::ios::in | std::ios::binary);

            if (!file.is_open())
            {
                return false;
            }

            json root = json::parse(file);

            object.FromJson(root);

            return true;
        }

        bool WriteJsonFile(const char* filepath, cJson& object)
        {
            std::ofstream file(filepath, std::ios::out | std::ios::binary);

            if (!file.is_open())
            {
                return false;
            }

            json root;
            object.ToJson(root);

            file << root.dump();

            return true;
        }

    }

}

namespace glm {

    void to_json(json& j, const glm::mat2& m)
    {
        j = json {
                { "m00", m[0][0] },
                { "m01", m[0][1] },
                { "m10", m[1][0] },
                { "m11", m[1][1] },
        };
    }

    void from_json(const json& j, glm::mat2& m)
    {
        j.at("m00").get_to(m[0][0]);
        j.at("m01").get_to(m[0][1]);
        j.at("m10").get_to(m[1][0]);
        j.at("m11").get_to(m[1][1]);
    }

    void to_json(json& j, const glm::mat3& m)
    {
        j = json {
                { "m00", m[0][0] },
                { "m01", m[0][1] },
                { "m02", m[0][2] },
                { "m10", m[1][0] },
                { "m11", m[1][1] },
                { "m12", m[1][2] },
                { "m20", m[2][0] },
                { "m21", m[2][1] },
                { "m22", m[2][2] }
        };
    }

    void from_json(const json& j, glm::mat3& m)
    {
        j.at("m00").get_to(m[0][0]);
        j.at("m01").get_to(m[0][1]);
        j.at("m02").get_to(m[0][2]);
        j.at("m10").get_to(m[1][0]);
        j.at("m11").get_to(m[1][1]);
        j.at("m12").get_to(m[1][2]);
        j.at("m20").get_to(m[2][0]);
        j.at("m21").get_to(m[2][1]);
        j.at("m22").get_to(m[2][2]);
    }

    void to_json(json& j, const glm::mat4& m)
    {
        j = json {
                { "m00", m[0][0] },
                { "m01", m[0][1] },
                { "m02", m[0][2] },
                { "m03", m[0][3] },
                { "m10", m[1][0] },
                { "m11", m[1][1] },
                { "m12", m[1][2] },
                { "m13", m[1][3] },
                { "m20", m[2][0] },
                { "m21", m[2][1] },
                { "m22", m[2][2] },
                { "m23", m[2][3] },
                { "m30", m[3][0] },
                { "m31", m[3][1] },
                { "m32", m[3][2] },
                { "m33", m[3][3] },
        };
    }

    void from_json(const json& j, glm::mat4& m)
    {
        j.at("m00").get_to(m[0][0]);
        j.at("m01").get_to(m[0][1]);
        j.at("m02").get_to(m[0][2]);
        j.at("m03").get_to(m[0][3]);
        j.at("m10").get_to(m[1][0]);
        j.at("m11").get_to(m[1][1]);
        j.at("m12").get_to(m[1][2]);
        j.at("m13").get_to(m[1][3]);
        j.at("m20").get_to(m[2][0]);
        j.at("m21").get_to(m[2][1]);
        j.at("m22").get_to(m[2][2]);
        j.at("m23").get_to(m[2][3]);
        j.at("m30").get_to(m[3][0]);
        j.at("m31").get_to(m[3][1]);
        j.at("m32").get_to(m[3][2]);
        j.at("m33").get_to(m[3][3]);
    }

}