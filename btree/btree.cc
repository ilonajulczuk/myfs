#include "btree.h"

/* TODO(att):
   - add deletion case
   */
namespace BTree {

    Node::Node(int key, int value) {
        item_ = new Item(key, value);
    }

    Node::Node(Item* item, Node* parent, bool is_leaf) {
        parent_ = parent;
        item_ = item;
        is_leaf_ = is_leaf;
        Item* current = item;
        Node *child_node;
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

    std::string Node::ToString() {
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

    Item* Node::Find(int key) {
        Item* current = item_;
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
            Item* next = current->NextItem();
            if (next == nullptr && !IsLeaf()) {
                return current->right()->Find(key);
            }
            current = next;
        }
        return nullptr;
    }

    bool Node::AssureNotFourNode() {
        Item* current = item_;
        int total = 0;
        while (current != nullptr) {
            total++;
            current = current->NextItem();
        }

        if (total != 3) {
            return false;
        }

        Item* middle = item_->NextItem();
        Node* new_left = nullptr;
        Node* new_right = nullptr;

        if (parent_ == nullptr) {
            Item* right_item = item_->NextItem()->NextItem();
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

        Item* previous = nullptr;
        current = parent_->item_;
        while (current != nullptr) {
            if (middle->key() < current->key()) {
                break;
            }
            previous = current;
            current = current->NextItem();
        }

        Item* right_item = item_->NextItem()->NextItem();
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


    void Node::Insert(int key, int value, bool assure) {
        // make sure that the node has less than three items.
        if (assure) {
            bool changed = AssureNotFourNode();
            if (changed) {
                return parent_->Insert(key, value, false);
            }
        }

        Item* previous = nullptr;
        Item* current = item_;
        while (current != nullptr) {
            if (key < current->key()) {
                if (!IsLeaf()) {
                    return current->left()->Insert(key, value, true);
                }
                Item* item = new Item(key, value);
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
            Item* next = current->NextItem();
            if (next == nullptr) {
                if (!IsLeaf()) {
                    return current->right()->Insert(key, value, true);
                }
                Item* item = new Item(key, value);
                current->SetNext(item);
                return;
            }
            previous = current;
            current = next;
        }
    }

    std::vector<Node*> Node::children() {
        std::vector<Node*> nodes;
        if (IsLeaf()) {
            return nodes;
        }

        Item* current = item_;
        nodes.push_back(current->left());
        while (current != nullptr) {
            nodes.push_back(current->right());
            Item* next = current->NextItem();
            current = next;
        }
        return nodes;
    }

    void Node::Traverse(std::function<void(BTree::Item*)> fn) {
        Item* previous = nullptr;
        Item* current = item_;
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

    std::vector<Item*> Node::items() {
        std::vector<Item*> all_items;
        Item* current = item_;
        while (current != nullptr) {
            all_items.push_back(current);
            current = current->NextItem();
        }
        return all_items;

    }

    std::string Tree::ToString() {
        return "I'm a tree!";
    }

    void Tree::Insert(int key, int value) {
        if(root_ == nullptr) {
            root_ = new Node(key, value);
        } else {
            root_->Insert(key, value, true);
        }
    }

    Item* Tree::Find(int key) {
        return root_->Find(key);
    }
} // namespace BTree
