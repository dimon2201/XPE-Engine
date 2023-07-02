#pragma once

namespace xpe {

    namespace core {

        enum GraphicsAPI {
            DX11 = 0,
            OPENGL = 1,
            VULKAN = 2
        };

        struct ENGINE_API EngineConfig final {
            static GraphicsAPI GPU_API;
        };

    }

}