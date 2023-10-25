#pragma once

namespace xpe {

    namespace ui {

        using namespace res;
        using namespace math;

        struct ENGINE_API UiObject : public Object, XmlObject
        {
            math::Transform Transform;
        };

    }

}