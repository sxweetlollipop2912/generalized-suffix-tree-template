#pragma once

#include<iostream>

template<typename T>
class AVLTree {
    template<typename T_Key, typename T_Mapped> friend
    class Map;

public:
    using value_type = T;
    using size_type = std::size_t;

private:
    struct AVLNode {
        value_type data;
        AVLNode *left;
        AVLNode *right;
        size_type height;

        AVLNode(value_type data, AVLNode *left = nullptr, AVLNode *right = nullptr, size_type height = 0)
                : data{std::move(data)}, left{left}, right{right}, height{height} {}
    };

    AVLNode *root_;
    size_type size_;
    std::function<size_type(const value_type &, const value_type &)> comp_;

    void clear(AVLNode *&node) {
        if (node == nullptr) return;

        clear(node->left);
        clear(node->right);
        delete node;
        node = nullptr;
    }

    AVLNode *insert(const value_type &data, AVLNode *node) {
        if (node == nullptr) {
            node = new AVLNode(data);
            ++size_;
        } else if (comp_(data, node->data)) {
            node->left = insert(data, node->left);
            if (balance_factor(node) == 2) {
                if (comp_(data, node->left->data))
                    node = single_right_rotate(node);
                else
                    node = left_right_rotate(node);
            }
        } else if (comp_(node->data, data)) {
            node->right = insert(data, node->right);
            if (balance_factor(node) == -2) {
                if (comp_(node->right->data, data))
                    node = single_left_rotate(node);
                else
                    node = right_left_rotate(node);
            }
        }

        node->height = std::max(height(node->left), height(node->right)) + 1;

        return node;
    }

    AVLNode *single_right_rotate(AVLNode *&node) {
        AVLNode *u = node->left;
        node->left = u->right;
        u->right = node;
        node->height = std::max(height(node->left), height(node->right)) + 1;
        u->height = std::max(height(u->left), height(u->right)) + 1;

        return u;
    }

    AVLNode *single_left_rotate(AVLNode *&node) {
        AVLNode *u = node->right;
        node->right = u->left;
        u->left = node;
        node->height = std::max(height(node->left), height(node->right)) + 1;
        u->height = std::max(height(u->left), height(u->right)) + 1;

        return u;
    }

    AVLNode *right_left_rotate(AVLNode *&node) {
        node->right = single_right_rotate(node->right);

        return single_left_rotate(node);
    }

    AVLNode *left_right_rotate(AVLNode *&node) {
        node->left = single_left_rotate(node->left);

        return single_right_rotate(node);
    }

    AVLNode *find_min(AVLNode *node) const {
        if (!node || !node->left) return node;
        else return find_min(node->left);
    }

    AVLNode *find_max(AVLNode *node) const {
        if (!node || !node->right) return node;
        else return find_max(node->right);
    }

    AVLNode *remove(const value_type &data, AVLNode *node) {
        if (!node) return nullptr;

        // Searching for the element
        if (comp_(data, node->data))
            node->left = remove(data, node->left);
        else if (comp_(node->data, data))
            node->right = remove(data, node->right);
            // Element found
        else {
            AVLNode *temp;

            // Element has 2 children
            if (node->left && node->right) {
                temp = find_min(node->right);
                node->data = temp->data;
                node->right = remove(node->data, node->right);
            }
                // Element has 1 or 0 child
            else {
                temp = node;
                if (!node->left) node = node->right;
                else if (!node->right) node = node->left;

                delete temp;
                temp = nullptr;
                --size_;
            }
        }

        // If deleted node was a leaf
        if (!node) return nullptr;

        node->height = std::max(height(node->left), height(node->right)) + 1;
        // If node is right-heavy
        if (balance_factor(node) == -2) {
            // tree is in RR case
            if (balance_factor(node->right) == -1)
                return single_left_rotate(node);
                // RL case
            else
                return right_left_rotate(node);
        }
            // If node is left-heavy
        else if (balance_factor(node) == 2) {
            // tree is in LL case
            if (balance_factor(node->left) == 1)
                return single_right_rotate(node);
                // LR case
            else
                return left_right_rotate(node);
        }

        return node;
    }

    size_type height(AVLNode *node) const {
        return !node ? -1 : node->height;
    }

    size_type balance_factor(AVLNode *node) const {
        if (!node) return 0;

        return height(node->left) - height(node->right);
    }

    void inorder(AVLNode *node, const std::function<void(const value_type &)> &func) const {
        if (node == nullptr) return;

        inorder(node->left, func);
        func(node->data);
        inorder(node->right, func);
    }

    AVLNode *find(AVLNode *node, const value_type &value) {
        if (!node) return nullptr;

        AVLNode *re = nullptr;
        if (comp_(value, node->data))
            re = find(node->left, value);
        else if (comp_(node->data, value))
            re = find(node->right, value);
        else
            re = node;

        return re;
    }

    AVLNode const *find(AVLNode *node, const value_type &value) const {
        if (!node) return nullptr;

        AVLNode const *re = nullptr;
        if (comp_(value, node->data))
            re = find(node->left, value);
        else if (comp_(node->data, value))
            re = find(node->right, value);
        else
            re = node;

        return re;
    }

public:
    AVLTree() : root_{nullptr}, size_{0} {
        comp_ = [](const value_type &x1, const value_type &x2) { return x1 < x2; };
    }

    explicit AVLTree(const std::function<size_type(const value_type &, const value_type &)> &comp)
            : root_{nullptr}, comp_{comp}, size_{0} {}

    ~AVLTree() { clear(); }

    [[nodiscard]] size_type size() const { return size_; }

    bool contains(const value_type &value) const {
        return find(root_, value);
    }

    std::vector<value_type> to_vec() const {
        std::vector<value_type> v;
        v.reserve(size());
        inorder(root_, [&](const value_type &e) { v.push_back(e); });

        return v;
    }

    void clear() { clear(root_); }

    void insert(const value_type &data) { root_ = insert(data, root_); }

    bool erase(const value_type &value) {
        auto old_size = size();
        root_ = remove(value, root_);

        return old_size != size();
    }
};