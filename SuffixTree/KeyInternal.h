#pragma once

#include <algorithm>

template<typename T_Key>
class KeyInternal {
private:
    /// Start and end (1 past last position) of a substring
    using const_iterator = typename T_Key::const_iterator;

    const_iterator begin_;
    const_iterator end_;

public:
    using value_type = typename T_Key::value_type;
    using size_type = typename T_Key::size_type;

    KeyInternal() = default;

    KeyInternal(const T_Key &key)
            : begin_(std::cbegin(key)), end_(std::cend(key)) {}

    KeyInternal(const const_iterator &begin, const const_iterator &end)
            : begin_(begin), end_(end) {}

    KeyInternal(const KeyInternal &src) = default;

    KeyInternal &operator=(const KeyInternal &src) = default;

    KeyInternal(KeyInternal &&src) noexcept = default;

    KeyInternal &operator=(KeyInternal &&src) noexcept = default;

    // Time complexity: O(1) if differs in size, otherwise O(n) where n is the length of 2 keys.
    bool operator==(const KeyInternal &other) const {
        if (this->size() != other.size())
            return false;

        return this->has_prefix(other);
    }

    inline const_iterator begin() const { return begin_; }

    inline const_iterator end() const { return end_; }

    inline const_iterator iter_at(int idx) const { return this->begin() + idx; }

    inline value_type at(int idx) const { return *(this->begin() + idx); }

    [[nodiscard]] inline size_type size(size_type from_idx = 0) const {
        const auto begin = std::next(this->begin(), from_idx);
        const auto end = this->end();
        if (end <= begin) {
            return 0;
        }

        return end - begin;
    }

    [[nodiscard]] inline bool empty() const {
        return this->begin() >= this->end();
    }

    inline KeyInternal substr(size_type from_idx) const {
        const auto start_used = std::next(this->begin(), from_idx);
        const auto key_end = this->end();
        auto result = KeyInternal((start_used < key_end) ? start_used : key_end,
                                  key_end);

        return result;
    }

    inline KeyInternal substr(size_type from_idx, size_type len) const {
        const auto start_used = std::next(this->begin(), from_idx);
        const auto end_used = std::next(start_used, len);
        const auto key_end = this->end();
        auto result = KeyInternal((start_used < key_end) ? start_used : key_end,
                                  (end_used < key_end) ? end_used : key_end);

        return result;
    }

    bool has_prefix(const KeyInternal &prefix, size_type str_begin_idx = 0, size_type prefix_begin_idx = 0) const {
        if (this->size() < prefix.size()) return false;

        const auto prefix_begin = std::next(prefix.begin(), prefix_begin_idx);
        const auto prefix_end = prefix.end();
        const auto iters = std::mismatch(std::next(this->begin(), str_begin_idx), this->end(),
                                         prefix_begin, prefix_end);

        return iters.second == prefix_end;
    }

    [[nodiscard]] T_Key debug(size_type pos = 0) const {
        const auto key_start = std::next(this->begin(), pos);
        const auto key_end = this->end();
        if (key_end <= key_start) {
            return {};
        }

        return T_Key(key_start, key_end);
    }
};