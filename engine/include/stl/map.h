#pragma once

namespace xpe {

    namespace core {

        template<typename K, typename V>
        using map = std::map<K, V, std::equal_to<K>, Allocator<std::pair<K, V>>>;

    }

}