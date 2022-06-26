#pragma once

#include "AVLTree.h"

template<typename T_Key, typename T_Mapped>
class Map {
public:
    using key_type = T_Key;
    using mapped_type = T_Mapped;
    using value_type = std::pair<key_type, mapped_type>;
    using size_type = std::size_t;

private:
    using container_type = AVLTree<value_type>;

    container_type map_;

public:
    Map() {
        map_ = AVLTree<value_type>([](const value_type &x1, const value_type &x2) {
            return x1.first < x2.first;
        });
    }

    explicit Map(const std::function<size_type(const key_type &, const key_type &)> &comp) {
        map_ = AVLTree<value_type>([&](const value_type &x1, const value_type &x2) {
            return comp(x1.first, x2.first);
        });
    }

    std::vector<value_type> to_vec() const { return map_.to_vec(); }

    bool contains(const key_type &key) const { return map_.contains(value_type(key, {})); }

    [[nodiscard]] size_type size() const { return map_.size(); }

    /// Allows modifications, i.e. `table.at(0) = sth;`\n
    /// Exception(s): key not found
    mapped_type &at(const key_type &key) {
        return map_.find(map_.root_, value_type(key, {}))->data.second;
    }

    /// Does not allow modifications.\n
    /// Exception(s): key not found
    mapped_type const &at(const key_type &key) const {
        return map_.find(map_.root_, value_type(key, {}))->data.second;
    }

    /// Allows modifications, i.e. `table[0] = sth;`\n
    /// Creates a new slot if table doesn't contain the key.\n
    mapped_type &operator[](const key_type &key) {
        map_.insert(value_type(key, {}));

        return at(key);
    }

    /// Does not allow modifications.`\n
    /// Exception(s): key not found
    mapped_type const &operator[](const key_type &key) const {
        return at(key);
    }

    bool erase(const key_type &key) { return map_.erase(value_type(key, {})); }

    void clear() { map_.clear(); }
};