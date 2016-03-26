#ifndef BFS_H
#define BFS_H

#include <queue>
#include <unordered_set>
#include <functional>
#include "btree.h"

/* TODO(att):
 * - for breadth first search I need a queue. How can I achieve it in C++?, std::queue
 * - should I write a generic enough bfs?
 */

namespace BFS {

    void traverse(BTree::Node* start, std::function<void(BTree::Node*)> fn);

} // namespace BFS

#endif
