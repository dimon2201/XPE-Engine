#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define JsonFields(clazz, ...) \
                         \
NLOHMANN_DEFINE_TYPE_INTRUSIVE(clazz, __VA_ARGS__) \
                         \
void ToJson(json &root) override  \
{                                    \
    root[#clazz] = *this;           \
}                                    \
                                     \
void FromJson(json &root) override   \
{                                    \
    *this = root[#clazz];           \
}                                    \

#define JsonEnum(clazz, ...) \
NLOHMANN_JSON_SERIALIZE_ENUM(clazz, __VA_ARGS__)

namespace xpe {

    namespace res {

        struct ENGINE_API JsonObject
        {
            virtual void ToJson(json& root) = 0;
            virtual void FromJson(json& root) = 0;
        };

        ENGINE_API bool ReadJsonFile(const char* filepath, JsonObject& object);
        ENGINE_API bool WriteJsonFile(const char* filepath, JsonObject& object);

    }

}