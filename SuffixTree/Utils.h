#pragma once

#include <string>

namespace Utils {
    template<typename T_Key>
    KeyInternal<T_Key> safe_cut_last_char(const KeyInternal<T_Key> &s) {
        if (s.empty())
            return s;
        return s.substr(0, s.size() - 1);
    }

//    std::string normalize(const std::string in) {
//        std::string out = in;
//        for(auto &c : out)
//            if (c >= 'A' && c <= 'Z') c -= ('A' - 'a');
//
//        return out;
//    }
}