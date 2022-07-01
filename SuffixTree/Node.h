#pragma once

#include <vector>
#include <stdexcept>
#include <functional>
#include <unordered_map>
#include <set>

#include "Edge.h"

template<typename T_Key>
class Node {
    template<typename T> friend
    class SuffixTree;

    using T_Element = typename T_Key::value_type;
private:
    static const int DATA_START_SIZE = 0;

    std::vector<int> data_;
    std::unordered_map<T_Element, Edge<T_Key> *> edges_;
    Node *suffix_;
    int result_count_;

    [[nodiscard]] bool contains(int idx) const {
        int low = 0, high = (int) data_.size() - 1;
        while (low < high) {
            int mid = (low + high) >> 1;
            if (idx > data_[mid])
                low = mid + 1;
            else
                high = mid;
        }

        return low == high && data_[low] == idx;
    }

    std::set<int> compute_and_cache_count_recursive() {
        std::set<int> set;
        for (auto &num: data_) { set.insert(num); }

        for (auto &[_, e]: edges_) {
            if (e) {
                auto set_e = e->dest()->compute_and_cache_count_recursive();
                set.insert(set_e);
            }
        }
        result_count_ = (int) set.size();

        return set;
    }

    void add_index(int idx) { data_.emplace_back(idx); }

protected:
    int compute_and_cache_count() {
        compute_and_cache_count_recursive();

        return this->result_count_;
    }

public:
    Node() : suffix_{nullptr}, result_count_{-1} {
        edges_.clear();
        data_.resize(DATA_START_SIZE);
    }

    [[nodiscard]] std::set<int> get_data() const {
        return get_data(-1);
    }

    [[nodiscard]] std::set<int> get_data(int count) const {
        std::set<int> set;

        for (auto &num: data_) {
            set.insert(num);
            if (set.size() == count)

                break;
        }

        if (count < 0 || set.size() < count) {
            for (auto &[_, e]: edges_) {
                auto set_e = e->dest()->get_data(count - (int) set.size());
                for (auto &num: set_e) {
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
        for (auto iter = this->suffix_; iter != nullptr && !iter->contains(idx); iter = iter->suffix_) {
            iter->add_ref(idx);
        }
    }

    [[nodiscard]] int get_result_count() const {
        if (this->result_count_ == -1) {
            throw std::runtime_error("get_result_count() shouldn't be called without calling compute_count() first");
        }

        return this->result_count_;
    }

    void add_edge(const T_Element &c, Edge<T_Key> *e) { edges_[c] = e; }

    Edge<T_Key> const *get_edge(const T_Element &c) const {
        return edges_.contains(c) ? edges_.at(c) : nullptr;
    }

    Edge<T_Key> *get_edge(const T_Element &c) {
        return edges_.find(c) != edges_.end() ? edges_.at(c) : nullptr;
    }

    Node const *get_suffix() const { return this->suffix_; }

    Node *get_suffix() { return this->suffix_; }

    void set_suffix(Node *suffix) { this->suffix_ = suffix; }
};