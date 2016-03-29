#include "btree.h"
#include <iostream>


int main() {
    BTree::Tree<int, int> t;
    std::cout << t.ToString() << " Wee!\n";
}
