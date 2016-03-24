#include "bfs.h"
#include "gtest/gtest.h"

template<typename T> void printToString(T t) {
    std::cout << t->ToString() << "\n";
}

TEST(FTest, TemplateThingies) {
    BTree::Tree t;
    t.insert(5, 4);
    std::cout << "Testing template function\n";
    printToString(&t);
    printToString(t.root());

}
