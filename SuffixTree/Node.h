#pragma once

#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <functional>

#include "Edge.h"
#include "HashTable.h"

template<typename T_Key>
class Node {
    template<typename T> friend
    class SuffixTree;

    using T_Element = typename T_Key::value_type;
private:
    using Table = HashTable <T_Element, std::shared_ptr<Edge<T_Key>>>;
    static const int DATA_START_SIZE = 0;
    static const int DEFAULT_EDGES_SIZE = 53;

    std::vector<int> data;

    Table edges;

    /// Use weak_ptr instead of shared_ptr to avoid shared_ptr cyclic dependency.
    std::weak_ptr<Node> suffix;
    int result_count;

    [[nodiscard]] bool contains(int idx) const {
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

        auto vec = edges.get_all();
        for (auto &[_, e]: vec) {
            if (e) {
                for (auto &num: e->dest()->compute_and_cache_count_recursive()) {
                    set.insert(num);
                }
            }
        }
        result_count = (int)set.size();

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
        edges = Table(DEFAULT_EDGES_SIZE,
                      [](const T_Element &e, std::size_t size) { return e % size; });
        data.resize(DATA_START_SIZE);
    }
    explicit Node(std::function<std::size_t(const T_Element&, std::size_t)> hash) : suffix{nullptr}, result_count{-1} {
        edges = Table(DEFAULT_EDGES_SIZE, hash);
        data.resize(DATA_START_SIZE);
    }
    Node(std::size_t edges_size, std::function<std::size_t(const T_Element&, std::size_t)> hash) : suffix{nullptr}, result_count{-1} {
        edges = Table(edges_size, hash);
        data.resize(DATA_START_SIZE);
    }

    [[nodiscard]] std::set<int> get_data() const {
        return get_data(-1);
    }

    [[nodiscard]] std::set<int> get_data(int count) const {
        std::set<int> set;

        for (auto &num: data) {
            set.insert(num);
            if (set.size() == count)

                break;
        }

        auto vec = edges.get_all();
        for (auto &[_, e]: vec) {
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

    [[nodiscard]] int get_result_count() const {
        if (this->result_count == -1) {
            throw std::runtime_error("get_result_count() shouldn't be called without calling compute_count() first");
        }

        return this->result_count;
    }

    void add_edge(const T_Element &c, std::shared_ptr<Edge<T_Key>> e) { edges[c] = e; }

    std::shared_ptr<Edge<T_Key> const> get_edge(const T_Element &c) const {
        return edges.contains(c) ? edges.at(c) : nullptr;
    }

    std::shared_ptr<Edge<T_Key>> get_edge(const T_Element &c) {
        return edges.contains(c) ? edges.at(c) : nullptr;
    }

    std::shared_ptr<Node const> get_suffix() const { return this->suffix.lock(); }

    std::shared_ptr<Node> get_suffix() { return this->suffix.lock(); }

    void set_suffix(std::shared_ptr<Node> const suffix) { this->suffix = suffix; }
};