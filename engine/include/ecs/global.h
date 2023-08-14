#pragma once

namespace xpe {

    namespace ecs {

        using namespace core;

        struct ENGINE_API Global : public Object, res::JsonObject
        {
            JsonClass(
                Global,
                m_Tag
            )

            inline bool operator ==(const Global& other) const
            {
                return m_Tag == other.GetTag();
            }
        };

    }

}