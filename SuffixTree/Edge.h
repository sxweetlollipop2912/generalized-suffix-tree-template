#pragma once

#include <algorithm>

#include "KeyInternal.h"

template<typename T_Key>
class SuffixNode;

template<typename T_Key>
class Edge {
private:
    SuffixNode<T_Key> *dest_ = nullptr;

public:
    KeyInternal<T_Key> label;

    Edge() = default;

    Edge(const KeyInternal<T_Key> label, SuffixNode<T_Key> *dest) : label{label}, dest_{dest} {}

    void set_dest(SuffixNode<T_Key> *node) { dest_ = node; }

    SuffixNode<T_Key> const *dest() const { return dest_; }

    SuffixNode<T_Key> *dest() { return dest_; }
};