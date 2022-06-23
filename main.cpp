#include <iostream>
#include <cstdlib>
#include <ctime>

#include "SuffixTree/SuffixTree.h"

void test_speed() {
    srand(time(nullptr));
    int sz = 1000;
    int max_len = 2000;
    std::cout << "Configuration: " << sz << " strings, " << max_len << " chars max. 26 lowercase letters.\n";
    std::cout << "Generating strings. Counting starts after char count is printed.\n";

    SuffixTree<std::string> tree;

    std::vector <std::string> words;
    int cnt = 0;
    for(int i = 0; i < sz; i++) {
        int len = rand() % max_len + 1;
        words.emplace_back();
        for(int j = 0; j < len; j++) {
            char c = rand() % 26 + 'a';
            words.back() += c;
        }

        cnt += len;

        //std::cout << i + 1 << ' ' << words[i] << '\n';
    }

    std::cout << cnt;

    for (int idx = 0; idx < sz; idx++) {
        auto &s = words[idx];
        tree.put(s, idx);

        auto set = tree.search(s);
        assert(set.find(idx) != set.end());
    }
}

void test_correctness() {
    SuffixTree<std::string> tree;

    std::string words[] = {"libertypike",
                           "franklintn",
                           "carothersjohnhenryhouse",
                           "carothersezealhouse",
                           "acrossthetauntonriverfromdightonindightonrockstatepark",
                           "dightonma",
                           "dightonrock",
                           "6mineoflowgaponlowgapfork",
                           "lowgapky",
                           "lemasterjohnjandellenhouse",
                           "lemasterhouse",
                           "70wilburblvd",
                           "poughkeepsieny",
                           "freerhouse",
                           "701laurelst",
                           "conwaysc",
                           "hollidayjwjrhouse",
                           "mainandappletonsts",
                           "menomoneefallswi",
                           "mainstreethistoricdistrict",
                           "addressrestricted",
                           "brownsmillsnj",
                           "hanoverfurnace",
                           "hanoverbogironfurnace",
                           "sofsavannahatfergusonaveandbethesdard",
                           "savannahga",
                           "bethesdahomeforboys",
                           "bethesda"};
    int sz = 28;
    for (int idx = 0; idx < sz; idx++) {
        auto &s = words[idx];
        tree.put(s, idx);

        for (int i = 0; i < s.size(); i++)
            for (int j = 1; j <= s.size() - i; j++) {
                auto set = tree.search(s.substr(i, j));
                assert(set.find(idx) != set.end());
            }
    }

    for (int idx = 0; idx < sz; idx++) {
        auto &s = words[idx];
        for (int i = 0; i < s.size(); i++)
            for (int j = 1; j <= s.size() - i; j++) {
                auto set = tree.search(s.substr(i, j));
                assert(set.find(idx) != set.end());
            }
    }

    for (int idx = 0; idx < sz; idx++) {
        auto &s = words[idx];
        tree.put(s, idx + sz);

        for (int i = 0; i < s.size(); i++)
            for (int j = 1; j <= s.size() - i; j++) {
                auto set = tree.search(s.substr(i, j));
                assert(set.find(idx) != set.end());
                assert(set.find(idx + sz) != set.end());
            }
    }

    auto set = tree.search("ypikefra");
    assert(set.empty());
}

int main() {
    //test_speed();
    test_correctness();
}
