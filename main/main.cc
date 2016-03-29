#include "btree.h"
#include "bfs.h"
#include <string>
#include <iostream>


template<typename K, typename V>
void printNode(BTree::Node<K, V>* node) {
    std::cout << node->ToString() << std::endl;
}

int main() {
    BTree::Tree<int, int> t;
    std::cout << t.ToString() << " Wee!\n";
}
