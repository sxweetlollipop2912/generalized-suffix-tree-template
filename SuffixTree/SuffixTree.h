#pragma once

#include <string>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <set>

#include "Node.h"

template<typename T_Key>
static KeyInternal<T_Key> safe_cut_last_char(const KeyInternal<T_Key> &s) {
    if (s.empty())
        return s;
    return s.substr(0, s.size() - 1);
}

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
 * In addition to those strings, there are a set of different strings that are implicitly contained within the GST, and it is composed of
 * the strings built by concatenating e1.label + e2.label + ... + $end, where e1, e2, ... is a proper path and $end is prefix of any of
 * the labels of the edges starting from the last node of the path.
 *
 * This kind of "implicit path" is important in the testAndSplit method.
 *
 */
template<typename T_String, typename T_Mapped>
class SuffixTree {
public:
    using key_type = KeyInternal<T_String>;
    using mapped_type = T_Mapped;
    using value_type = std::pair<key_type, mapped_type>;
    using size_type = std::size_t;

private:
    using element_type = typename key_type::value_type;
    using node_type = Node<key_type, mapped_type>;
    using edge_type = Edge<key_type, mapped_type>;

    std::vector<node_type *> all_nodes;
    std::vector<edge_type *> all_edges;
    /**
     * The root of the suffix tree
     */
    node_type *root;
    /**
     * The last leaf that was added during the update operation
     */
    node_type *active_leaf;

    node_type *make_node() {
        all_nodes.push_back(new node_type);
        return all_nodes.back();
    }

    edge_type *make_edge(const key_type &label, node_type *dest) {
        all_edges.push_back(new edge_type(label, dest));
        return all_edges.back();
    }

    /**
     * Returns the tree node (if present) that corresponds to the given string.
     */
    node_type const *search_node(const T_String &word) const {
        /*
         * Verifies if exists a path from the root to a node such that the concatenation
         * of all the labels on the path is a super string of the given word.
         * If such a path is found, the last node on it is returned.
         */
        auto node = root;

        for (auto it = word.begin(); it != word.end(); it++) {
            // follow the edge corresponding to this char
            auto edge = node->get_edge(*it);

            if (edge) {
                for (auto iw = it, il = edge->label.begin(); iw != word.end() && il < edge->label.end(); iw++, il++)
                    // *iw != *il
                    if (*iw < *il || *il < *iw)
                        // the label on the edge does not correspond to the one in the string to search
                        return nullptr;

                if (edge->label.size() < word.end() - it) {
                    // advance to next node
                    node = edge->dest();
                    it += edge->label.size() - 1;
                } else
                    // there is no edge starting with this char
                    return edge->dest();
            } else
                return nullptr;
        }

        return nullptr;
    }

    /**
     * Return a (Node, string) (n, remainder) pair such that n is a farthest descendant of
     * input_node (the input node) that can be reached by following a path of edges denoting
     * a prefix of input and remainder will be string that must be
     * appended to the concatenation of labels from input_node to n to get input.
     */
    std::pair<node_type *, key_type>
    canonize(node_type *input_node, key_type input) {
        if (!input.empty()) {
            auto edge = input_node->get_edge(*input.begin());

            // descend the tree as long as a proper label is found
            while (edge && input.has_prefix(edge->label)) {
                input = input.substr(edge->label.size());
                input_node = edge->dest();
                if (!input.empty())
                    edge = input_node->get_edge(*input.begin());
            }
        }

        return std::make_pair(input_node, std::move(input));
    }

    /**
     * Tests whether the string part + t is contained in the subtree that has input_node as root.
     * If that's not the case, and there exists a path of edges e1, e2, ... such that
     *     e1.label + e2.label + ... + $end = part
     * and there is an edge g such that
     *     g.label = part + rest
     *
     * Then g will be split in two different edges, one having $end as label, and the other one
     * having rest as label.
     *
     * @param input_node starting node
     * @param part the string to search
     * @param t the following character
     * @param remainder the remainder of the string to add to the index
     * @param value the value to add to the index
     * @return a pair containing:
     *                  true/false depending on whether (part + t) is contained in the subtree starting in input_node
     *                  the last node that can be reached by following the path denoted by part starting from input_node
     *
     */
    std::pair<bool, node_type *>
    test_and_split(node_type *input_node,
                   const key_type &part,
                   const element_type &t,
                   const key_type &remainder,
                   mapped_type value) {
        // descend the tree as far as possible
        auto[node, str] = canonize(input_node, part);
        std::pair<bool, node_type *> re;

        if (!str.empty()) {
            auto edge = node->get_edge(*str.begin());
            auto &label = edge->label;

            // must see whether "str" is substring of the label of an edge
            if (label.size() > str.size() &&
                // label[str.size()] == t
                // Strictly use operator < as label[str.size()] and t might be custom objects.
                !(*(label.iter_at(str.size())) < t) &&
                !(t < *(label.iter_at(str.size()))))

                re = std::make_pair(true, node);
            else {
                // need to split the edge
                assert(label.has_prefix(str));
                label = label.substr(str.size());

                // build a new node
                auto new_node = make_node();
                // build a new edge
                auto new_edge = make_edge(str, new_node);

                // link node -> new_node
                new_node->add_edge(*label.begin(), edge);
                node->add_edge(*str.begin(), new_edge);

                re = std::make_pair(false, new_node);
            }
        } else {
            auto edge = node->get_edge(t);
            if (edge) {
                if (remainder == edge->label) {
                    // update payload of destination node
                    edge->dest()->add_ref(value);

                    re = std::make_pair(true, node);
                } else if (remainder.has_prefix(edge->label)) {

                    re = std::make_pair(true, node);
                } else if (edge->label.has_prefix(remainder)) {
                    // need to split as above
                    auto new_node = make_node();
                    new_node->add_ref(value);

                    auto new_edge = make_edge(remainder, new_node);
                    edge->label = edge->label.substr(remainder.size());
                    new_node->add_edge(*edge->label.begin(), edge);
                    node->add_edge(t, new_edge);

                    re = std::make_pair(false, node);
                } else {
                    // they are different words. No prefix. but they may still share some common substr
                    re = std::make_pair(true, node);
                }
            } else {
                // if there is no t-transition from node
                re = std::make_pair(false, node);
            }
        }

        return re;
    }

    /**
     * Updates the tree starting from input_node and by adding part.
     *
     * Returns a reference (Node, string) pair for the string that has been added so far.
     * This means:
     * - the Node will be the Node that can be reached by the longest path string (S1)
     *   that can be obtained by concatenating consecutive edges in the tree and
     *   that is a substring of the string added so far to the tree.
     * - the string will be the remainder that must be added to S1 to get the string
     *   added so far.
     *
     * @param input_node the node to start from
     * @param part the string to add to the tree
     * @param rest the rest of the string
     * @param value the value to add to the index
     */
    std::pair<node_type *, key_type>
    update(node_type *input_node, const key_type &part, const element_type &new_char,
           const key_type &rest, mapped_type value) {
        auto tmp_part = part;
        auto input = input_node;

        auto[endpoint, node] = test_and_split(input,
                                              part.substr(0, part.size() - 1),
                                              new_char,
                                              rest,
                                              value);
        auto old_root = root;

        while (!endpoint) {
            node_type *leaf;
            auto tmp_edge = node->get_edge(new_char);
            if (tmp_edge)
                // such a node is already present. This is one of the main differences from Ukkonen's case:
                // the tree can contain deeper nodes at this stage because different strings were added by previous iterations.
                leaf = tmp_edge->dest();
            else {
                // must build a new leaf
                leaf = make_node();
                leaf->add_ref(value);
                node->add_edge(new_char, make_edge(rest, leaf));
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
            } else {
                auto[node_, str] = canonize(input->get_suffix(),
                                            safe_cut_last_char(tmp_part));
                input = node_;
                tmp_part = {str.begin(), ++str.end()};
            }

            auto[endpoint_, node_] = test_and_split(input,
                                                    safe_cut_last_char(tmp_part),
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
    SuffixTree() {
        root = make_node();
        active_leaf = root;
    }

    ~SuffixTree() {
        for (auto &e: all_nodes) delete e;
        for (auto &e: all_edges) delete e;
    }

    /**
     * Searches for the given word within the GST and returns at most the given number of matches.
     *
     * @param word the key to search for
     * @param count the max number of results to return
     * @return at most <tt>results</tt> values for the given word
     */
    std::set<mapped_type> search(const T_String &word, int count) const {
        auto tmp = search_node(word);

        if (tmp)
            return tmp->get_data(count);
        return {};
    }

    /**
     * Searches for the given word within the GST.
     *
     * Returns all the indexes for which the key contains the <tt>word</tt> that was
     * supplied as input.
     *
     * @param word the key to search for
     * @return the collection of indexes associated with the input <tt>word</tt>
     */
    std::set<mapped_type> search(const T_String &word) const {
        return search(word, -1);
    }

    /**
     * Adds the specified <tt>index</tt> to the GST under the given <tt>key</tt>.
     *
     * Entries must be inserted so that their indexes are in non-decreasing order,
     * otherwise an IllegalStateException will be raised.
     *
     * @param string the string key that will be added to the index
     * @param index the value that will be added to the index
     */
    void put(const T_String &string, mapped_type index) {
        key_type key(string);

        // reset active_leaf
        active_leaf = root;

        // proceed with tree construction (closely related to procedure in
        // Ukkonen's paper)
        auto node = root;
        key_type text(key.begin(), key.begin());
        // iterate over the string, one char at a time
        for (auto i = 0; i < key.size(); i++) {
            text = {text.begin(), ++text.end()};

            // update the tree with the new transitions due to this new char
            auto active = update(node, KeyInternal(text), key.at(i), key.substr(i), index);

            // make sure the active pair is canonical
            active = canonize(active.first, active.second);
            node = active.first;
            text = active.second;
        }

        // add leaf suffix link, is necessary
        if (!active_leaf->get_suffix() && active_leaf != root && active_leaf != node)
            active_leaf->set_suffix(node);
    }
};
