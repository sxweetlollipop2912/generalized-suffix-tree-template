#pragma once

#include <vector>
#include <stdexcept>
#include <functional>

#include "Edge.h"
#include "Map.h"
#include "Set.h"

template<typename T_Key>
class SuffixNode {
    template<typename T> friend
    class SuffixTree;

    using T_Element = typename T_Key::value_type;
private:
    using Map = Map<T_Element, Edge<T_Key> *>;
    static const int DATA_START_SIZE = 0;

    std::vector<int> data_;
    Map edges_;
    SuffixNode *suffix_;
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

    Set<int> compute_and_cache_count_recursive() {
        Set<int> set;
        for (auto &num: data_) { set.insert(num); }

        auto vec = edges_.to_vec();
        for (auto &[_, e]: vec) {
            if (e) {
                for (auto &num: e->dest()->compute_and_cache_count_recursive()) {
                    set.insert(num);
                }
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
    SuffixNode() : suffix_{nullptr}, result_count_{-1} {
        data_.resize(DATA_START_SIZE);
    }

    explicit SuffixNode(const std::function<typename Map::size_type(const T_Element &, const T_Element &)> &comp)
            : suffix_{nullptr}, result_count_{-1} {
        edges_ = Map(comp);
        data_.resize(DATA_START_SIZE);
    }

    [[nodiscard]] Set<int> get_data() const {
        return get_data(-1);
    }

    [[nodiscard]] Set<int> get_data(int count) const {
        Set<int> set;

        for (auto &num: data_) {
            set.insert(num);
            if (set.size() == count)

                break;
        }

        if (count < 0 || set.size() < count) {
            auto vec = edges_.to_vec();
            for (auto &[_, e]: vec) {
                auto vec2 = e->dest()->get_data(count - (int) set.size()).to_vec();
                for (auto &num: vec2) {
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
        return edges_.contains(c) ? edges_.at(c) : nullptr;
    }

    std::vector<typename Map::value_type const> get_edges() const {
        return edges_.to_vec();
    }

    std::vector<typename Map::value_type> get_edges() {
        return edges_.to_vec();
    }

    SuffixNode const *get_suffix() const { return this->suffix_; }

    SuffixNode *get_suffix() { return this->suffix_; }

    void set_suffix(SuffixNode *suffix) { this->suffix_ = suffix; }
};