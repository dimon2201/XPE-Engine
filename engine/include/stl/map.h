#pragma once

namespace xpe {

    namespace core {

        template<typename K, typename V>
        using map = std::map<K, V, std::equal_to<K>, cMainAllocator<std::pair<K, V>>>;

        template<typename K, typename V>
        using hmap = std::map<K, V, std::equal_to<K>, cHotAllocator<std::pair<K, V>>>;

    }

}