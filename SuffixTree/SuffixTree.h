#pragma once

#include <string>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <iostream>

#include "Node.h"
#include "Utils.h"

/**
 * An utility object, used to store the data returned by the SuffixTree.searchWithCount method.
 * It contains a collection of results and the total number of results present in the GST.
 * @see SuffixTree#searchWithCount(std::string, int)
 */
struct ResultSuffix {
    /**
     * The total number of results present in the database
     */
    int total;
    /**
    * The collection of (some) results present in the GST
    */
    std::set<int> results;

    ResultSuffix(int total, std::set<int> set) : total{total}, results{std::move(set)} {}
};

/**
 * A Generalized Suffix Tree, based on the Ukkonen's paper "On-line construction of suffix trees"
 * http://www.cs.helsinki.fi/u/ukkonen/SuffixT1withFigs.pdf
 *
 * Allows for fast storage and fast(er) retrieval by creating a tree-based index out of a set of strings.
 * Unlike common suffix trees, which are generally used to build an index out of one (very) long string,
 * a Generalized Suffix Tree can be used to build an index over many strings.
 *
 * Its main operations are put and search:
 * Put adds the given key to the index, allowing for later retrieval of the given value.
 * Search can be used to retrieve the set of all the values that were put in the index with keys that contain a given input.
 *
 * In particular, after put(K, V), search(H) will return a set containing V for any string H that is substring of K.
 *
 * The overall complexity of the retrieval operation (search) is O(m) where m is the length of the string to search within the index.
 *
 * Although the implementation is based on the original design by Ukkonen, there are a few aspects where it differs significantly.
 *
 * The tree is composed of a set of nodes and labeled edges. The labels on the edges can have any length as long as it's greater than 0.
 * The only constraint is that no two edges going out from the same node will start with the same character.
 *
 * Because of this, a given (startNode, stringSuffix) pair can denote a unique path within the tree, and it is the path (if any) that can be
 * composed by sequentially traversing all the edges (e1, e2, ...) starting from startNode such that (e1.label + e2.label + ...) is equal
 * to the stringSuffix.
 * See the search method for details.
 *
 * The union of all the edge labels from the root to a given leaf node denotes the set of the strings explicitly contained within the GST.
 * In addition to those Strings, there are a set of different strings that are implicitly contained within the GST, and it is composed of
 * the strings built by concatenating e1.label + e2.label + ... + $end, where e1, e2, ... is a proper path and $end is prefix of any of
 * the labels of the edges starting from the last node of the path.
 *
 * This kind of "implicit path" is important in the testAndSplit method.
 *
 */
class SuffixTree {
private:
    /**
     * The root of the suffix tree
     */
    Node *const root = new Node;
    /**
     * The index of the last item that was added to the GST
     */
    int last;
    /**
     * The last leaf that was added during the update operation
     */
    Node *active_leaf;

    /**
     * Returns the tree node (if present) that corresponds to the given string.
     */
    Node const* search_node(const std::string &word) const {
        /*
         * Verifies if exists a path from the root to a node such that the concatenation
         * of all the labels on the path is a super string of the given word.
         * If such a path is found, the last node on it is returned.
         */
        Node const* node = root;

        for(int i = 0; i < word.size(); i++) {
            auto c = word.at(i);
            // follow the edge corresponding to this char
            auto edge = node->get_edge(c);

            if (edge) {
                for(int iw = i, il = 0; iw < word.size() && il < edge->label.size(); iw++, il++)
                    if (word.at(iw) != edge->label.at(il))
                        // the label on the edge does not correspond to the one in the string to search
                        return nullptr;

                if (edge->label.size() < word.size() - i) {
                    // advance to next node
                    node = edge->dest();
                    i += edge->label.size() - 1;
                }
                else
                    // there is no edge starting with this char
                    return edge->dest();
            }
            else
                return nullptr;
        }

        return nullptr;
    }

    /**
     * Return a (Node, String) (n, remainder) pair such that n is a farthest descendant of
     * s (the input node) that can be reached by following a path of edges denoting
     * a prefix of input and remainder will be string that must be
     * appended to the concatenation of labels from s to n to get input.
     */
    std::pair<Node*, std::string> canonize(Node* node, std::string input) {
        if (!input.empty()) {
            auto edge = node->get_edge(input[0]);

            // descend the tree as long as a proper label is found
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

    /**
     * Tests whether the string part + t is contained in the subtree that has input as root.
     * If that's not the case, and there exists a path of edges e1, e2, ... such that
     *     e1.label + e2.label + ... + $end = part
     * and there is an edge g such that
     *     g.label = part + rest
     *
     * Then g will be split in two different edges, one having $end as label, and the other one
     * having rest as label.
     *
     * @param input starting node
     * @param part the string to search
     * @param t the following character
     * @param remainder the remainder of the string to add to the index
     * @param value the value to add to the index
     * @return a pair containing
     *                  true/false depending on whether (part + t) is contained in the subtree starting in input
     *                  the last node that can be reached by following the path denoted by part starting from input
     *
     */
    std::pair<bool, Node*> test_and_split(Node* input, const std::string &part, char t, const std::string &remainder, int value) {
        // descend the tree as far as possible
        auto [node, str] = canonize(input, part);

        if (!str.empty()) {
            auto edge = node->get_edge(str[0]);
            auto &label = edge->label;

            // must see whether "str" is substring of the label of an edge
            if (label.size() > str.size() && label[str.size()] == t)

                return std::make_pair(true, node);
            else {
                // need to split the edge
                assert(label.substr(0, str.size()) == str);
                label = label.substr(str.size());

                // build a new node
                auto new_node = new Node;
                // build a new edge
                auto new_edge = new Edge(str, new_node);

                // link node -> new_node
                new_node->add_edge(label[0], *edge);
                node->add_edge(str[0], *new_edge);

                return std::make_pair(false, new_node);
            }
        }
        else {
            auto edge = node->get_edge(t);
            if (edge) {
                if (remainder == edge->label) {
                    // update payload of destination node
                    edge->dest()->add_ref(value);

                    return std::make_pair(true, node);
                }
                else if (remainder.substr(0, edge->label.size()) == edge->label) {

                    return std::make_pair(true, node);
                }
                else if (edge->label.substr(0, remainder.size()) == remainder) {
                    // need to split as above
                    auto new_node = new Node;
                    new_node->add_ref(value);

                    auto new_edge = new Edge(remainder, new_node);
                    edge->label = edge->label.substr(remainder.size());
                    new_node->add_edge(edge->label[0], *edge);
                    node->add_edge(t, *new_edge);

                    return std::make_pair(false, node);
                }
                else {
                    // they are different words. No prefix. but they may still share some common substr
                    return std::make_pair(true, node);
                }
            }
            else {
                // if there is no t-transtion from node
                return std::make_pair(false, node);
            }
        }
    }

    /**
     * Updates the tree starting from inputNode and by adding part.
     *
     * Returns a reference (Node, String) pair for the string that has been added so far.
     * This means:
     * - the Node will be the Node that can be reached by the longest path string (S1)
     *   that can be obtained by concatenating consecutive edges in the tree and
     *   that is a substring of the string added so far to the tree.
     * - the String will be the remainder that must be added to S1 to get the string
     *   added so far.
     *
     * @param input_node the node to start from
     * @param part the string to add to the tree
     * @param rest the rest of the string
     * @param value the value to add to the index
     */
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
                // such a node is already present. This is one of the main differences from Ukkonen's case:
                // the tree can contain deeper nodes at this stage because different strings were added by previous iterations.
                leaf = tmp_edge->dest();
            else {
                // must build a new leaf
                leaf = new Node;
                leaf->add_ref(value);
                node->add_edge(new_char, *(new Edge(rest, leaf)));
            }

            // update suffix link for newly created leaf
            if (active_leaf != root)
                active_leaf->set_suffix(leaf);
            active_leaf = leaf;

            if (old_root != root)
                old_root->set_suffix(node);
            old_root = node;

            if (!input->get_suffix()) { // root node
                assert(root == input);
                // this is a special case to handle what is referred to as node _|_ on the paper
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

    /**
     * Searches for the given word within the GST.
     *
     * Returns all the indexes for which the key contains the <tt>word</tt> that was
     * supplied as input.
     *
     * @param word the key to search for
     * @return the collection of indexes associated with the input <tt>word</tt>
     */
    std::set<int> search(const std::string &word) const {
        return search(word, -1);
    }

    /**
     * Searches for the given word within the GST and returns at most the given number of matches.
     *
     * @param word the key to search for
     * @param count the max number of results to return
     * @return at most <tt>results</tt> values for the given word
     */
    std::set<int> search(const std::string &word, int count) const {
        auto tmp = search_node(word);

        if (tmp)
            return tmp->get_data(count);
        return {};
    }

    /**
     * Searches for the given word within the GST and returns at most the given number of matches.
     *
     * @param word the key to search for
     * @param count the max number of results to return
     * @return at most <tt>results</tt> values for the given word
     * @see SuffixTree#ResultSuffix
     */
    ResultSuffix search_with_count(const std::string &word, int count) const {
        auto tmp = search_node(word);

        return tmp ? ResultSuffix(tmp->get_result_count(), tmp->get_data(count)) : ResultSuffix(0, {});
    }

    /**
     * Adds the specified <tt>index</tt> to the GST under the given <tt>key</tt>.
     *
     * Entries must be inserted so that their indexes are in non-decreasing order,
     * otherwise an IllegalStateException will be raised.
     *
     * @param key the string key that will be added to the index
     * @param idx the value that will be added to the index
     * @throws std::runtime_error if an invalid index is passed as input
     */
    void put(const std::string &key, int idx) {
        if (idx < last) {
            throw std::runtime_error("Input index must not be less than any of the previously inserted ones. Got " + std::to_string(idx) + ", expected at least " + std::to_string(last));
        }
        last = idx;

        // reset activeLeaf
        active_leaf = root;

        auto node = root;
        // proceed with tree construction (closely related to procedure in
        // Ukkonen's paper)
        std::string text;
        // iterate over the string, one char at a time
        for(int i = 0; i < key.size(); i++) {
            text += key.at(i);

            // update the tree with the new transitions due to this new char
            auto active = update(node, text, key.substr(i), idx);

            // make sure the active pair is canonical
            active = canonize(active.first, active.second);
            node = active.first;
            text = active.second;
        }

        // add leaf suffix link, is necessary
        if (!active_leaf->get_suffix() && active_leaf != root && active_leaf != node)
            active_leaf->set_suffix(node);
    }

    int compute_count() { return root->compute_and_cache_count(); }
};