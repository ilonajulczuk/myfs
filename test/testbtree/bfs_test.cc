#include "gtest/gtest.h"

#include "bfs.h"

template<typename T> void printToString(T t) {
    std::cout << t->ToString() << "\n";
}

TEST(BFSTest, TemplateThingies) {
    BTree::Tree t;
    t.Insert(5, 4);
    std::cout << "Testing template function\n";
    printToString(&t);
    printToString(t.root());
}
