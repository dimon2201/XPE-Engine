#pragma once

namespace xpe {

    namespace core {

        template<typename K, typename V>
        using unordered_map = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, MainAllocator<std::pair<K, V>>>;

    }

}