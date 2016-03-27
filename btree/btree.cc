#include <algorithm>
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

    std::vector<Node*> Node::Adjacent(Node* node) {
        auto all_nodes = children();
        auto it = std::find(all_nodes.begin(), all_nodes.end(), node);
        auto pos = std::distance(all_nodes.begin(), it);
        std::vector<Node*> adjacent;
        if (pos > 0) {
            adjacent.push_back(all_nodes[pos - 1]);
        }
        if (pos < all_nodes.size() - 1) {
            adjacent.push_back(all_nodes[pos + 1]);
        }
        return adjacent;
    }

    Item* Node::GetLeftParentItem() {
        int first_key = item_->key();
        auto all_items = parent_->items();
        Item* previous = nullptr;
        for (auto item : all_items) {
           if (item->key() >  first_key) {
               return previous;
           }
           previous = item;
        }
        return previous;
    }

    Item* Node::GetRightParentItem() {
        int first_key = item_->key();
        auto all_items = parent_->items();
        for (auto item : all_items) {
           if (item->key() >  first_key) {
               return item;
           }
        }
        return nullptr;
    }

    void Node::Delete(int key, Item* to_replace) {
        // Eliminate 1-key nodes. (other than the root).
        // 1. tries to steal a key from one of its adjacent siblings (nodes)
        // and then perform a simple rotation.
        if (items().size() == 1) {
            // Not root.
            if (parent_ != nullptr) {
                auto siblings = parent_->Adjacent(this);
                bool rotated = false;
                for (auto sibling : siblings) {
                    auto sibling_size = sibling->items().size();
                    if (sibling_size > 1) {
                        // perform rotation
                        // it's either rotation from left or right
                        if (sibling->items()[0]->key() < item_->key()) {
                            Item* last = sibling->items()[sibling_size - 1];
                            Item* before_last = sibling->items()[sibling_size - 2];
                            before_last->SetNext(nullptr);
                            Item* left_parent_item = GetLeftParentItem();
                            Item* stolen = new Item(left_parent_item->key(), left_parent_item->value());
                            left_parent_item->SetKey(last->key());
                            left_parent_item->SetValue(last->value());

                            stolen->SetNext(item_);
                            stolen->SetLeft(last->right());
                            stolen->SetRight(item_->left());
                            item_ = stolen;
                            delete last;
                        } else {
                            Item* first = sibling->item_;
                            Item* right_parent_item = GetRightParentItem();
                            Item* stolen = new Item(right_parent_item->key(), right_parent_item->value());
                            right_parent_item->SetKey(first->key());
                            right_parent_item->SetValue(first->value());
                            stolen->SetRight(first->left());
                            stolen->SetLeft(item_->right());
                            item_->SetNext(stolen);
                            sibling->item_ = first->NextItem();
                            delete first;
                        }
                        rotated = true;
                        break;
                    }
                }
                // if not rotated, try something else
            }
        }

        if (!IsLeaf()) {
            Item* previous = nullptr;
            Item* current = item_;
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
            Item* previous = nullptr;
            Item* current = item_;
            if (to_replace != nullptr) {
                to_replace->SetKey(current->key());
                to_replace->SetValue(current->key());
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

    void Tree::Delete(int key) {
        return root_->Delete(key);
    }
} // namespace BTree
