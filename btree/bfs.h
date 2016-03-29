#ifndef BFS_H
#define BFS_H

#include <functional>
#include <queue>
#include <unordered_set>

#include "btree.h"

namespace BFS {
    template<typename T, typename F>
    void Traverse(const T& start, F fn) {
        std::queue<T> q;
        std::unordered_set<T> seen;
        q.push(start);

        while(!q.empty()) {
            T current = q.front();
            q.pop();
            fn(current);
            for (auto n : current->children()) {
                auto search = seen.find(n);
                if(search == seen.end()) {
                    q.push(n);
                }
            }
        }
    }
} // namespace BFS

#endif
