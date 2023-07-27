#pragma once

namespace xpe {

    namespace core {

        class Game;

        class ENGINE_API GameBuilder final {

        public:
            static Game* Build();
            static void Free();

        };

    }

}