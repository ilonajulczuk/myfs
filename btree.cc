#include "btree.h"

/* TODO(att):
   - write good enough traversing that would make debugging possible
   - add tests
   - add deletion case
   */
namespace BTree {

    Node::Node(int key, int value) {
        item_ = new Item(key, value);
    }

    Node::Node(Item* item, Node* parent, bool isLeaf) {
        parent_ = parent;
        item_ = item;
        isLeaf_ = isLeaf;
        Item* current = item;
        Node *childNode;
        while(current != nullptr) {
            childNode = item->right();
            if (childNode != nullptr) {
                childNode->setParent(this);
            }
            childNode = item->left();
            if (childNode != nullptr) {
                childNode->setParent(this);
            }
            current = current->nextItem();
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

    Item* Node::find(int key) {
        Item* current = item_;
        while (current != nullptr) {
            if (key == current->key()) {
                return current;
            }
            if (key < current->key()) {
                if (!isLeaf_) {
                    return current->left()->find(key);
                } else {
                    return nullptr;
                }
            }
            Item* next = current->nextItem();
            if (next == nullptr && !isLeaf_) {
                return current->right()->find(key);
            }
            current = next;
        }
        return nullptr;
    }

    bool Node::assureNotFourNode() {
        Item* current = item_;
        int total = 0;
        while (current != nullptr) {
            total++;
            current = current->nextItem();
        }

        if (total != 3) {
            return false;
        }

        Item* middle = item_->nextItem();
        Node* newLeft = nullptr;
        Node* newRight = nullptr;

        if (parent_ == nullptr) {
            Item* rightItem = item_->nextItem()->nextItem();
            newRight = new Node(rightItem, this, isLeaf_);
            item_->setNext(nullptr);
            newLeft = new Node(item_, this, isLeaf_);
            isLeaf_ = false;
            item_ = middle;
            middle->setNext(nullptr);
            middle->setRight(newRight);
            middle->setLeft(newLeft);
            return false;
        }

        // find the correct place to insert middle;
        // middle gets previous item in node as left child and next as right
        // previous node up, gets left child as right child, and next node up, gets right child as left;


        Item* previous = nullptr;
        current = parent_->item_;
        while (current != nullptr) {
            if (middle->key() < current->key()) {
                break;
            }
            previous = current;
            current = current->nextItem();
        }

        Item* rightItem = item_->nextItem()->nextItem();
        newRight = new Node(rightItem, parent_, isLeaf_);
        newLeft = this;
        item_->setNext(nullptr);

        middle->setLeft(newLeft);
        middle->setNext(nullptr);
        middle->setRight(newRight);

        if (previous == nullptr) {
            parent_->setItem(middle);
        } else {
            previous->setRight(newLeft);
            previous->setNext(middle);
        }

        if (current != nullptr) {
            middle->setNext(current);
            current->setLeft(newRight);
        }
        return true;
    }


    void Node::insert(int key, int value, bool assure) {
        // make sure that the node has less than three items.
        if (assure) {
            bool changed = assureNotFourNode();
            if (changed) {
                return parent_->insert(key, value, false);
            }
        }

        Item* previous = nullptr;
        Item* current = item_;
        while (current != nullptr) {
            if (key < current->key()) {
                if (isLeaf_) {
                    Item* item = new Item(key, value);
                    // check if we're inserting before first item.
                    if (previous == nullptr) {
                        item->setNext(current);
                        item_ = item;
                        return;
                    } else {
                        item->setNext(current);
                        previous->setNext(item);
                        return;
                    }
                }
                return current->left()->insert(key, value, true);
            } else {
                Item* next = current->nextItem();
                if (next == nullptr) {
                    if (isLeaf_) {
                        Item* item = new Item(key, value);
                        current->setNext(item);
                        return;
                    } else {
                        return current->right()->insert(key, value, true);
                    }
                }
                previous = current;
                current = next;
            }
        }
    }

    std::vector<Node*> Node::children() {
        std::vector<Node*> nodes;
        if (isLeaf_) {
            return nodes;
        }

        Item* current = item_;
        nodes.push_back(current->left());
        while (current != nullptr) {
            nodes.push_back(current->right());
            Item* next = current->nextItem();
            current = next;
        }
        return nodes;
    }

    std::vector<Item*> Node::items() {
        std::vector<Item*> allItems;
        Item* current = item_;
        while (current != nullptr) {
            allItems.push_back(current);
            current = current->nextItem();
        }
        return allItems;

    }

    std::string Tree::ToString() {
        return "I'm a tree!";
    }

    void Tree::insert(int key, int value) {
        if(root_ == nullptr) {
            root_ = new Node(key, value);
        } else {
            root_->insert(key, value, true);
        }
    }

    Item* Tree::find(int key) {
        return root_->find(key);
    }
} // namespace BTree


