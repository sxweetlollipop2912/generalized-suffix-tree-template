#pragma once

#include <algorithm>

template<typename T_Key>
class KeyInternal {
    /// Start and end (1 past last position) of a substring
    using KeyConstIterator = typename T_Key::const_iterator;
    using T_Element = typename T_Key::value_type;
private:
    KeyConstIterator begin_;
    KeyConstIterator end_;

public:
    KeyInternal() = default;

    KeyInternal(const T_Key &key)
            : begin_(std::cbegin(key)), end_(std::cend(key)) {}

    KeyInternal(const KeyConstIterator &begin, const KeyConstIterator &end)
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

    inline KeyConstIterator begin() const { return begin_; }

    inline KeyConstIterator end() const { return end_; }

    inline KeyConstIterator iter_at(int idx) const { return this->begin() + idx; }

    inline T_Element at(int idx) const { return *(this->begin() + idx); }

    [[nodiscard]] inline std::size_t size(std::size_t from_idx = 0) const {
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

    inline KeyInternal substr(std::size_t from_idx) const {
        const auto start_used = std::next(this->begin(), from_idx);
        const auto key_end = this->end();
        auto result = KeyInternal(
                (start_used < key_end) ? start_used : key_end,
                key_end);

        return result;
    }

    inline KeyInternal substr(std::size_t from_idx, std::size_t len) const {
        const auto start_used = std::next(this->begin(), from_idx);
        const auto end_used = std::next(start_used, len);
        const auto key_end = this->end();
        auto result = KeyInternal(
                (start_used < key_end) ? start_used : key_end,
                (end_used < key_end) ? end_used : key_end);

        return result;
    }

    bool has_prefix(const KeyInternal &prefix, std::size_t str_begin_idx = 0, std::size_t prefix_begin_idx = 0) const {
        if (this->size() < prefix.size()) return false;

        const auto prefix_begin = std::next(prefix.begin(), prefix_begin_idx);
        const auto prefix_end = prefix.end();
        const auto iters = std::mismatch(std::next(this->begin(), str_begin_idx), this->end(),
                                         prefix_begin, prefix_end);

        return iters.second == prefix_end;
    }

    [[nodiscard]] std::string debug(std::size_t pos = 0) const {
        const auto key_start = std::next(this->begin(), pos);
        const auto key_end = this->end();
        if (key_end <= key_start) {
            return {};
        }

        return std::string(key_start, key_end);
    }
};