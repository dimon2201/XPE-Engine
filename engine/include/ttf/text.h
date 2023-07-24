#pragma once

#include <rendering/buffers/structure_buffer.h>
#include <core/ecs.hpp>
#include <ttf/ttf_manager.hpp>
#include <rendering/buffers/instance_buffer.h>

namespace xpe {

    namespace ttf {

        struct ENGINE_API Text {
            Text() {}
            ~Text() {}

            xpe::core::string Chars;
            xpe::render::TransformComponent* Transform;
            Font* TextFont;
        };

    }

}