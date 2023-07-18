#pragma once

namespace xpe {

    namespace core {

        typedef std::basic_stringstream<char, std::char_traits<char>, MainAllocator<char>> stringstream;
        typedef std::basic_stringstream<char, std::char_traits<char>, HotAllocator<char>> hstringstream;

    }

}