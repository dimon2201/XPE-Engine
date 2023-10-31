#pragma once

#include <core/allocators.h>

namespace xpe {

    namespace core {

        typedef std::basic_string<char, std::char_traits<char>, cMainAllocator<char>> string;
        typedef std::basic_string<char, std::char_traits<char>, cHotAllocator<char>> hstring;

        ENGINE_API u64 Hash(const string& str);
        ENGINE_API u64 Hash(const hstring& str);

    }

}