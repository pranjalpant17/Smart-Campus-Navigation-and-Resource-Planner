
#ifndef DFS_H
#define DFS_H

#include "graph.h"
#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

class DFSModule {
    const CampusGraph& G;
    const size_t MAX_PATHS = 20;
public:
    explicit DFSModule(const CampusGraph& graph) : G(graph) {}

    vector<vector<int>> run(int src, int dest) {
         cout << "\n┌─────────────────────────────────────────────────────┐\n";
        cout << "│  From  : " << left << setw(43) << G.buildingName(src) << "│\n";
        cout << "│  To    : " << left << setw(43) << G.buildingName(dest) << "│\n";
        cout << "└─────────────────────────────────────────────────────┘\n";

        vector<bool>         visited(G.V, false);
        vector<int>          currentPath;
        vector<vector<int>>  allPaths;

        dfsRecursive(src, dest, visited, currentPath, allPaths);

        printAllPaths(allPaths);
        return allPaths;
    }

private:
    void dfsRecursive(int node, int dest,
                      vector<bool>&        visited,
                      vector<int>&         currentPath,
                      vector<vector<int>>& allPaths)
    {
        if (allPaths.size() >= MAX_PATHS) return;

        visited[node] = true;
        currentPath.push_back(node);

        if (node == dest) {
            allPaths.push_back(currentPath);   // store path
        } else {
            for (auto& e : G.getNeighbours(node)) {
                if (!visited[e.to]) {
                    dfsRecursive(e.to, dest, visited, currentPath, allPaths);
                }
            }
        }

        currentPath.pop_back();
        visited[node] = false;
    }


    void printAllPaths(const vector<vector<int>>& allPaths) const {
        if (allPaths.empty()) {
            cout << "  ✗ No paths found.\n";
            return;
        }

        cout << "\n  Found " << allPaths.size() << " path(s) (max 20):\n\n";

        int idx = 1;
        for (auto& path : allPaths) {
            cout << "  Path " << setw(2) << idx++ << " ("
                 << path.size() - 1 << " hops): ";

            for (int i = 0; i < (int)path.size(); i++) {
                cout << G.buildingName(path[i]);
                if (i + 1 < (int)path.size()) cout << " ──► ";
            }
            cout << "\n";
        }
    }
};

#endif
