#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>

#include "SuffixTree/SuffixTree.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

void test_speed() {
    srand(time(nullptr));
    int sz = 80000;
    int max_len = 100;
    int test = 30;
    std::cout << "Remember to set to release.\nConfiguration: " << sz << " strings, " << max_len
              << " chars max. 26 lowercase letters.\n";

    double avg = 0, avg_cnt = 0;
    for (int t = 1; t <= test; t++) {
        std::cout << "TIME " << t << '\n';

        SuffixTree<std::string, int> tree;

        std::vector<std::string> words;
        int cnt = 0;
        for (int i = 0; i < sz; i++) {
            int len = rand() % max_len + 1;
            words.emplace_back();
            for (int j = 0; j < len; j++) {
                char c = rand() % 26 + 'a';
                words.back() += c;
            }

            cnt += len;
        }

        std::cout << cnt << '\n';
        avg_cnt += cnt;

        double avg_f = 0, avg_f_cnt = 0;
        auto t1 = high_resolution_clock::now();

        for (int idx = 0; idx < sz; idx++) {
            auto &s = words[idx];

            auto t1 = high_resolution_clock::now();
            tree.put(s, idx);
            auto t2 = high_resolution_clock::now();

            avg_f_cnt += s.size();

            //tree.search(s);

            duration<double, std::milli> ms_double = t2 - t1;
            avg_f += ms_double.count();
        }

        auto t2 = high_resolution_clock::now();
        /* Getting number of milliseconds as a double. */
        duration<double, std::milli> ms_double = t2 - t1;
        std::cout << ms_double.count() << "ms\n";
        avg += ms_double.count();

        avg_f /= (double) sz;
        avg_f_cnt /= (double) sz;
        std::cout << avg_f << "ms " << (avg_f * sz) << "ms " << avg_f_cnt << '\n';
    }

    avg /= (double) test;
    avg_cnt /= (double) test;
    std::cout << "AVG " << avg << "ms " << avg_cnt << "\n";
}

void test_correctness() {
    srand(time(nullptr));
    int sz = 100;
    int max_len = 100;
    int test = 20;
    std::cout << "Remember to set to debug.\nConfiguration: " << sz << " strings, " << max_len
              << " chars max. 26 lowercase letters.\n";

    for (int t = 1; t <= test; t++) {
        std::cout << "TEST " << t << '\n';

        SuffixTree<std::string, int> tree;

        std::vector<std::string> words;
        int cnt = 0;
        for (int i = 0; i < sz; i++) {
            int len = rand() % max_len + 1;
            words.emplace_back();
            for (int j = 0; j < len; j++) {
                char c = rand() % 26 + 'a';
                words.back() += c;
            }

            cnt += len;
        }

        std::cout << cnt << '\n';

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
    }
}

void test_correctness_vec() {
    srand(time(nullptr));
    int sz = 100;
    int max_len = 100;
    std::cout << "Remember to set to debug.\nConfiguration: " << sz << " strings, " << max_len
              << " chars max. 26 lowercase letters.\n";

    int test = 20;

    for (int t = 1; t <= test; t++) {
        std::cout << "TEST " << t << '\n';

        SuffixTree<std::vector<int>, int> tree;

        std::vector<std::vector<int>> words;
        int cnt = 0;
        for (int i = 0; i < sz; i++) {
            int len = rand() % max_len + 1;
            words.emplace_back();
            for (int j = 0; j < len; j++) {
                int c = rand() % 200;
                words.back().push_back(c);
            }

            cnt += len;
        }

        std::cout << cnt << '\n';

        for (int idx = 0; idx < sz; idx++) {
            auto &s = words[idx];
            tree.put(s, idx);

            for (int i = 0; i < s.size(); i++)
                for (int j = i + 1; j <= s.size(); j++) {
                    auto set = tree.search({s.begin() + i, s.begin() + j});
                    assert(set.find(idx) != set.end());
                }
        }

        for (int idx = 0; idx < sz; idx++) {
            auto &s = words[idx];
            for (int i = 0; i < s.size(); i++)
                for (int j = i + 1; j <= s.size(); j++) {
                    auto set = tree.search({s.begin() + i, s.begin() + j});
                    assert(set.find(idx) != set.end());
                }
        }

        for (int idx = 0; idx < sz; idx++) {
            auto &s = words[idx];
            tree.put(s, idx + sz);

            for (int i = 0; i < s.size(); i++)
                for (int j = i + 1; j <= s.size(); j++) {
                    auto set = tree.search({s.begin() + i, s.begin() + j});
                    assert(set.find(idx) != set.end());
                    assert(set.find(idx + sz) != set.end());
                }
        }
    }
}

int main() {
    //test_speed();
    test_correctness();
    test_correctness_vec();

    SuffixTree<std::string, int> tree;
    std::string words[] = {"qwe", "rtyr", "uio", "pas", "dfg", "hjk", "lzx", "cvb", "bnm"};
    int sz = 8;

    for (int idx = 0; idx < sz; idx++) {
        auto &s = words[idx];
        tree.put(s, idx);

        for (int i = 0; i < s.size(); i++)
            for (int j = 1; j <= s.size() - i; j++) {
                auto set = tree.search(s.substr(i, j));
                assert(set.find(idx) != set.end());
            }

        for (int i = 0; i < sz; i++) {
            if (i == idx) continue;
            auto &s1 = words[i];

            for (int i = 0; i < s1.size(); i++)
                for (int j = 1; j <= s1.size() - i; j++) {
                    auto set = tree.search(s1.substr(i, j));
                    assert(set.find(idx) == set.end());
                }
        }
    }
}
