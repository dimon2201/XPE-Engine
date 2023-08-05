#pragma once

#include <nlohmann/json.hpp>

#include <stl/string.h>

using json = nlohmann::json;

#define JsonFields(clazz, ...)                                  \
                                                                \
NLOHMANN_DEFINE_TYPE_INTRUSIVE(clazz, __VA_ARGS__)              \
                                                                \
clazz() : xpe::res::JsonObject() {}                             \
                                                                \
clazz(const xpe::core::string& jsonName) : xpe::res::JsonObject(jsonName) {}   \
                                                                \
void ToJson(json &root) override                                \
{                                                               \
    root[JsonName.c_str()] = *this;                             \
}                                                               \
                                                                \
void FromJson(json &root) override                              \
{                                                               \
    root.at(JsonName.c_str()).get_to(*this);                    \
}                                                               \

#define JsonEnum(clazz, ...) \
NLOHMANN_JSON_SERIALIZE_ENUM(clazz, __VA_ARGS__)

namespace xpe {

    namespace res {

        struct ENGINE_API JsonObject
        {
            xpe::core::string JsonName;

            JsonObject() = default;
            JsonObject(const xpe::core::string& jsonName) : JsonName(jsonName) {}
            virtual ~JsonObject() = default;

            virtual void ToJson(json& root) = 0;
            virtual void FromJson(json& root) = 0;
        };

        ENGINE_API bool ReadJsonFile(const char* filepath, JsonObject& object);
        ENGINE_API bool WriteJsonFile(const char* filepath, JsonObject& object);

    }

}