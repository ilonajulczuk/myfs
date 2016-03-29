#ifndef BTREE_H
#define BTREE_H

#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <utility>

namespace BTree {

    template<typename K = int, typename V = int>
    class Item;

    /* Node represents a node of twoThreeFour tree.
     * Node can be twoNode, threeNode or fourNode.
     * TwoNode has two children, threeNode has three and fourNode has... four.
     * Keys and values in node are represented by Items.
     */
    template<typename K = int, typename V = int>
    class Node {
        public:
            using ValueType = V;
            using KeyType = K;
            using ItemT = Item<K, V>;
            using NodeT = Node<K, V>;

            // default constructor is deleted.
            Node() = delete;

            Node(KeyType key, ValueType value);
            Node(ItemT* item, Node* parent, bool is_leaf);

            std::string ToString();

            ItemT* Find(KeyType key);
            void Insert(KeyType key, ValueType value, bool assure);
            void Delete(KeyType key, ItemT* to_replace=nullptr);

            void Traverse(std::function<void(ItemT*)> fn);

            std::vector<ItemT*> items();
            std::vector<Node<K, V>*> children();

            void SetItem(ItemT* item) { item_ = item; }

            Node* parent() { return parent_; }
            void SetParent(Node<K, V>* node) { parent_ = node; }
            bool IsLeaf() { return is_leaf_; }
            bool IsRoot() { return parent_ == nullptr; }
            std::pair<Node<K, V>*, Node<K, V>*> Adjacent(Node<K, V>* node);
        private:
            ItemT* GetPrevious(ItemT* item);
            bool AssureNotFourNode();
            bool StealFromSibling(std::pair<Node<K, V>*, Node<K, V>*> siblings);
            void PullUpToParent();
            void FuseLeft(Node<K, V>* sibling);
            void FuseRight(Node<K, V>* sibling);
            ItemT* GetLeftParentItem();
            ItemT* GetRightParentItem();
            ItemT* item_ = nullptr;
            bool is_leaf_ = true;
            Node<K, V>* parent_ = nullptr;
    };

    template<typename K, typename V>
    class Item {
        public:
            using ValueType = V;
            using KeyType = K;
            using NodeT = Node<K, V>;
            using ItemT = Item<K, V>;

            Item(KeyType key, ValueType value): key_(key), value_(value) {}

            std::string ToString() {
                return "<Item: " + std::to_string(key_) + ", " + std::to_string(value_) + ">";
            }

            KeyType key() {
                return key_;
            }

            void SetKey(KeyType key) {
                key_ = key;
            }

            ValueType value() {
                return value_;
            }

            void SetValue(ValueType value) {
                value_ = value;
            }

            NodeT* right() {
                return right_;
            }

            void SetRight(NodeT* node) {
                right_ = node;
            }

            NodeT* left() {
                return left_;
            }

            void SetLeft(NodeT* node) {
                left_ = node;
            }

            ItemT* NextItem() {
                return next_item_;
            }
            void SetNext(ItemT* item) {
                next_item_ = item;
            }

            void UpdateParent(NodeT* parent) {
                if (right_ != nullptr) {
                    right_->SetParent(parent);
                }
                if (left_ != nullptr) {
                    left_->SetParent(parent);
                }
            }
        private:
            KeyType key_;
            ValueType value_;
            ItemT* next_item_ = nullptr;
            NodeT* left_ = nullptr;
            NodeT* right_ = nullptr;
    };

    template<typename K = int, typename V = int>
    class Tree {
        public:
            using ValueType = V;
            using KeyType = K;
            using NodeT = Node<K, V>;
            using ItemT = Item<K, V>;

            std::string ToString();

            Tree() {}
            ~Tree() = default;
            NodeT* root() { return root_; }
            void Insert(KeyType key, ValueType value);
            void Delete(KeyType key);
            ItemT* Find(KeyType key);

        private:
            NodeT* root_ = nullptr;
    };

} // namespace BTree

#endif // BTREE_H
