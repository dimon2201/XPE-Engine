#pragma once

namespace xpe {

    namespace math {

        using namespace core;

        struct ENGINE_API sTransform : public cObject, public sModelMatrix {};
        Json(sTransform, Position, Rotation, Scale)
        Xml(sTransform, Position, Rotation, Scale)

    }

}