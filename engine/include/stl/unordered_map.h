#pragma once

namespace xpe {

    namespace core {

        template<typename K, typename V>
        using unordered_map = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, cMainAllocator<std::pair<K, V>>>;

        template<typename K, typename V>
        using hunordered_map = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, cHotAllocator<std::pair<K, V>>>;

    }

}