#pragma once

namespace xpe {

    namespace core {

        template<typename T>
        using vector = std::vector<T, MainAllocator<T>>;

        template<typename T>
        using hvector = std::vector<T, HotAllocator<T>>;

    }

}