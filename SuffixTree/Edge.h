#pragma once

#include <algorithm>

#include "KeyInternal.h"

template<typename T_Key>
class Node;

template<typename T_Key>
class Edge {
private:
    std::shared_ptr<Node<T_Key>> dest_{};

public:
    KeyInternal<T_Key> label;

    Edge() = default;

    Edge(const KeyInternal<T_Key> label, std::shared_ptr<Node<T_Key>> const dest) : label{label}, dest_{dest} {}

    void set_dest(std::shared_ptr<Node<T_Key>> node) { dest_ = node; }

    std::shared_ptr<Node<T_Key> const> dest() const { return dest_; }

    std::shared_ptr<Node<T_Key>> dest() { return dest_; }
};