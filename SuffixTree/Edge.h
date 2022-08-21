#pragma once

#include <algorithm>

#include "KeyInternal.h"

template<typename T_Key, typename T_Mapped>
class Node;

template<typename T_Key, typename T_Mapped>
class Edge {
public:
    using key_type = T_Key;
    using mapped_type = T_Mapped;

private:
    using node_type = Node<key_type, mapped_type>;

    node_type *dest_ = nullptr;

public:
    key_type label;

    Edge() = default;

    Edge(const key_type label, node_type *dest) : label{label}, dest_{dest} {}

    void set_dest(node_type *node) { dest_ = node; }

    node_type const *dest() const { return dest_; }

    node_type *dest() { return dest_; }
};