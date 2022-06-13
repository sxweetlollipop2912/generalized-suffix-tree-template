#pragma once

#include "KeyInternal.h"

template<typename T_Key>
class Node;

template<typename T_Key>
class Edge {
private:
    Node<T_Key> *dest_{};

public:
    KeyInternal<T_Key> label;

    Edge() = default;

    Edge(const KeyInternal<T_Key> label, Node<T_Key> *const dest) : label{label}, dest_{dest} {}

    void set_dest(Node<T_Key> *node) { dest_ = node; }

    Node<T_Key> const *dest() const { return dest_; }

    Node<T_Key> *dest() { return dest_; }
};