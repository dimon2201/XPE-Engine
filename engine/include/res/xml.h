#pragma once

#include <stl/string.h>
#include <stl/vector.h>

#include <pugixml.hpp>

using xml = pugi::xml_node;

#define XmlClass(clazz, ...)

#define XmlClass(clazz, ...)                                    \
                                                                \
clazz() = default;                                              \
                                                                \
clazz(const xpe::core::string& name) { Name = name; }           \
                                                                \

#define XmlEnum(clazz, ...) \
NLOHMANN_JSON_SERIALIZE_ENUM(clazz, __VA_ARGS__)

namespace xpe {

    namespace res {

        struct ENGINE_API XmlObject
        {
            xpe::core::string Name;
            xpe::core::vector<XmlObject*> Children;

            virtual void ToXml(xml& root) = 0;
            virtual void FromXml(xml& root) = 0;

            bool LoadFile(const char* filepath);
            bool SaveFile(const char* filepath);

        protected:
            pugi::xml_document m_Doc;
        };

    }

}