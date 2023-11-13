#include <res/xml.h>

namespace xpe {

    namespace res {

        bool cXml::LoadFile(const char *filepath)
        {
            pugi::xml_parse_result result = m_Doc.load_file(filepath);
            if (!result)
                return false;

            FromXmlChildren(FromXml(m_Doc), XmlChildren);

            return true;
        }

        bool cXml::SaveFile(const char *filepath)
        {
            ToXmlChildren(ToXml(m_Doc), XmlChildren);
            return m_Doc.save_file(filepath);
        }

        void cXml::FromXmlChildren(xml root, xpe::core::vector<cXml*>& children)
        {
            for (auto* child : children) {
                FromXmlChildren(child->FromXml(root), child->XmlChildren);
            }
        }

        void cXml::ToXmlChildren(xml root, xpe::core::vector<cXml*>& children)
        {
            for (auto* child : children) {
                ToXmlChildren(child->ToXml(root), child->XmlChildren);
            }
        }

        void SetXmlValue(xml &root, const char *name, bool &value) {
            root.append_attribute(name).set_value(value);
        }

        void SetXmlValue(xml &root, const char *name, int &value) {
            root.append_attribute(name).set_value(value);
        }

        void SetXmlValue(xml &root, const char *name, u32 &value) {
            root.append_attribute(name).set_value(value);
        }

        void SetXmlValue(xml &root, const char *name, float &value) {
            root.append_attribute(name).set_value(value);
        }

        void SetXmlValue(xml &root, const char *name, double &value) {
            root.append_attribute(name).set_value(value);
        }

        void SetXmlValue(xml &root, const char *name, u64 &value) {
            root.append_attribute(name).set_value(value);
        }

        void SetXmlValue(xml &root, const char *name, s64 &value) {
            root.append_attribute(name).set_value(value);
        }

        void SetXmlValue(xml &root, const char *name, string &value) {
            root.append_attribute(name).set_value(value.c_str());
        }

        void SetXmlValue(xml &root, const char *name, std::string &value) {
            root.append_attribute(name).set_value(value.c_str());
        }

        void SetXmlValue(xml &root, const char *name, glm::vec2 &value) {
            hstringstream ss;
            ss << "x:" << value.x << " ";
            ss << "y:" << value.y;
            root.append_attribute(name).set_value(ss.str().c_str());
        }

        void SetXmlValue(xml &root, const char *name, glm::vec3 &value) {
            hstringstream ss;
            ss << "x:" << value.x << " ";
            ss << "y:" << value.y << " ";
            ss << "z:" << value.z;
            root.append_attribute(name).set_value(ss.str().c_str());
        }

        void SetXmlValue(xml &root, const char *name, glm::vec4 &value) {
            hstringstream ss;
            ss << "x:" << value.x << " ";
            ss << "y:" << value.y << " ";
            ss << "z:" << value.z << " ";
            ss << "w:" << value.w;
            root.append_attribute(name).set_value(ss.str().c_str());
        }

        void GetXmlValue(xml &root, const char *name, bool &value) {
            value = root.attribute(name).as_bool();
        }

        void GetXmlValue(xml &root, const char *name, int &value) {
            value = root.attribute(name).as_int();
        }

        void GetXmlValue(xml &root, const char *name, u32 &value) {
            value = root.attribute(name).as_uint();
        }

        void GetXmlValue(xml &root, const char *name, float &value) {
            value = root.attribute(name).as_float();
        }

        void GetXmlValue(xml &root, const char *name, double &value) {
            value = root.attribute(name).as_double();
        }

        void GetXmlValue(xml &root, const char *name, u64 &value) {
            value = root.attribute(name).as_ullong();
        }

        void GetXmlValue(xml &root, const char *name, s64 &value) {
            value = root.attribute(name).as_llong();
        }

        void GetXmlValue(xml &root, const char *name, string &value) {
            value = root.attribute(name).as_string();
        }

        void GetXmlValue(xml &root, const char *name, std::string &value) {
            value = root.attribute(name).as_string();
        }

        void GetXmlValue(xml &root, const char *name, glm::vec2 &value) {
            hstringstream ss(root.attribute(name).value());
            char coordinate;
            char equal;
            char separator;
            ss >> coordinate >> equal >> value.x >> separator;
            ss >> coordinate >> equal >> value.y;
        }

        void GetXmlValue(xml &root, const char *name, glm::vec3 &value) {
            hstringstream ss(root.attribute(name).value());
            char coordinate;
            char equal;
            char separator;
            ss >> coordinate >> equal >> value.x >> separator;
            ss >> coordinate >> equal >> value.y >> separator;
            ss >> coordinate >> equal >> value.z;
        }

        void GetXmlValue(xml &root, const char *name, glm::vec4 &value) {
            hstringstream ss(root.attribute(name).value());
            char coordinate;
            char equal;
            char separator;
            ss >> coordinate >> equal >> value.x >> separator;
            ss >> coordinate >> equal >> value.y >> separator;
            ss >> coordinate >> equal >> value.z >> separator;
            ss >> coordinate >> equal >> value.w;
        }

    }

}