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
    T& unwrap() { return value_; }
    T const& peek() const { return value_; }
};

template<typename T_Key, typename T_Mapped>
class HashTable {
public:
    using key_type = T_Key;
    using mapped_type = T_Mapped;
    using value_type = std::pair <key_type, mapped_type>;
    using size_type = std::size_t;

private:
    using container_type = std::vector <Option<value_type>>;

    container_type table_;
    size_type size_, left_;
    std::function<size_type(const key_type&, size_type)> hash_;

    typename container_type::const_iterator iter_at(const key_type& key, const size_type &hash_idx) const {
        auto idx = hash_idx;
        for(auto i = 0; i < size_ && table_[idx].is_some(); i++, idx = (idx + 1) % size_) {
            if (table_[idx].peek().first == key)
                return table_.begin() + idx;
        }

        return table_.end();
    }
    typename container_type::iterator iter_at(const key_type& key, const size_type &hash_idx) {
        auto idx = hash_idx;
        for(auto i = 0; i < size_ && table_[idx].is_some(); i++, idx = (idx + 1) % size_) {
            if (table_[idx].peek().first == key)
                return table_.begin() + idx;
        }

        return table_.end();
    }
    typename container_type::iterator insert(const key_type &key, const mapped_type &mapped_value, const size_type &hash_idx) {
        {
            auto it = iter_at(key, hash_idx);
            if (it != table_.end()) return it;
        }

        if (left_ == 0) return table_.end();

        --left_;
        auto idx = hash_idx;
        while (table_[idx].is_some()) idx = (idx + 1) % size_;
        table_[idx] = Option(value_type(key, mapped_value));

        return table_.begin() + idx;
    }

public:
    HashTable() : size_{0}, left_{0} {
        hash_ = [](const key_type &key, size_type size) { return 0; };
    }
    explicit HashTable(size_type size) : size_{size}, left_{size_} {
        table_.resize(size_);
        hash_ = [](const key_type &key, size_type size) { return (size_type)key % size; };
    }
    HashTable(size_type size, std::function<std::size_t(const key_type&, size_type)> hash) : size_{size}, left_{size_} {
        table_.resize(size_);
        hash_ = hash;
    }

    size_type hash(const key_type &key) const { return hash_(key, size_); }

    std::vector <std::pair<key_type const &, mapped_type&>> get_all() {
        typedef std::pair<key_type const &, mapped_type&> return_value_type;

        std::vector <return_value_type> vec;
        for(auto &p : table_) {
            if (p.is_some()) {
                vec.push_back(return_value_type(p.peek().first, p.peek().second));
            }
        }

        return vec;
    }
    std::vector <std::pair<key_type const &, mapped_type const &>> get_all() const {
        typedef std::pair<key_type const &, mapped_type const &> return_value_type;

        std::vector <return_value_type> vec;
        for(auto &p : table_) {
            if (p.is_some()) {
                vec.push_back(return_value_type(p.peek().first, p.peek().second));
            }
        }

        return vec;
    }

    bool contains(const key_type &key) const { return iter_at(key, hash(key)) != table_.end(); }
    [[nodiscard]] size_type size() const { return size_; }

    /// Allows modifications, i.e. `table.at(0) = sth;`\n
    /// Exception(s): key not found
    mapped_type& at(const key_type &key) {
        auto it = iter_at(key, hash(key));
        if (it == table_.end()) {
            throw std::out_of_range("Key not found.");
        }

        return it->unwrap().second;
    }
    /// Does not allow modifications.\n
    /// Exception(s): key not found
    mapped_type const& at(const key_type &key) const {
        auto it = iter_at(key, hash(key));
        if (it == table_.end()) {
            throw std::out_of_range("Key not found.");
        }

        return it->peek().second;
    }
    /// Allows modifications, i.e. `table[0] = sth;`\n
    /// Creates a new slot if table doesn't contain the key.\n
    /// Exception(s): table is full
    mapped_type& operator[](const key_type &key) {
        auto it = insert(key, {}, hash(key));

        if (it == table_.end())
            throw std::out_of_range("Hash table of size " + std::to_string(size_) + " is full.");
        return it->unwrap().second;
    }

    void clear() { std::fill(table_.begin(), table_.end(), {}); }
};