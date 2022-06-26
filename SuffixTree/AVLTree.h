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
        value_type data_;
        AVLNode *left_;
        AVLNode *right_;
        size_type height_;

        explicit AVLNode(value_type data, AVLNode *left = nullptr, AVLNode *right = nullptr, size_type height = 0)
                : data_{std::move(data)}, left_{left}, right_{right}, height_{height} {}
    };

    AVLNode *root_;
    size_type size_;
    std::function<size_type(const value_type &, const value_type &)> comp_;

    void clear(AVLNode *&node) {
        if (node == nullptr) return;

        clear(node->left_);
        clear(node->right_);
        delete node;
        node = nullptr;
    }

    AVLNode *insert(const value_type &data, AVLNode *node) {
        if (node == nullptr) {
            node = new AVLNode(data);
            ++size_;
        } else if (comp_(data, node->data_)) {
            node->left_ = insert(data, node->left_);
            if (balance_factor(node) == 2) {
                if (comp_(data, node->left_->data_))
                    node = single_right_rotate(node);
                else
                    node = left_right_rotate(node);
            }
        } else if (comp_(node->data_, data)) {
            node->right_ = insert(data, node->right_);
            if (balance_factor(node) == -2) {
                if (comp_(node->right_->data_, data))
                    node = single_left_rotate(node);
                else
                    node = right_left_rotate(node);
            }
        }

        node->height_ = std::max(height(node->left_), height(node->right_)) + 1;

        return node;
    }

    AVLNode *single_right_rotate(AVLNode *&node) {
        AVLNode *u = node->left_;
        node->left_ = u->right_;
        u->right_ = node;
        node->height_ = std::max(height(node->left_), height(node->right_)) + 1;
        u->height_ = std::max(height(u->left_), height(u->right_)) + 1;

        return u;
    }

    AVLNode *single_left_rotate(AVLNode *&node) {
        AVLNode *u = node->right_;
        node->right_ = u->left_;
        u->left_ = node;
        node->height_ = std::max(height(node->left_), height(node->right_)) + 1;
        u->height_ = std::max(height(u->left_), height(u->right_)) + 1;

        return u;
    }

    AVLNode *right_left_rotate(AVLNode *&node) {
        node->right_ = single_right_rotate(node->right_);

        return single_left_rotate(node);
    }

    AVLNode *left_right_rotate(AVLNode *&node) {
        node->left_ = single_left_rotate(node->left_);

        return single_right_rotate(node);
    }

    AVLNode *find_min(AVLNode *node) const {
        if (!node || !node->left_) return node;
        else return find_min(node->left_);
    }

    AVLNode *find_max(AVLNode *node) const {
        if (!node || !node->right_) return node;
        else return find_max(node->right_);
    }

    AVLNode *remove(const value_type &data, AVLNode *node) {
        if (!node) return nullptr;

        // Searching for the element
        if (comp_(data, node->data_))
            node->left_ = remove(data, node->left_);
        else if (comp_(node->data_, data))
            node->right_ = remove(data, node->right_);
            // Element found
        else {
            AVLNode *temp;

            // Element has 2 children
            if (node->left_ && node->right_) {
                temp = find_min(node->right_);
                node->data_ = temp->data_;
                node->right_ = remove(node->data_, node->right_);
            }
                // Element has 1 or 0 child
            else {
                temp = node;
                if (!node->left_) node = node->right_;
                else if (!node->right_) node = node->left_;

                delete temp;
                temp = nullptr;
                --size_;
            }
        }

        // If deleted node was not a leaf
        if (node) {
            node->height_ = std::max(height(node->left_), height(node->right_)) + 1;
            // If node is right-heavy
            if (balance_factor(node) == -2) {
                // tree is in RR case
                if (balance_factor(node->right_) == -1)
                    node = single_left_rotate(node);
                    // RL case
                else
                    node = right_left_rotate(node);
            }
                // If node is left-heavy
            else if (balance_factor(node) == 2) {
                // tree is in LL case
                if (balance_factor(node->left_) == 1)
                    node = single_right_rotate(node);
                    // LR case
                else
                    node = left_right_rotate(node);
            }
        }

        return node;
    }

    size_type height(AVLNode *node) const {
        return !node ? -1 : node->height_;
    }

    size_type balance_factor(AVLNode *node) const {
        if (!node) return 0;

        return height(node->left_) - height(node->right_);
    }

    void inorder(AVLNode *node, const std::function<void(const value_type &)> &func) const {
        if (node == nullptr) return;

        inorder(node->left_, func);
        func(node->data_);
        inorder(node->right_, func);
    }

    AVLNode *find(AVLNode *node, const value_type &value) {
        if (!node) return nullptr;

        AVLNode *re = nullptr;
        if (comp_(value, node->data_))
            re = find(node->left_, value);
        else if (comp_(node->data_, value))
            re = find(node->right_, value);
        else
            re = node;

        return re;
    }

    AVLNode const *find(AVLNode *node, const value_type &value) const {
        if (!node) return nullptr;

        AVLNode const *re = nullptr;
        if (comp_(value, node->data_))
            re = find(node->left_, value);
        else if (comp_(node->data_, value))
            re = find(node->right_, value);
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