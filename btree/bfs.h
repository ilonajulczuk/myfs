#ifndef BFS_H
#define BFS_H

#include <functional>
#include <queue>
#include <unordered_set>

#include "btree.h"

namespace BFS {

    void Traverse(BTree::Node* start, std::function<void(BTree::Node*)> fn);

} // namespace BFS

#endif
