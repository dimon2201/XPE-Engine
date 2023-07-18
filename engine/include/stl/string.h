#pragma once

namespace xpe {

    namespace core {

        typedef std::basic_string<char, std::char_traits<char>, MainAllocator<char>> string;
        typedef std::basic_string<char, std::char_traits<char>, HotAllocator<char>> hstring;

    }

}