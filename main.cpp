#include <iostream>

#include "SuffixTree/Edge.h"
#include "SuffixTree/Node.h"
#include "SuffixTree/SuffixTree.h"

int main() {
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
