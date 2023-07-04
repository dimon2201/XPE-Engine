#pragma once

namespace xpe {

    namespace core {

        template<typename T>
        using vector = std::vector<T, Allocator<T>>;

    }

}