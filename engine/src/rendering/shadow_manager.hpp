#pragma once

namespace xpe {

    namespace render {

        class ENGINE_API cShadowManager final
        {
        public:
            static void Init();
            static void Free();
            static void Flush();
        };

    }

}