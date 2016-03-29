#include "bfs.h"

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
    /*
    void Traverse(BTree::Node* start, std::function<void(BTree::Node*)> fn) {
        std::queue<BTree::Node*> q;
        std::unordered_set<BTree::Node*> seen;
        q.push(start);

        while(!q.empty()) {
            BTree::Node* current = q.front();
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
    */
} // namespace BFS
