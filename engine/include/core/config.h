#pragma once

namespace xpe {

    namespace core {

        enum eGPU_API {
            DX11 = 0,
            OPENGL = 1,
            VULKAN = 2
        };

        struct ENGINE_API EngineConfig final {
            static eGPU_API GPU_API;
        };

    }

}