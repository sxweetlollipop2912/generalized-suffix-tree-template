#pragma once

#include "AVLTree.h"

template<typename T_Key>
class Set {
public:
    using value_type = T_Key;
    using size_type = std::size_t;

private:
    using container_type = AVLTree<value_type>;

    container_type set_;

public:
    Set() {
        set_ = AVLTree<value_type>([](const value_type &x1, const value_type &x2) {
            return x1 < x2;
        });
    }

    explicit Set(const std::function<size_type(const value_type &, const value_type &)> &comp) {
        set_ = AVLTree<value_type>(comp);
    }

    std::vector<value_type> to_vec() const { return set_.to_vec(); }

    bool contains(const value_type &key) const { return set_.contains(key); }

    [[nodiscard]] size_type size() const { return set_.size(); }

    void insert(const value_type &key) { set_.insert(key); }

    bool erase(const value_type &key) { return set_.erase(key); }

    void clear() { set_.clear(); }
};