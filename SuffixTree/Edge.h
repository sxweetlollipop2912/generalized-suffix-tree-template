#pragma once

class Node;

class Edge {
private:
    Node* dest_;

public:
    std::string label;

    Edge() = default;
    Edge(const std::string label, Node *const dest) : label{label}, dest_{dest} {}

    void set_dest(Node *node) { dest_ = node; }
    Node const* dest() const { return dest_; }
    Node* dest() { return dest_; }
};