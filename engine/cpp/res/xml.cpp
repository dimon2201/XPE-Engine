#include <res/xml.h>

namespace xpe {

    namespace res {

        bool cXml::LoadFile(const char *filepath)
        {
            pugi::xml_parse_result result = m_Doc.load_file(filepath);
            if (!result)
                return false;

            FromXml(m_Doc);
            FromXmlChildren(m_Doc, Children);

            return true;
        }

        bool cXml::SaveFile(const char *filepath)
        {
            ToXml(m_Doc);
            ToXmlChildren(m_Doc, Children);
            return m_Doc.save_file(filepath);
        }

        void cXml::FromXmlChildren(xml &root, xpe::core::vector<cXml*>& children)
        {
            for (auto* child : children) {
                child->FromXml(root);
                FromXmlChildren(root, child->Children);
            }
        }

        void cXml::ToXmlChildren(xml &root, xpe::core::vector<cXml*>& children)
        {
            for (auto* child : children) {
                child->ToXml(root);
                ToXmlChildren(root, child->Children);
            }
        }

    }

}