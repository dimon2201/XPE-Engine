#pragma once

namespace xpe {

    namespace math {

        using namespace core;

        struct ENGINE_API sTransform : public sModelMatrix {};
        JSON(sTransform, Position, Rotation, Scale)
        XML(sTransform, Position, Rotation, Scale)

    }

}