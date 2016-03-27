#include <algorithm>
#include <functional>
#include <vector>
#include <map>

#include "bfs.h"
#include "btree.h"
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
        int treeDepth() {
            int max_depth = 0;
            int current_depth = 1;
            Node* parent = nullptr;
            for(auto node : *nodes_) {
                current_depth = 1;
                parent = node->parent();
                while(parent != nullptr) {
                    current_depth++;
                    parent = parent->parent();
                }
                if (current_depth > max_depth) {
                    max_depth = current_depth;
                }
            }
            return max_depth;
        }

    private:
        std::vector<Node*>* nodes_;
};

TEST(FTest, DisplayTree) {
    BTree::Tree t;
    std::vector<BTree::Node*> nodes;
    NodeTester tester(&nodes);

    // All in the same node so far.
    t.Insert(11, 1);
    t.Insert(2, 3);
    t.Insert(7, 2);

    BFS::Traverse(t.root(), tester);
    EXPECT_TRUE(!tester.isEmpty());
    EXPECT_EQ(tester.count(), 1);
    EXPECT_EQ(tester.treeDepth(), 1);
    nodes.clear();

    // One root node and two threenodes.
    t.Insert(8, 2);
    std::cout << "Traversal 1: \n";
    BFS::Traverse(t.root(), printNode);

    BFS::Traverse(t.root(), tester);
    EXPECT_TRUE(!tester.isEmpty());
    EXPECT_EQ(tester.count(), 3);
    EXPECT_EQ(tester.treeDepth(), 2);
    nodes.clear();

    t.Insert(8, 3);
    std::cout << "Traversal 2.1: \n";
    BFS::Traverse(t.root(), printNode);
    t.Insert(6, 3);
    std::cout << "Traversal 2.2: \n";
    BFS::Traverse(t.root(), printNode);
    t.Insert(9, 3);

    // Pull up another node to root.
    std::cout << "Traversal 2.3: \n";
    BFS::Traverse(t.root(), printNode);

    BFS::Traverse(t.root(), tester);
    EXPECT_TRUE(!tester.isEmpty());
    EXPECT_EQ(tester.count(), 4);
    EXPECT_EQ(tester.treeDepth(), 2);
    nodes.clear();

    t.Insert(7, 3);
    std::cout << "Traversal 3.1: \n";
    BFS::Traverse(t.root(), printNode);
    t.Insert(4, 4);

    std::cout << "Traversal 3.2: \n";
    BFS::Traverse(t.root(), printNode);

    // Pull third node up to root.
    t.Insert(1, 3);
    std::cout << "Traversal 4.1: \n";
    BFS::Traverse(t.root(), printNode);

    BFS::Traverse(t.root(), tester);
    EXPECT_TRUE(!tester.isEmpty());
    EXPECT_EQ(tester.count(), 5);
    EXPECT_EQ(tester.treeDepth(), 2);
    nodes.clear();

    // Pull root up again, previous root become nodes.
    t.Insert(3, 3);
    std::cout << "Traversal 4.2: \n";
    BFS::Traverse(t.root(), printNode);
    BFS::Traverse(t.root(), tester);
    EXPECT_TRUE(!tester.isEmpty());
    EXPECT_EQ(tester.count(), 7);
    EXPECT_EQ(tester.treeDepth(), 3);
    nodes.clear();
}

TEST(FTest, InsertAndFindInTree) {
    BTree::Tree t;
    std::vector<std::pair<int, int>> key_to_val = {
        {11, 1}, {2, 3}, {3, 4}, {4, 5}, {5, 6},
        {6, 7}, {7, 8}, {8, 2}, {10, 3}, {9, 9}
    };

    for (const auto& pair : key_to_val) {
        t.Insert(pair.first, pair.second);
    }

    for (const auto& pair : key_to_val) {
        auto found = t.Find(pair.first);
        EXPECT_NE(nullptr, found);
        EXPECT_EQ(found->value(), pair.second);
    }
}

TEST(FTest, DeleteFromLeafNode) {
    std::vector<std::pair<int, int>> key_to_val = {
        {3, 4}, {4, 5}, {5, 6},
    };

    for (const auto& to_delete : key_to_val) {
        BTree::Tree t;
        for (const auto& pair : key_to_val) {
            t.Insert(pair.first, pair.second);
        }
        t.Delete(to_delete.first);
        for (const auto& pair : key_to_val) {
            auto found = t.Find(pair.first);
            if (pair.first == to_delete.first) {
                EXPECT_EQ(nullptr, found);
            } else {
                EXPECT_NE(nullptr, found);
                EXPECT_EQ(found->value(), pair.second);
            }
        }
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

    std::vector<std::pair<int, int>> key_to_val = {
        {11, 1}, {2, 3}, {3, 4}, {4, 5}, {5, 6},
        {6, 7}, {7, 8}, {8, 2}, {10, 3}, {9, 9}
    };
    for (const auto& pair : key_to_val) {
        t.Insert(pair.first, pair.second);
    }
    BFS::Traverse(t.root(), printNode);
    std::vector<BTree::Item*> items;
    ItemTester tester(&items);
    t.root()->Traverse(tester);
    EXPECT_TRUE(!tester.isEmpty());
    EXPECT_TRUE(tester.areSorted());
}

TEST(FTest, TestAdjacent) {
    // TODO(att): add validity check to tester, so that it checks for
    // nodes with zero or too many items, or items out of order.
    // test if items that weren't supposed to be deleted are still there
    // and that key match values.
    BTree::Tree t;
    std::vector<BTree::Node*> nodes;
    NodeTester tester(&nodes);

    t.Insert(7, 3);
    t.Insert(8, 3);
    t.Insert(4, 2);
    t.Insert(5, 3);
    t.Insert(6, 3);
    t.Insert(9, 4);
    t.Insert(3, 2);
    t.Insert(10, 4);
    t.Insert(11, 4);

    BFS::Traverse(t.root(), tester);
    EXPECT_TRUE(!tester.isEmpty());
    EXPECT_EQ(tester.count(), 5);
    EXPECT_EQ(tester.treeDepth(), 2);

    auto children = t.root()->children();
    auto A = children[0];
    auto B = children[1];
    auto C = children[2];
    auto D = children[3];

    std::map<BTree::Node*, std::vector<BTree::Node*>> adjacency_map =
        { {A, {B}},  {B, {A, C}}, {C, {B, D}}, {D, {C}} };

    for (auto child : t.root()->children()) {
        auto adjacent = t.root()->Adjacent(child);
        auto expected = adjacency_map[child];
        EXPECT_EQ(adjacent, expected);
    }

    BFS::Traverse(t.root(), printNode);
    t.Delete(6);
    EXPECT_EQ(t.Find(6), nullptr);
    BFS::Traverse(t.root(), printNode);
    t.Delete(8);
    EXPECT_EQ(t.Find(8), nullptr);

    BFS::Traverse(t.root(), printNode);
    t.Delete(7);
    EXPECT_EQ(t.Find(7), nullptr);

    BFS::Traverse(t.root(), printNode);
}

TEST(FTest, TestMergingWithRoot) {
    BTree::Tree t;
    std::vector<BTree::Node*> nodes;
    NodeTester tester(&nodes);

    t.Insert(1, 2);
    t.Insert(2, 2);
    t.Insert(2, 3);
    t.Insert(3, 2);
    t.Insert(4, 2);
    t.Insert(5, 3);
    t.Insert(6, 3);
    t.Insert(7, 3);
    t.Insert(8, 3);

    BFS::Traverse(t.root(), tester);
    EXPECT_TRUE(!tester.isEmpty());
    EXPECT_EQ(tester.count(), 7);
    EXPECT_EQ(tester.treeDepth(), 3);

    BFS::Traverse(t.root(), printNode);

    t.Delete(6);
    EXPECT_EQ(t.Find(6), nullptr);
    BFS::Traverse(t.root(), printNode);
}

GTEST_API_ int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
