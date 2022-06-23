#pragma once

#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <functional>

#include "Edge.h"

template<typename T_Key>
class Node {
    template<typename T> friend
    class SuffixTree;

    using T_Element = typename T_Key::value_type;
private:
    static const int START_SIZE = 0;

    std::vector<int> data;

    std::function<std::size_t(const T_Element&)> map_to_idx;
    std::vector <std::shared_ptr<Edge<T_Key>>> edges;

    /// Use weak_ptr instead of shared_ptr to avoid shared_ptr cyclic dependency.
    std::weak_ptr<Node> suffix;
    int result_count;

    bool contains(int idx) const {
        int low = 0, high = (int)data.size() - 1;

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
        for (auto &e: edges) {
            if (e) {
                for (auto &num: e->dest()->compute_and_cache_count_recursive()) {
                    set.insert(num);
                }
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
    Node() : suffix{std::shared_ptr<Node>(nullptr)}, result_count{-1} {
        map_to_idx = [](const T_Element &e) { return (std::size_t)e; };
        data.resize(START_SIZE);
        edges.clear();
    }
    explicit Node(std::function<std::size_t(const T_Element&)> map_func) : suffix{nullptr}, result_count{-1} {
        map_to_idx = map_func;
        data.resize(START_SIZE);
        edges.clear();
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

        for (auto &e: edges) {
            if (e && (count < 0 || set.size() < count)) {
                for (auto &num: e->dest()->get_data(count - (int) set.size())) {
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
        for (auto iter = this->suffix.lock(); iter != nullptr && !iter->contains(idx); iter = iter->suffix.lock()) {
            iter->add_ref(idx);
        }
    }

    int get_result_count() const {
        if (this->result_count == -1) {
            throw std::runtime_error("get_result_count() shouldn't be called without calling compute_count() first");
        }

        return this->result_count;
    }

    void add_edge(const T_Element &c, std::shared_ptr<Edge<T_Key>> e) {
        auto idx = map_to_idx(c);
        if (idx >= edges.size()) edges.resize(idx + 1, nullptr);
        edges[idx] = e;
    }

    std::shared_ptr<Edge<T_Key> const> get_edge(const T_Element &c) const {
        auto idx = map_to_idx(c);
        auto re = edges.size() > idx ? edges.at(idx) : nullptr;

        return re;
    }

    std::shared_ptr<Edge<T_Key>> get_edge(const T_Element &c) {
        auto idx = map_to_idx(c);
        auto re = edges.size() > idx ? edges.at(idx) : nullptr;

        return re;
    }

    std::shared_ptr<Node const> get_suffix() const { return this->suffix.lock(); }

    std::shared_ptr<Node> get_suffix() { return this->suffix.lock(); }

    void set_suffix(std::shared_ptr<Node> const suffix) { this->suffix = suffix; }
};