#ifndef BTREE_H
#define BTREE_H

#include <string>
#include <vector>
#include <iostream>
#include <functional>

namespace BTree {

    class Item;

    /* Node represents a node of twoThreeFour tree.
     * Node can be twoNode, threeNode or fourNode.
     * TwoNode has two children, threeNode has three and fourNode has... four.
     * Keys and values in node are represented by Items.
     */
    class Node {
        public:
            // default constructor is deleted.
            Node() = delete;

            Node(int key, int value);
            Node(Item* item, Node* parent, bool is_leaf);

            std::string ToString();

            Item* Find(int key);
            void Insert(int key, int value, bool assure);

            void Traverse(std::function<void(BTree::Item*)> fn);

            std::vector<Item*> items();
            std::vector<Node*> children();

            void SetItem(Item* item) { item_ = item; }

            Node* parent() { return parent_; }
            void SetParent(Node* node) { parent_ = node; }
            bool IsLeaf() { return is_leaf_; }
        private:
            bool AssureNotFourNode();
            Item* item_ = nullptr;
            bool is_leaf_ = true;
            Node* parent_ = nullptr;
    };

    class Item {
        public:
            Item(int key, int value): key_(key), value_(value) {}

            std::string ToString() {
                return "<Item: " + std::to_string(key_) + ", " + std::to_string(value_) + ">";
            }

            int key() {
                return key_;
            }

            int value() {
                return value_;
            }

            Node* right() {
                return right_;
            }

            void SetRight(Node* node) {
                right_ = node;
            }

            Node* left() {
                return left_;
            }

            void SetLeft(Node* node) {
                left_ = node;
            }

            Item* NextItem() {
                return next_item_;
            }
            void SetNext(Item* item) {
                next_item_ = item;
            }
        private:
            int key_;
            int value_;
            Item* next_item_ = nullptr;
            Node* left_ = nullptr;
            Node* right_ = nullptr;
    };


    class Tree {
        public:
            std::string ToString();

            Tree() {}
            ~Tree() = default;
            Node* root() { return root_; }
            void Insert(int key, int value);
            Item* Find(int key);

        private:
            Node* root_ = nullptr;
    };

} // namespace BTree

#endif // BTREE_H
