#include <res/xml.h>

namespace xpe {

    namespace res {

        bool XmlObject::LoadFile(const char *filepath)
        {
            pugi::xml_parse_result result = m_Doc.load_file(filepath);

            if (!result)
                return false;
            else
                FromXml(m_Doc);

            return true;
        }

        bool XmlObject::SaveFile(const char *filepath)
        {
            ToXml(m_Doc);
            return m_Doc.save_file(filepath);
        }

    }

}