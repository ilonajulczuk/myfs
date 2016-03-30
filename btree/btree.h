#ifndef BTREE_H
#define BTREE_H

#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>
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

            std::string ToString();

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

    template<typename K, typename V>
    Node<K, V>::Node(KeyType key, ValueType value) {
        item_ = new ItemT(key, value);
    }

    template<typename K, typename V>
    Node<K, V>::Node(ItemT* item, Node* parent, bool is_leaf) {
        parent_ = parent;
        item_ = item;
        is_leaf_ = is_leaf;
        ItemT* current = item;
        NodeT* child_node;
        while(current != nullptr) {
            child_node = item->right();
            if (child_node != nullptr) {
                child_node->SetParent(this);
            }
            child_node = item->left();
            if (child_node != nullptr) {
                child_node->SetParent(this);
            }
            current = current->NextItem();
        }
    }

    template<typename K, typename V>
    std::string Node<K, V>::ToString() {
        std::string desc = "<Node: [";
        for (auto item : items()) {
            desc += item->ToString();
        }
        if (parent_ == nullptr) {
            desc += ",nullptr parent";
        } else {
            desc += "," + parent_->ToString();
        }
        desc += "]>";
        return desc;
    }

    template<typename K, typename V>
    Item<K, V>* Node<K, V>::Find(KeyType key) {
        ItemT* current = item_;
        while (current != nullptr) {
            if (key == current->key()) {
                return current;
            }
            if (key < current->key()) {
                if (!IsLeaf()) {
                    return current->left()->Find(key);
                }
                return nullptr;
            }
            ItemT* next = current->NextItem();
            if (next == nullptr && !IsLeaf()) {
                return current->right()->Find(key);
            }
            current = next;
        }
        return nullptr;
    }

    template<typename K, typename V>
    bool Node<K, V>::AssureNotFourNode() {
        ItemT* current = item_;
        int total = 0;
        while (current != nullptr) {
            total++;
            current = current->NextItem();
        }

        if (total != 3) {
            return false;
        }

        ItemT* middle = item_->NextItem();
        Node* new_left = nullptr;
        Node* new_right = nullptr;

        if (parent_ == nullptr) {
            ItemT* right_item = item_->NextItem()->NextItem();
            new_right = new Node(right_item, this, IsLeaf());
            item_->SetNext(nullptr);
            new_left = new Node(item_, this, IsLeaf());
            is_leaf_ = false;
            item_ = middle;
            middle->SetNext(nullptr);
            middle->SetRight(new_right);
            middle->SetLeft(new_left);
            return false;
        }

        // Find the correct place to insert middle.
        // middle gets previous item in node as left child and next
        // as right previous node up, gets left child
        // as right child, and next node up, gets right child as left

        ItemT* previous = nullptr;
        current = parent_->item_;
        while (current != nullptr) {
            if (middle->key() < current->key()) {
                break;
            }
            previous = current;
            current = current->NextItem();
        }

        ItemT* right_item = item_->NextItem()->NextItem();
        new_right = new Node(right_item, parent_, IsLeaf());
        new_left = this;
        item_->SetNext(nullptr);

        middle->SetLeft(new_left);
        middle->SetNext(nullptr);
        middle->SetRight(new_right);

        if (previous == nullptr) {
            parent_->SetItem(middle);
        } else {
            previous->SetRight(new_left);
            previous->SetNext(middle);
        }

        if (current != nullptr) {
            middle->SetNext(current);
            current->SetLeft(new_right);
        }
        return true;
    }


    template<typename K, typename V>
    void Node<K, V>::Insert(KeyType key, ValueType value, bool assure) {
        // make sure that the node has less than three items.
        if (assure) {
            bool changed = AssureNotFourNode();
            if (changed) {
                return parent_->Insert(key, value, false);
            }
        }

        ItemT* previous = nullptr;
        ItemT* current = item_;
        while (current != nullptr) {
            if (key < current->key()) {
                if (!IsLeaf()) {
                    return current->left()->Insert(key, value, true);
                }
                ItemT* item = new ItemT(key, value);
                // Check if we're inserting before first item.
                if (previous == nullptr) {
                    item->SetNext(current);
                    item_ = item;
                } else {
                    item->SetNext(current);
                    previous->SetNext(item);
                }
                return;
            }
            ItemT* next = current->NextItem();
            if (next == nullptr) {
                if (!IsLeaf()) {
                    return current->right()->Insert(key, value, true);
                }
                ItemT* item = new ItemT(key, value);
                current->SetNext(item);
                return;
            }
            previous = current;
            current = next;
        }
    }

    template<typename K, typename V>
    std::pair<Node<K, V>*, Node<K, V>*> Node<K, V>::Adjacent(Node* node) {
        auto all_nodes = children();
        auto it = std::find(all_nodes.begin(), all_nodes.end(), node);
        auto pos = std::distance(all_nodes.begin(), it);
        std::pair<Node*, Node*> adjacent = {nullptr, nullptr};
        if (pos > 0) {
            adjacent.first = all_nodes[pos - 1];
        }
        if (pos < all_nodes.size() - 1) {
            adjacent.second = all_nodes[pos + 1];
        }
        return adjacent;
    }

    template<typename K, typename V>
    Item<K, V>* Node<K, V>::GetPrevious(ItemT* item) {
        ItemT* previous = nullptr;
        ItemT* current = item_;
        while (current != nullptr) {
           if (current->key() >= item->key()) {
               return previous;
           }
           previous = current;
           current = current->NextItem();
        }
        return previous;
    }

    template<typename K, typename V>
    Item<K, V>* Node<K, V>::GetLeftParentItem() {
        auto first_key = item_->key();
        auto all_items = parent_->items();
        ItemT* previous = nullptr;
        for (auto item : all_items) {
           if (item->key() >  first_key) {
               return previous;
           }
           previous = item;
        }
        return previous;
    }

    template<typename K, typename V>
    Item<K, V>* Node<K, V>::GetRightParentItem() {
        auto first_key = item_->key();
        auto all_items = parent_->items();
        for (auto item : all_items) {
           if (item->key() >  first_key) {
               return item;
           }
        }
        return nullptr;
    }

    template<typename K, typename V>
    void Node<K, V>::Delete(KeyType key, ItemT* to_replace) {
        // Eliminate 1-key nodes. (other than the root).
        // Rules for deletion are well described in a wikipedia article:
        // https://en.wikipedia.org/wiki/2%E2%80%933%E2%80%934_tree
        if (items().size() == 1) {
            if (!IsRoot()) {
                auto siblings = parent_->Adjacent(this);
                bool stolen = StealFromSibling(siblings);
                // if not rotated, try something else
                if (!stolen) {
                    // can either pull up to parent
                    if (parent_->items().size() == 1) {
                        PullUpToParent();
                        return parent_->Delete(key, to_replace);
                    }
                    if (siblings.first != nullptr) {
                        FuseLeft(siblings.first);
                    } else {
                        FuseRight(siblings.second);
                    }
                }
            }
        }

        if (!IsLeaf()) {
            ItemT* previous = nullptr;
            ItemT* current = item_;
            while(current != nullptr) {
                if (key == current->key()) {
                    // find successor and rotate all the way.
                    return current->right()->Delete(key, current);
                } else if (key < current->key()) {
                    return current->left()->Delete(key);
                } else if (current->NextItem() == nullptr) {
                    return current->right()->Delete(key);
                } else {
                    previous = current;
                    current = current->NextItem();
                }
            }
        }

        if (IsLeaf()) {
            ItemT* previous = nullptr;
            ItemT* current = item_;
            if (to_replace != nullptr) {
                to_replace->SetKey(current->key());
                to_replace->SetValue(current->value());
                item_ = current->NextItem();
                delete current;
                return;
            }

            while(current != nullptr) {
                if (key == current->key()) {
                    if (previous == nullptr) {
                        item_ = current->NextItem();
                    } else {
                        previous->SetNext(current->NextItem());
                    }
                    delete current;
                    return;
                }

                previous = current;
                current = current->NextItem();
            }
        }
    }

    template<typename K, typename V>
    bool Node<K, V>::StealFromSibling(std::pair<Node*, Node*> siblings) {
        bool has_stolen = false;
        if (siblings.first != nullptr && siblings.first->items().size() > 1) {
            auto sibling_size = siblings.first->items().size();
            ItemT* last = siblings.first->items()[sibling_size - 1];
            ItemT* before_last = siblings.first->items()[sibling_size - 2];
            before_last->SetNext(nullptr);
            ItemT* left_parent_item = GetLeftParentItem();
            ItemT* stolen = new ItemT(left_parent_item->key(), left_parent_item->value());
            left_parent_item->SetKey(last->key());
            left_parent_item->SetValue(last->value());

            stolen->SetNext(item_);
            stolen->SetLeft(last->right());
            stolen->SetRight(item_->left());
            item_ = stolen;
            delete last;
            has_stolen = true;
        } else if(siblings.second != nullptr && siblings.second->items().size() > 1) {
            ItemT* first = siblings.second->item_;
            ItemT* right_parent_item = GetRightParentItem();
            ItemT* stolen = new ItemT(right_parent_item->key(), right_parent_item->value());
            right_parent_item->SetKey(first->key());
            right_parent_item->SetValue(first->value());
            stolen->SetRight(first->left());
            stolen->SetLeft(item_->right());
            item_->SetNext(stolen);
            siblings.second->item_ = first->NextItem();
            delete first;
            has_stolen = true;
        }
        return has_stolen;
    }

    template<typename K, typename V>
    void Node<K, V>::FuseLeft(Node* sibling) {
        auto parent_item = GetLeftParentItem();
        auto left = sibling->item_;
        left->UpdateParent(this);

        auto right = item_;
        left->SetNext(parent_item);
        item_ = left;
        auto before_parent_item = parent_->GetPrevious(parent_item);
        if (before_parent_item == nullptr) {
            parent_->item_ = parent_item->NextItem();
        } else {
            before_parent_item->SetNext(parent_item->NextItem());
            before_parent_item->SetRight(this);
        }

        parent_item->SetNext(right);
        parent_item->SetLeft(left->right());
        parent_item->SetRight(right->left());
        delete sibling;
    }

    template<typename K, typename V>
    void Node<K, V>::FuseRight(Node* sibling) {
        auto parent_item = GetRightParentItem();
        auto right = sibling->item_;
        right->UpdateParent(this);
        auto left = item_;
        left->SetNext(parent_item);
        auto before_parent_item = parent_->GetPrevious(parent_item);
        if (before_parent_item == nullptr) {
            parent_->item_ = parent_item->NextItem();
        } else {
            before_parent_item->SetNext(parent_item->NextItem());
            before_parent_item->SetRight(this);
        }
        if (parent_item->NextItem() != nullptr) {
            parent_item->NextItem()->SetLeft(this);
        }

        parent_item->SetNext(right);
        parent_item->SetLeft(left->right());
        parent_item->SetRight(right->left());
        delete sibling;
    }

    template<typename K, typename V>
    void Node<K, V>::PullUpToParent() {
        ItemT* middle = parent_->item_;
        Node* left_node = middle->left();
        Node* right_node = middle->right();

        ItemT* left = left_node->item_;
        ItemT* right = right_node->item_;

        right->UpdateParent(parent_);
        left->UpdateParent(parent_);

        middle->SetRight(right->left());
        middle->SetLeft(left->right());

        if (left_node->IsLeaf()) {
            is_leaf_ = true;
        }
        left->SetNext(middle);
        middle->SetNext(right);
        delete left_node;
        delete right_node;
        parent_->item_ = left;
    }

    template<typename K, typename V>
    std::vector<Node<K, V>*> Node<K, V>::children() {
        std::vector<Node*> nodes;
        if (IsLeaf()) {
            return nodes;
        }

        ItemT* current = item_;
        nodes.push_back(current->left());
        while (current != nullptr) {
            nodes.push_back(current->right());
            ItemT* next = current->NextItem();
            current = next;
        }
        return nodes;
    }

    template<typename K, typename V>
    void Node<K, V>::Traverse(std::function<void(ItemT*)> fn) {
        ItemT* previous = nullptr;
        ItemT* current = item_;
        /*      4---6
         *    2   5   7
         */
        while (current != nullptr) {
            if (!IsLeaf() && previous == nullptr) {
                current->left()->Traverse(fn);
            }
            fn(current);
            if (!IsLeaf()) {
                current->right()->Traverse(fn);
            }
            previous = current;
            current = current->NextItem();
        }
    }

    template<typename K, typename V>
    std::vector<Item<K, V>*> Node<K, V>::items() {
        std::vector<ItemT*> all_items;
        ItemT* current = item_;
        while (current != nullptr) {
            all_items.push_back(current);
            current = current->NextItem();
        }
        return all_items;

    }

    template<typename K, typename V>
    std::string Item<K, V>::ToString() {
        return "<Item: " + std::to_string(key_) + ", " + std::to_string(value_) + ">";
    }

    template<typename K, typename V>
    std::string Tree<K, V>::ToString() {
        return "I'm a tree!";
    }

    template<typename K, typename V>
    void Tree<K, V>::Insert(KeyType key, ValueType value) {
        if(root_ == nullptr) {
            root_ = new Node<K, V>(key, value);
        } else {
            root_->Insert(key, value, true);
        }
    }

    template<typename K, typename V>
    Item<K, V>* Tree<K, V>::Find(KeyType key) {
        return root_->Find(key);
    }

    template<typename K, typename V>
    void Tree<K, V>::Delete(KeyType key) {
        return root_->Delete(key);
    }
} // namespace BTree

#endif // BTREE_H
