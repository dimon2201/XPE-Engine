#pragma once

namespace xpe {

    namespace core {

        template<typename K, typename V>
        using map = std::map<K, V, std::equal_to<K>, MainAllocator<std::pair<K, V>>>;

        template<typename K, typename V>
        using hmap = std::map<K, V, std::equal_to<K>, HotAllocator<std::pair<K, V>>>;

    }

}