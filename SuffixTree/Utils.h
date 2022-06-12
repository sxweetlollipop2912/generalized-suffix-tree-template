#pragma once

#include <string>

namespace Utils {
    std::string safe_cut_last_char(const std::string &s) {
        return s.empty() ? "" : s.substr(0, s.size() - 1);
    }

//    std::string normalize(const std::string in) {
//        std::string out = in;
//        for(auto &c : out)
//            if (c >= 'A' && c <= 'Z') c -= ('A' - 'a');
//
//        return out;
//    }
}