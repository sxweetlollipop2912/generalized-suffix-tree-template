#pragma once

#include <string>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <iostream>

#include "Node.h"
#include "Utils.h"

struct ResultSuffix {
    int total;
    std::set<int> results;

    ResultSuffix(int total, std::set<int> set) : total{total}, results{std::move(set)} {}
};

class SuffixTree {
private:
    Node *const root = new Node;
    int last;
    Node *active_leaf;

    Node const* search_node(const std::string &word) const {
        Node const* node = root;

        for(int i = 0; i < word.size(); i++) {
            auto c = word.at(i);
            auto edge = node->get_edge(c);

            if (edge) {
                for(int iw = i, il = 0; iw < word.size() && il < edge->label.size(); iw++, il++)
                    if (word.at(iw) != edge->label.at(il))

                        return nullptr;

                if (edge->label.size() < word.size() - i) {
                    node = edge->dest();
                    i += edge->label.size() - 1;
                }
                else

                    return edge->dest();
            }
            else
                return nullptr;
        }

        return nullptr;
    }

    std::pair<Node*, std::string> canonize(Node* node, std::string input) {
        if (!input.empty()) {
            auto edge = node->get_edge(input[0]);

            while (edge && input.size() >= edge->label.size()) {
                if (input.substr(0, edge->label.size()) == edge->label) {
                    input = input.substr(edge->label.size());
                    node = edge->dest();
                    if (!input.empty())
                        edge = node->get_edge(input[0]);
                } else
                    break;
            }
        }

        return std::make_pair(node, std::move(input));
    }

    std::pair<bool, Node*> test_and_split(Node* input, const std::string &part, char t, const std::string &remainder, int value) {
        auto [node, str] = canonize(input, part);

        if (!str.empty()) {
            auto edge = node->get_edge(str[0]);
            auto &label = edge->label;

            if (label.size() > str.size() && label[str.size()] == t)

                return std::make_pair(true, node);
            else {
                assert(label.substr(0, str.size()) == str);
                label = label.substr(str.size());

                auto new_node = new Node;
                auto new_edge = new Edge(str, new_node);

                new_node->add_edge(label[0], *edge);
                node->add_edge(str[0], *new_edge);

                return std::make_pair(false, new_node);
            }
        }
        else {
            auto edge = node->get_edge(t);
            if (edge) {
                if (remainder == edge->label) {
                    edge->dest()->add_ref(value);

                    return std::make_pair(true, node);
                }
                else if (remainder.substr(0, edge->label.size()) == edge->label) {

                    return std::make_pair(true, node);
                }
                else if (edge->label.substr(0, remainder.size()) == remainder) {
                    auto new_node = new Node;
                    new_node->add_ref(value);

                    auto new_edge = new Edge(remainder, new_node);
                    edge->label = edge->label.substr(remainder.size());
                    new_node->add_edge(edge->label[0], *edge);
                    node->add_edge(t, *new_edge);

                    return std::make_pair(false, node);
                }
                else {
                    return std::make_pair(true, node);
                }
            }
            else {
                return std::make_pair(false, node);
            }
        }
    }

    std::pair<Node*, std::string> update(Node* input_node, const std::string &part, const std::string &rest, int value) {
        auto &new_char = part.back();
        auto tmp_part = part;
        auto input = input_node;
        auto [endpoint, node] = test_and_split(input,
                                               part.substr(0, part.size() - 1),
                                               new_char,
                                               rest,
                                               value);
        auto old_root = root;

        while (!endpoint) {
            Node* leaf;
            auto tmp_edge = node->get_edge(new_char);
            if (tmp_edge)
                leaf = tmp_edge->dest();
            else {
                leaf = new Node;
                leaf->add_ref(value);
                node->add_edge(new_char, *(new Edge(rest, leaf)));
            }

            if (active_leaf != root)
                active_leaf->set_suffix(leaf);
            active_leaf = leaf;

            if (old_root != root)
                old_root->set_suffix(node);
            old_root = node;

            if (!input->get_suffix()) {
                assert(root == input);
                tmp_part = tmp_part.substr(1);
            }
            else {
                auto [node_, str] = canonize(input->get_suffix(), Utils::safe_cut_last_char(tmp_part));
                input = node_;
                tmp_part = str + tmp_part.back();
            }

            auto [endpoint_, node_] = test_and_split(input,
                                                     Utils::safe_cut_last_char(tmp_part),
                                                     new_char,
                                                     rest,
                                                     value);
            endpoint = endpoint_;
            node = node_;
        }

        if (old_root != root)
            old_root->set_suffix(node);

        return std::make_pair(input, tmp_part);
    }

public:
    SuffixTree() : last{0}, active_leaf{root} {}

    Node const* get_root() const { return root; }
    std::set<int> search(const std::string &word) const {
        return search(word, -1);
    }

    std::set<int> search(const std::string &word, int count) const {
        auto tmp = search_node(word);

        if (tmp)
            return tmp->get_data(count);
        return {};
    }

    ResultSuffix search_with_count(const std::string &word, int count) const {
        auto tmp = search_node(word);

        return tmp ? ResultSuffix(tmp->get_result_count(), tmp->get_data(count)) : ResultSuffix(0, {});
    }

    void put(const std::string &key, int idx) {
        if (idx < last) {
            throw std::runtime_error("Input index must not be less than any of the previously inserted ones. Got " + std::to_string(idx) + ", expected at least " + std::to_string(last));
        }

        last = idx;
        active_leaf = root;
        auto node = root;
        std::string text;

        for(int i = 0; i < key.size(); i++) {
            text += key.at(i);
            auto active = update(node, text, key.substr(i), idx);
            active = canonize(active.first, active.second);
            node = active.first;
            text = active.second;
        }

        if (!active_leaf->get_suffix() && active_leaf != root && active_leaf != node)
            active_leaf->set_suffix(node);
    }

    int compute_count() { return root->compute_and_cache_count(); }
};