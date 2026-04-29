#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <iostream>
#include <iomanip>
using namespace std;

using pii = pair<int, int>;

class DijkstraModule {
    const CampusGraph& G;

public:
    explicit DijkstraModule(const CampusGraph& graph) : G(graph) {}

    pair<vector<int>, int> run(int src, int dest,
                               bool accessibleOnly = false) {
        vector<int> dist(G.V, INT_MAX);
        vector<int> parent(G.V, -1);

        priority_queue<pii, vector<pii>, greater<pii>> minHeap;

        dist[src] = 0;
        minHeap.push({0, src});

         cout << "\n┌─────────────────────────────────────────────────────┐\n";
        cout << "│  From  : " << left << setw(43) << G.buildingName(src) << "│\n";
        cout << "│  To    : " << left << setw(43) << G.buildingName(dest) << "│\n";
        cout << "└─────────────────────────────────────────────────────┘\n";

        while (!minHeap.empty()) {
            auto temp = minHeap.top();
            int d = temp.first;
            int u = temp.second;
            minHeap.pop();

            if (d > dist[u]) continue;

            if (u == dest) break;

            for (auto& e : G.getNeighbours(u)) {
                if (accessibleOnly && !e.isAccessible) continue;

                int newDist = dist[u] + e.weight;
                if (newDist < dist[e.to]) {
                    dist[e.to]   = newDist;
                    parent[e.to] = u;
                    minHeap.push({newDist, e.to});
                }
            }
        }
          
        vector<int> path;
        for (int cur = dest; cur != -1; cur = parent[cur])
            path.push_back(cur);
        reverse(path.begin(), path.end());

        if (path.empty() || path[0] != src) {
            cout << "  ✗ No "
                 << (accessibleOnly ? "accessible " : "")
                 << "path found from "
                 << G.buildingName(src) << " to "
                 << G.buildingName(dest) << "\n";
            return {{}, INT_MAX};
        }

        printPath(path, dist[dest]);

        return {path, dist[dest]};
    }

private:

    void printPath(const vector<int>& path, int totalDist) const {
        cout << "\n  ✓ Shortest path:\n    ";
        for (int i = 0; i < (int)path.size(); i++) {
            cout << G.buildingName(path[i]);
            if (i + 1 < (int)path.size()) {
                int w = G.edgeWeight(path[i], path[i + 1]);
                cout << "  ──(" << w << "m)──►  ";
            }
        }
        cout << "\n  Total distance: " << totalDist << " metres\n";
    }

};

#endif 
