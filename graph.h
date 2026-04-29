#ifndef GRAPH_H
#define GRAPH_H

#include "models.h"
#include <vector>
#include <unordered_map>
#include <climits>
#include <iostream>
#include <iomanip>
using namespace std;

class CampusGraph {
public:
    int V;                                      // No of ver
    vector<vector<Edge>>  adjList;              // Adj List
    vector<vector<int>>   adjMatrix;            // Adj Matrix
    unordered_map<int, Building> buildings;

    CampusGraph(int v)
        : V(v),
          adjList(v),
          adjMatrix(v, vector<int>(v, INT_MAX))
    {
        for (int i = 0; i < v; i++)
            adjMatrix[i][i] = 0;
    }

    void addBuilding(const Building& b) {
        buildings[b.id] = b;
    }

    void addEdge(int u, int v, int weight,
                 bool accessible = true,
                 const string& pathType = "footpath")
    {
        adjList[u].push_back(Edge(v, weight, accessible, pathType));
        adjList[v].push_back(Edge(u, weight, accessible, pathType));
        adjMatrix[u][v] = weight;
        adjMatrix[v][u] = weight;
    }

    void printAdjList() const {
        cout << "\n╔══════════════════════════════════════════════════╗\n";
        cout << "║        CAMPUS GRAPH — ADJACENCY LIST             ║\n";
        cout << "╚══════════════════════════════════════════════════╝\n";

        for (int i = 0; i < V; i++) {
            if (!buildings.count(i)) continue;
            cout << "  [" << setw(2) << i << "] "
                 << left << setw(20) << buildings.at(i).name
                 << " ──► ";
            for (auto& e : adjList[i]) {
                if (buildings.count(e.to))
                    cout << buildings.at(e.to).name
                         << "(" << e.weight << "m"
                         << (e.isAccessible ? "" : ",NA")
                         << ") ";
            }
            cout << "\n";
        }
        cout << "  NA = Not wheelchair accessible\n";
    }


void printAdjMatrix() const {
    int limit = min(V, 8);

    cout << "\n╔══════════════════════════════════════════════════╗\n";
        cout << "║        CAMPUS GRAPH — ADJACENCY MATRIX           ║\n";
        cout << "╚══════════════════════════════════════════════════╝\n";

    cout << setw(22) << " ";
    for (int j = 0; j < limit; j++)
        cout << right << setw(6) << j;
    cout << "\n";

    cout << string(22 + limit * 6, '-') << "\n";

    for (int i = 0; i < limit; i++) {
        string name = buildings.count(i) ? buildings.at(i).name : "?";

        if (name.length() > 20)
            name = name.substr(0, 19) + ".";

        cout << left << setw(22) << name;

        for (int j = 0; j < limit; j++) {
            if (adjMatrix[i][j] == INT_MAX)
                cout << right << setw(6) << "INF"; 
            else
                cout << right << setw(6) << adjMatrix[i][j];
        }
        cout << "\n";
    }
}

    const vector<Edge>& getNeighbours(int node) const {
        return adjList[node];
    }

    int edgeWeight(int u, int v) const {
        return adjMatrix[u][v];
    }

    string buildingName(int id) const {
        return buildings.count(id) ? buildings.at(id).name : "?";
    }
};

#endif
