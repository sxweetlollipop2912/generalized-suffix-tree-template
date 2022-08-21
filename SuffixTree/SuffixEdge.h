#pragma once

#include <algorithm>

#include "KeyInternal.h"

template<typename T_Key, typename T_Mapped>
class SuffixNode;

template<typename T_Key, typename T_Mapped>
class SuffixEdge {
public:
    using key_type = T_Key;
    using mapped_type = T_Mapped;

private:
    using node_type = SuffixNode<key_type, mapped_type>;

    node_type *dest_ = nullptr;

public:
    key_type label;

    SuffixEdge() = default;

    SuffixEdge(const key_type label, node_type *dest) : label{std::move(label)}, dest_{std::move(dest)} {}

    void set_dest(node_type *node) { dest_ = std::move(node); }

    node_type const *dest() const { return dest_; }

    node_type *dest() { return dest_; }
};