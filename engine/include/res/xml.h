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

namespace xpe {

    namespace res {

        class ENGINE_API cXml
        {

        public:
            xpe::core::string Name;
            xpe::core::vector<cXml*> Children;

            virtual void ToXml(xml& root) = 0;
            virtual void FromXml(xml& root) = 0;

            bool LoadFile(const char* filepath);
            bool SaveFile(const char* filepath);

        protected:
            pugi::xml_document m_Doc;

        private:
            void FromXmlChildren(xml& root, xpe::core::vector<cXml*>& children);
            void ToXmlChildren(xml& root, xpe::core::vector<cXml*>& children);
        };

    }

}