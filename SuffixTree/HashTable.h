#pragma once

#include <vector>
#include <stdexcept>

template<typename T>
class Option {
private:
    T value_;
    bool some_;

public:
    Option() : value_{}, some_{false} {}

    explicit Option(T value) : value_{value}, some_{true} {}

    [[nodiscard]] bool is_some() const { return some_; }

    [[nodiscard]] bool is_none() const { return !is_some(); }

    T &unwrap() { return value_; }

    T const &peek() const { return value_; }
};

template<typename T_Key, typename T_Mapped>
class HashTable {
public:
    using key_type = T_Key;
    using mapped_type = T_Mapped;
    using value_type = std::pair<key_type, mapped_type>;
    using size_type = std::size_t;

private:
    using container_type = std::vector<Option<value_type>>;

    container_type table_;
    size_type size_;
    std::function<size_type(const key_type &, size_type, const std::function<bool(size_type)> &)> hash_;

    typename container_type::const_iterator iter_at(const key_type &key) const {
        auto idx = hash(key, [&](size_type idx) {
            return table_[idx].is_none() || table_[idx].peek().first == key;
        });

        return table_.begin() + idx;
    }

    typename container_type::iterator iter_at(const key_type &key) {
        auto idx = hash(key, [&](size_type idx) {
            return table_[idx].is_none() || table_[idx].peek().first == key;
        });

        return table_.begin() + idx;
    }

    typename container_type::iterator
    insert(const key_type &key, const mapped_type &mapped_value) {
        auto it = iter_at(key);
        if (it->is_none())
            *it = Option(value_type(key, mapped_value));

        return it;
    }

public:
    HashTable() : size_{0} {
        hash_ = [](const key_type &, size_type, const std::function<bool(size_type)> &) { return 0; };
    }

    HashTable(size_type size,
              std::function<std::size_t(const key_type &, size_type, const std::function<bool(size_type)> &)> hash)
            : size_{size} {
        table_.resize(size_);
        hash_ = hash;
    }

    size_type hash(const key_type &key) const {
        return hash_(key, size_, [&](size_type idx) { return table_[idx].is_none(); });
    }

    size_type hash(const key_type &key, const std::function<bool(size_type)> &predicate) const {
        return hash_(key, size_, predicate);
    }

    std::vector<std::pair<key_type const &, mapped_type &>> get_all() {
        typedef std::pair<key_type const &, mapped_type &> return_value_type;

        std::vector<return_value_type> vec;
        for (auto &p: table_) {
            if (p.is_some()) {
                vec.push_back(return_value_type(p.peek().first, p.peek().second));
            }
        }

        return vec;
    }

    std::vector<std::pair<key_type const &, mapped_type const &>> get_all() const {
        typedef std::pair<key_type const &, mapped_type const &> return_value_type;

        std::vector<return_value_type> vec;
        for (auto &p: table_) {
            if (p.is_some()) {
                vec.push_back(return_value_type(p.peek().first, p.peek().second));
            }
        }

        return vec;
    }

    bool contains(const key_type &key) const { return iter_at(key)->is_some(); }

    [[nodiscard]] size_type size() const { return size_; }

    /// Allows modifications, i.e. `table.at(0) = sth;`\n
    /// Exception(s): key not found
    mapped_type &at(const key_type &key) {
        auto it = iter_at(key);
        if (it->is_none()) {
            throw std::out_of_range("Key not found.");
        }

        return it->unwrap().second;
    }

    /// Does not allow modifications.\n
    /// Exception(s): key not found
    mapped_type const &at(const key_type &key) const {
        auto it = iter_at(key);
        if (it->is_none()) {
            throw std::out_of_range("Key not found.");
        }

        return it->unwrap().second;
    }

    /// Allows modifications, i.e. `table[0] = sth;`\n
    /// Creates a new slot if table doesn't contain the key.\n
    /// Exception(s): table is full
    mapped_type &operator[](const key_type &key) {
        auto it = insert(key, {});

        return it->unwrap().second;
    }

    void clear() { std::fill(table_.begin(), table_.end(), {}); }
};