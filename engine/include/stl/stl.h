#pragma once

namespace xpe {

    namespace core {

        typedef std::basic_string<char, std::char_traits<char>, cMainAllocator<char>> string;
        typedef std::basic_string<char, std::char_traits<char>, cHotAllocator<char>> hstring;

        ENGINE_API u64 Hash(const string& str);
        ENGINE_API u64 Hash(const hstring& str);

        typedef std::basic_stringstream<char, std::char_traits<char>, cMainAllocator<char>> stringstream;
        typedef std::basic_stringstream<char, std::char_traits<char>, cHotAllocator<char>> hstringstream;

        template<typename T>
        using vector = std::vector<T, cMainAllocator<T>>;

        template<typename T>
        using hvector = std::vector<T, cHotAllocator<T>>;

        template<typename T, size_t Size>
        using array = std::array<T, Size>;

        template<typename K, typename V>
        using map = std::map<K, V, std::equal_to<K>, cMainAllocator<std::pair<K, V>>>;

        template<typename K, typename V>
        using hmap = std::map<K, V, std::equal_to<K>, cHotAllocator<std::pair<K, V>>>;

        template<typename K, typename Comparator = std::less<K>>
        using set = std::set<K, Comparator, cMainAllocator<K>>;

        template<typename K, typename Comparator = std::less<K>>
        using hset = std::set<K, Comparator, cHotAllocator<K>>;

        template<typename K>
        using unordered_set = std::unordered_set<K, std::hash<K>, std::equal_to<K>, cMainAllocator<K>>;

        template<typename K>
        using hunordered_set = std::unordered_set<K, std::hash<K>, std::equal_to<K>, cHotAllocator<K>>;

        template<typename K, typename V>
        using unordered_map = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, cMainAllocator<std::pair<K, V>>>;

        template<typename K, typename V>
        using hunordered_map = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, cHotAllocator<std::pair<K, V>>>;

    }

}