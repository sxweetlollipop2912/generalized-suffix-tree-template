#pragma once

#include <vector>
#include <map>
#include <set>
#include <stdexcept>

#include "Edge.h"

template<typename T_Key>
class Node {
    template<typename T> friend
    class SuffixTree;

private:
    static const int START_SIZE = 0;

    std::vector<int> data;
    std::map<char, Edge<T_Key>> edges;
    Node *suffix;
    int result_count;

    bool contains(int idx) const {
        int low = 0, high = data.size() - 1;

        while (low < high) {
            int mid = (low + high) >> 1;

            if (idx > data[mid])
                low = mid + 1;
            else
                high = mid;
        }

        return low == high && data[low] == idx;
    }

    std::set<int> compute_and_cache_count_recursive() {
        std::set<int> set;

        for (auto &num: data) { set.insert(num); }
        for (auto &[_, e]: edges) {
            for (auto &num: e.dest()->compute_and_cache_count_recursive()) {
                set.insert(num);
            }
        }

        result_count = set.size();

        return set;
    }

    void add_index(int idx) { data.emplace_back(idx); }

protected:
    int compute_and_cache_count() {
        compute_and_cache_count_recursive();

        return this->result_count;
    }

public:
    Node() : suffix{nullptr}, result_count{-1} {
        edges = std::map<char, Edge<T_Key>>();
        data.resize(START_SIZE);
    }

    std::set<int> get_data() const {
        return get_data(-1);
    }

    std::set<int> get_data(int count) const {
        std::set<int> set;

        for (auto &num: data) {
            set.insert(num);
            if (set.size() == count)

                break;
        }

        for (auto &[_, e]: edges) {
            if (count < 0 || set.size() < count) {
                for (auto &num: e.dest()->get_data(count - (int) set.size())) {
                    set.insert(num);
                    if (set.size() == count)

                        break;
                }
            }
        }

        return set;
    }

    void add_ref(int idx) {
        if (contains(idx))
            return;

        add_index(idx);
        for (auto iter = this->suffix; iter != nullptr && !iter->contains(idx); iter = iter->suffix) {
            iter->add_ref(idx);
        }
    }

    int get_result_count() const {
        if (this->result_count == -1) {
            throw std::runtime_error("get_result_count() shouldn't be called without calling compute_count() first");
        }

        return this->result_count;
    }

    void add_edge(const char c, const Edge<T_Key> &e) { edges[c] = e; }

    Edge<T_Key> const *get_edge(const char c) const {
        return edges.find(c) != edges.end() ? &edges.at(c) : nullptr;
    }

    Edge<T_Key> *get_edge(const char c) {
        return edges.find(c) != edges.end() ? &edges.at(c) : nullptr;
    }

    std::map<char, Edge<T_Key>> get_edges() const { return edges; }

    Node const *get_suffix() const { return this->suffix; }

    Node *get_suffix() { return this->suffix; }

    void set_suffix(Node *const suffix) { this->suffix = suffix; }
};