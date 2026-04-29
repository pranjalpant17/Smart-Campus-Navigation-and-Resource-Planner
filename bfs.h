
#ifndef BFS_H
#define BFS_H

#include "graph.h"
#include <queue>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

class BFSModule {
    const CampusGraph& G;

public:
    explicit BFSModule(const CampusGraph& graph) : G(graph) {}

    vector<int> run(int src, int dest) {
        vector<bool> visited(G.V, false);
        vector<int>  parent(G.V, -1);
        queue<int>   q;

        cout << "\n┌─────────────────────────────────────────────────────┐\n";
        cout << "│  From  : " << left << setw(43) << G.buildingName(src) << "│\n";
        cout << "│  To    : " << left << setw(43) << G.buildingName(dest) << "│\n";
        cout << "└─────────────────────────────────────────────────────┘\n";

        visited[src] = true;
        q.push(src);

        cout << "  Queue progression:\n  ";

        while (!q.empty()) {
            int u = q.front(); q.pop();

            cout << G.buildingName(u);

            if (u == dest) {
                cout << " [DESTINATION REACHED]\n";
                break;
            }
            cout << " → ";

            for (auto& e : G.getNeighbours(u)) {
                if (!visited[e.to]) {
                    visited[e.to]  = true;
                    parent[e.to]   = u;
                    q.push(e.to);
                }
            }
        }

        vector<int> path;
        for (int cur = dest; cur != -1; cur = parent[cur])
            path.push_back(cur);

        reverse(path.begin(), path.end());

        if (path.empty() || path[0] != src) {
            cout << "\n  ✗ No path exists between "
                 << G.buildingName(src) << " and "
                 << G.buildingName(dest) << "\n";
            return {};
        }

        printPath(path);
        return path;
    }

private:

    void printPath(const vector<int>& path) const {
        cout << "\n  ✓ Shortest-hop path:\n    ";
        for (int i = 0; i < (int)path.size(); i++) {
            cout << G.buildingName(path[i]);
            if (i + 1 < (int)path.size()) cout << "  →  ";
        }
        cout << "\n  Total hops: " << path.size() - 1 << "\n";
    }
};

#endif
