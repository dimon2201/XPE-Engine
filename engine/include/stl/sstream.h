#pragma once

namespace xpe {

    namespace core {

        typedef std::basic_stringstream<char, std::char_traits<char>, cMainAllocator<char>> stringstream;
        typedef std::basic_stringstream<char, std::char_traits<char>, cHotAllocator<char>> hstringstream;

    }

}