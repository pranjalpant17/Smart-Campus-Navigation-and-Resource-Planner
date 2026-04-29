#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "graph.h"
#include<fstream>
class ResourceAllocator {
    CampusGraph& G;

public:

    void loadFromFile() {
    ifstream fin("resources.txt");

    if (!fin) return;

    int buildingId, occupied;
    string room;

    while (fin >> buildingId) {
        fin.ignore(); // skip space

        getline(fin, room, ' '); // read first word

        string temp;
        getline(fin, temp); // read remaining

        string full = room + " " + temp;

        int lastSpace = full.find_last_of(' ');
        occupied = stoi(full.substr(lastSpace + 1));
        string actualRoom = full.substr(0, lastSpace);

        if (!G.buildings.count(buildingId)) continue;

        auto &b = G.buildings[buildingId];

        for (auto &r : b.resources) {
            if (r->name == actualRoom) {
                r->isOccupied = occupied;
            }
        }
    }

    fin.close();
}
    void saveToFile() {
    ofstream fout("resources.txt");

    for (auto &pair : G.buildings) {
        int buildingId = pair.first;
        auto &b = pair.second;

        for (auto &r : b.resources) {
            fout << buildingId << " "
                 << r->name << " "
                 << r->isOccupied << "\n";
        }
    }

    fout.close();
}

    ResourceAllocator(CampusGraph& graph) : G(graph) {}

    void showResources(int buildingId) {
        if (!G.buildings.count(buildingId)) {
            cout << "Invalid building ID\n";
            return;
        }

        auto &b = G.buildings[buildingId];

        cout << "\nResources in " << b.name << ":\n";
        for (auto &r : b.resources)
            r->display();
    }

    void showAvailable(int buildingId) {
        if (!G.buildings.count(buildingId)) {
            cout << "Invalid building ID\n";
            return;
        }

        auto &b = G.buildings[buildingId];

        cout << "\nAvailable resources in " << b.name << ":\n";
        bool found = false;

        for (auto &r : b.resources) {
            if (!r->isOccupied) {
                r->display();
                found = true;
            }
        }

        if (!found)
            cout << "No free resources\n";
    }

    void allocate(int buildingId, int requiredCapacity) {
        if (!G.buildings.count(buildingId)) {
            cout << "Invalid building ID\n";
            return;
        }

        auto &b = G.buildings[buildingId];

        for (auto &r : b.resources) {
            if (!r->isOccupied && r->capacity >= requiredCapacity) {
                r->isOccupied = true;
                saveToFile();

                cout << "Allocated " << r->name
                     << " in " << b.name << endl;
                return;
            }
        }

        cout << "No suitable resource found in this building\n";
    }

    void deallocate(int buildingId, string roomName) {
        if (!G.buildings.count(buildingId)) {
            cout << "Invalid building ID\n";
            return;
        }

        auto &b = G.buildings[buildingId];

        for (auto &r : b.resources) {
            if (r->name == roomName) {
                if (!r->isOccupied) {
                    cout << "Already free\n";
                    return;
                }

                r->isOccupied = false;
                saveToFile();
                cout << "Deallocated " << roomName << endl;
                return;
            }
        }

        cout << "Room not found\n";
    }

    void allocateNearest(int src, int requiredCapacity) {
        vector<int> dist(G.V, INT_MAX);

        using pii = pair<int,int>;
        priority_queue<pii, vector<pii>, greater<pii>> pq;

        dist[src] = 0;
        pq.push({0, src});

        while (!pq.empty()) {
            pii top = pq.top();
            pq.pop();
            int d = top.first;
            int u = top.second; //current building

            if (d > dist[u]) continue;

            if (G.buildings.count(u)) {
                auto &b = G.buildings[u];

                for (auto &r : b.resources) {
                    if (!r->isOccupied && r->capacity >= requiredCapacity) {
                        r->isOccupied = true;
                        saveToFile();

                        cout << "\nAllocated nearest resource:\n";
                        cout << r->name << " in " << b.name << endl;
                        cout << "Distance: " << d << " metres\n";
                        return;
                    }
                }
            }

            for (auto &e : G.getNeighbours(u)) {
                int v = e.to;
                int newDist = d + e.weight;

                if (newDist < dist[v]) {
                    dist[v] = newDist;
                   // parent[v] = u;
                    pq.push({newDist, v});
                }
            }
        }

        cout << "No available resource found in entire campus\n";
    }
    void allocateLab(int buildingId, int requiredCapacity) {
    if (!G.buildings.count(buildingId)) {
        cout << "Invalid building ID\n";
        return;
    }

    auto &b = G.buildings[buildingId];

    for (auto &r : b.resources) {
        if (r->getType() == "Lab" &&
            !r->isOccupied &&
            r->capacity >= requiredCapacity) {

            r->isOccupied = true;
            saveToFile();

            cout << "Allocated LAB: " << r->name
                 << " in " << b.name << endl;
            return;
        }
    }

    cout << "No lab available in this building\n";
    }
    void allocateNearestLab(int src, int requiredCapacity) {
    vector<int> dist(G.V, INT_MAX);
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        pair<int,int> top = pq.top();
        pq.pop();
        int d = top.first;
        int u = top.second;

        if (d > dist[u]) continue;

        if (G.buildings.count(u)) {
            auto &b = G.buildings[u];

            for (auto &r : b.resources) {
                if (r->getType() == "Lab" &&
                    !r->isOccupied &&
                    r->capacity >= requiredCapacity) {

                    r->isOccupied = true;
                    saveToFile();

                    cout << "Nearest LAB allocated:\n";
                    cout << r->name << " in " << b.name << endl;
                    cout << "Distance: " << d << "m\n";
                    return;
                }
            }
        }

        for (auto &e : G.getNeighbours(u)) {
            int v = e.to;
            int nd = d + e.weight;

            if (nd < dist[v]) {
                dist[v] = nd;
                pq.push({nd, v});
            }
        }
    }

    cout << "No lab found in campus\n";
    }

};

#endif
