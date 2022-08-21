#pragma once

#include <vector>
#include <stdexcept>
#include <functional>
#include <map>
#include <set>

#include "Edge.h"

template<typename T_Key, typename T_Mapped>
class Node {
    template<typename T1, typename T2> friend
    class SuffixTree;

public:
    using key_type = T_Key;
    using mapped_type = T_Mapped;

private:
    using element_type = typename key_type::value_type;
    using edge_type = Edge<key_type, mapped_type>;

    Node *suffix_;

    std::set<mapped_type> data_;
    std::map<element_type, edge_type *> edges_;

    void get_data(std::set<mapped_type> &set, int count) const {
        for (auto &num: data_) {
            set.insert(num);
            if (set.size() == count)

                break;
        }

        if (set.size() != count) {
            for (auto &[_, e]: edges_) {
                e->dest()->get_data(set, count);
                if (set.size() == count)

                    break;
            }
        }
    }

    bool add_index(mapped_type idx) {
        if (data_.find(idx) != data_.end())
            return false;

        data_.insert(idx);
        return true;
    }

public:
    Node() : suffix_{nullptr} {
        edges_.clear();
    }

    [[nodiscard]] std::set<mapped_type> get_data() const {
        return get_data(-1);
    }

    [[nodiscard]] std::set<mapped_type> get_data(int count) const {
        std::set<mapped_type> set;
        get_data(set, count);

        return set;
    }

    bool add_ref(mapped_type idx) {
        if (!add_index(idx))
            return false;

        for (auto iter = this->suffix_; iter != nullptr && iter->add_ref(idx); iter = iter->suffix_);

        return true;
    }

    void add_edge(const element_type &c, edge_type *e) { edges_[c] = e; }

    edge_type const *get_edge(const element_type &c) const {
        return edges_.contains(c) ? edges_.at(c) : nullptr;
    }

    edge_type *get_edge(const element_type &c) {
        return edges_.find(c) != edges_.end() ? edges_.at(c) : nullptr;
    }

    Node const *get_suffix() const { return this->suffix_; }

    Node *get_suffix() { return this->suffix_; }

    void set_suffix(Node *suffix) { this->suffix_ = suffix; }
};