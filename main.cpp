#include "models.h"
#include "graph.h"
#include "bfs.h"
#include "dfs.h"
#include "dijkstra.h"
#include "campus_data.h"
#include "allocator.h"

#include <iostream>
using namespace std;

void showBuildingList(const CampusGraph& G) {
    cout << "\n--- Campus Building IDs ---\n";
    for (int i = 0; i < G.V; i++)
        if (G.buildings.count(i))
            cout << "  " << i << " : " << G.buildings.at(i).name << "\n";
}

void menu() {
    cout << "\n===== SMART CAMPUS NAVIGATION =====\n";
    cout << "1. Show Graph\n";
    cout << "2. BFS (Minimum Hops)\n";
    cout << "3. DFS (20 Paths)\n";
    cout << "4. Dijkstra (Shortest Distance)\n";
    cout << "5. Show Resources\n";
    cout << "6. Allocate Classroom\n";
    cout << "7. Deallocate\n";
    cout << "8. Show Available\n";
    cout << "9. Nearest Classroom Allocation\n";
    cout << "10. Allocate Lab\n";
    cout << "11. Allocate Nearest Lab\n";
    cout << "12. Exit\n";
    cout << "Enter choice: ";
}

int main() {

    system("chcp 65001");

    CampusGraph G = buildCampusGraph();
    ResourceAllocator ra(G);

    ra.loadFromFile(); 

    int choice;

    do {
        menu();
        cin >> choice;

        if (choice == 1) {
            G.printAdjList();
            G.printAdjMatrix();
        }

        else if (choice == 2) {
            showBuildingList(G);
            int src, dest;
            cout << "Enter source and destination: ";
            cin >> src >> dest;

            BFSModule bfs(G);
            bfs.run(src, dest);
        }

        else if (choice == 3) {
            showBuildingList(G);
            int src, dest;
            cout << "Enter source and destination: ";
            cin >> src >> dest;

            DFSModule dfs(G);
            dfs.run(src, dest);
        }

        else if (choice == 4) {
            showBuildingList(G);
            int src, dest;
            cout << "Enter source and destination: ";
            cin >> src >> dest;

            DijkstraModule dijk(G);
            dijk.run(src, dest, false);
        }

        else if (choice == 5) {
            showBuildingList(G);
            int id;
            cout << "Enter building ID: ";
            cin >> id;
            ra.showResources(id);
        }

        else if (choice == 6) {
            showBuildingList(G);
            int id, cap;
            cout << "Enter building ID and required capacity: ";
            cin >> id >> cap;
            ra.allocate(id, cap);
        }

        else if (choice == 7) {
            showBuildingList(G);
            int id;
            string name;
            cout << "Enter building ID and room name: ";
            cin >> id >> name;
            ra.deallocate(id, name);
        }

        else if (choice == 8) {
            showBuildingList(G);
            int id;
            cout << "Enter building ID: ";
            cin >> id;
            ra.showAvailable(id);
        }

        else if (choice == 9) {
            showBuildingList(G);
            int src, cap;
            cout << "Enter source building and required capacity: ";
            cin >> src >> cap;
            ra.allocateNearest(src, cap);
        }

        else if (choice == 10) {
            showBuildingList(G);
            int id, cap;
            cout << "Enter building ID and capacity: ";
            cin >> id >> cap;
            ra.allocateLab(id, cap);
        }

        else if (choice == 11) {
            showBuildingList(G);
            int src, cap;
            cout << "Enter source and capacity: ";
            cin >> src >> cap;
            ra.allocateNearestLab(src, cap);
        }

        else if (choice == 12) {
            cout << "\nGoodbye!\n";
        }

        else {
            cout << "Invalid choice!\n";
        }

    } while (choice != 12);

    return 0;
}
