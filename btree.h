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
            Node(Item* item, Node* parent, bool isLeaf);

            std::string ToString();
            std::vector<Item*> items();

            Item* find(int key);
            void traverse(std::function<void(BTree::Item*)> inFn);

            void insert(int key, int value, bool assure);

            std::vector<Node*> children();
            void setItem(Item* item) { item_ = item; }
            void setParent(Node* node) { parent_ = node; }
            Node* parent() { return parent_; }
        private:
            bool assureNotFourNode();
            Item* item_ = nullptr;
            bool isLeaf_ = true;
            Node* parent_ = nullptr;
    };

    class Item {
        public:
            std::string ToString() {
                return "<Item: " + std::to_string(key_) + ", " + std::to_string(value_) + ">";
            }
            Item(int key, int value): key_(key), value_(value), nextItem_(nullptr), left_(nullptr), right_(nullptr) {}

            int key() {
                return key_;
            }

            int value() {
                return value_;
            }

            Node* right() {
                return right_;
            }

            void setRight(Node* node) {
                right_ = node;
            }

            void setLeft(Node* node) {
                left_ = node;
            }

            Node* left() {
                return left_;
            }
            Item* nextItem() {
                return nextItem_;
            }
            void setNext(Item* item) {
                nextItem_ = item;
            }
        private:
            int key_;
            int value_;
            Item* nextItem_;
            Node* left_;
            Node* right_;
    };


    class Tree {
        public:
            std::string ToString();

            Tree() { root_ = nullptr; }
            ~Tree() = default;
            Node* root() { return root_; }
            void insert(int key, int value);
            Item* find(int key);

        private:
            Node* root_;
    };

} // namespace BTree

#endif // BTREE_H
