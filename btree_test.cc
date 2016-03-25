#include <functional>
#include <vector>
#include <algorithm>
#include "btree.h"
#include "bfs.h"
#include "gtest/gtest.h"

void printNode(BTree::Node* node) {
    std::cout << node->ToString() << std::endl;
}

class NodeTester {
    public:
        using Node = BTree::Node;
        NodeTester() = delete;
        NodeTester(std::vector<Node*>* nodes) {
            nodes_ = nodes;
        }
        void operator() (Node* node) {
            nodes_->push_back(node);
        }
        bool isEmpty() {
            return nodes_->empty();
        }
        int count() {
            return nodes_->size();
        }

    private:
        std::vector<Node*>* nodes_;
};

TEST(FTest, DisplayTree) {
    BTree::Tree t;
    // TODO(att):
    // Check if every node that was inserted is there.
    // Check if the traversal is correctly in order.
    // Check if the tree has correct depth. And there is no
    // node that is invalid.

    // All in the same node so far.
    t.insert(11, 1);
    t.insert(2, 3);
    t.insert(7, 2);

    // One root node and two threenodes.
    t.insert(8, 2);
    std::cout << "Traversal 1: \n";
    BFS::traverse(t.root(), printNode);

    t.insert(8, 3);
    std::cout << "Traversal 2.1: \n";
    BFS::traverse(t.root(), printNode);
    t.insert(6, 3);
    std::cout << "Traversal 2.2: \n";
    BFS::traverse(t.root(), printNode);
    t.insert(9, 3);

    // Pull up another node to root.
    std::cout << "Traversal 2.3: \n";
    BFS::traverse(t.root(), printNode);

    t.insert(7, 3);
    std::cout << "Traversal 3.1: \n";
    BFS::traverse(t.root(), printNode);
    t.insert(4, 4);

    std::cout << "Traversal 3.2: \n";
    BFS::traverse(t.root(), printNode);

    // Pull third node up to root.
    t.insert(1, 3);
    std::cout << "Traversal 4.1: \n";
    BFS::traverse(t.root(), printNode);

    // Pull root up again, previous root become nodes.
    t.insert(3, 3);
    std::cout << "Traversal 4.2: \n";
    BFS::traverse(t.root(), printNode);
}

TEST(FTest, InsertAndFindInTree) {
    BTree::Tree t;
    std::vector<std::pair<int, int>> key_to_val = {{11, 1}, {2, 3}, {8, 2}, {7, 3}};

    for (const auto& pair : key_to_val) {
        t.insert(pair.first, pair.second);
    }

    for (const auto& pair : key_to_val) {
        auto found = t.find(pair.first);
        EXPECT_TRUE(found != nullptr);
        EXPECT_EQ(found->value(), pair.second);
    }
}

class ItemTester {
    // I think that relying on copy of the pointer to items_
    // when I pass the tester as std::function is rather hacky,
    // however using reference to std::function didn't work
    // and replacing the std::function with template also didn't
    // work and I don't really know why? oO
    public:
        using Item = BTree::Item;
        ItemTester() = delete;
        ItemTester(std::vector<Item*>* items) {
            items_ = items;
        }
        void operator() (Item* item) {
            items_->push_back(item);
        }
        bool isEmpty() {
            return items_->empty();
        }
        bool areSorted() {
            return std::is_sorted(
                    items_->begin(),
                    items_->end(),
                    [](Item* i1, Item* i2) {
                        return i1->key() < i2->key(); });
        }

    private:
        std::vector<Item*>* items_;
};

TEST(FTest, TestTraversingInOrder) {
    BTree::Tree t;
    std::vector<std::pair<int, int>> key_to_val = {{11, 1}, {2, 3}, {8, 2}, {7, 3}};

    for (const auto& pair : key_to_val) {
        t.insert(pair.first, pair.second);
    }
    BFS::traverse(t.root(), printNode);
    std::vector<BTree::Item*> items;
    ItemTester tester(&items);
    t.root()->traverse(tester);
    EXPECT_TRUE(!tester.isEmpty());
    EXPECT_TRUE(tester.areSorted());
}

GTEST_API_ int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
