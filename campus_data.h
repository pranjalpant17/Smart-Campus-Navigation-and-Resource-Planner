#ifndef CAMPUS_DATA_H
#define CAMPUS_DATA_H

#include "graph.h"
#include "models.h"
#include <vector>
using namespace std;

CampusGraph buildCampusGraph() {
    CampusGraph G(30);

    // ───── BUILDINGS (VERTICES) ─────
    G.addBuilding(Building(0,  "Main Gate: 1"));
    G.addBuilding(Building(1,  "Gate: 2"));
    G.addBuilding(Building(2,  "Gate: 3"));

    G.addBuilding(Building(3,  "B.Tech Block"));
    G.addBuilding(Building(4,  "CSIT Block"));
    G.addBuilding(Building(5,  "Paramedical Block"));
    G.addBuilding(Building(6,  "Old MCA Block"));
    G.addBuilding(Building(7,  "Mechanical Block"));
    G.addBuilding(Building(8,  "GEU Block"));
    G.addBuilding(Building(9,  "Chanakya Block"));

    G.addBuilding(Building(10,  "Santoshanand Library"));

    G.addBuilding(Building(11,  "Param Lab"));
    G.addBuilding(Building(12,  "Aryabhatt Lab"));

    G.addBuilding(Building(13,  "Quick Bite Cafeteria"));
    G.addBuilding(Building(14,  "Ravi Canteen"));
    G.addBuilding(Building(15,  "Tuck Shop: 1"));
    G.addBuilding(Building(16,  "Tuck Shop: 2"));

    G.addBuilding(Building(17,  "Badminton Court"));
    G.addBuilding(Building(18,  "Basketball Court"));
    G.addBuilding(Building(19,  "Main Ground"));

    G.addBuilding(Building(20,  "CSA Hostel"));
    G.addBuilding(Building(21,  "SP Hostel"));
    G.addBuilding(Building(22, "Laxmi Bai Hostel"));

    G.addBuilding(Building(23, "Medical Room"));
    
    G.addBuilding(Building(24, "Parking : 1(Gate 2)"));
    G.addBuilding(Building(25, "Parking : 2(CSIT)"));

    G.addBuilding(Building(26,  "Convention Centre"));

    G.addBuilding(Building(27,  "Happiness Cafeteria"));

    G.addBuilding(Building(28,  "Civil Block"));

    G.addBuilding(Building(29,  "GYM"));

    // ───── ADD RESOURCES ─────

    auto &csit = G.buildings[4];
    for(int i=1;i<=12;i++) csit.addResource(make_shared<Classroom>("CR"+to_string(i),80));
    for(int i=1;i<=11;i++) csit.addResource(make_shared<Classroom>("LT"+to_string(i),180));
    for(int i=1;i<=3;i++) csit.addResource(make_shared<Lab>("Lab-"+to_string(i),90));
    for(int i=4;i<=7;i++) csit.addResource(make_shared<Lab>("Lab-"+to_string(i),50));
    csit.addResource(make_shared<Lab>("Lab8A",55));
    csit.addResource(make_shared<Lab>("Lab8B",55));

    auto &btech = G.buildings[3];
    for(int i=13;i<=18;i++) btech.addResource(make_shared<Classroom>("CR"+to_string(i),80));
    for(int i=12;i<=15;i++) btech.addResource(make_shared<Classroom>("LT"+to_string(i),220));
    for(int i=1;i<=4;i++) btech.addResource(make_shared<Lab>("Electronics Lab "+to_string(i),40));

    auto &civil = G.buildings[28];
    for(int i=19;i<=24;i++) civil.addResource(make_shared<Classroom>("CR"+to_string(i),80));
    for(int i=1;i<=2;i++) civil.addResource(make_shared<Lab>("Civil Lab "+to_string(i),40));

    auto &geu = G.buildings[8];
    for(int i=26;i<=31;i++) geu.addResource(make_shared<Classroom>("CR"+to_string(i),80));
    for(int i=39;i<=43;i++) geu.addResource(make_shared<Classroom>("CR"+to_string(i),80));

    auto &mca = G.buildings[6];
    for(int i=32;i<=38;i++) mca.addResource(make_shared<Classroom>("CR"+to_string(i),60));
    for(int i=44;i<=48;i++) mca.addResource(make_shared<Classroom>("CR"+to_string(i),60));

    auto &lib = G.buildings[10];
    for(int i=1;i<=2;i++) lib.addResource(make_shared<Lab>("Digital Lab "+to_string(i),90));

    auto &mech = G.buildings[7];
    for(int i=1;i<=4;i++) mech.addResource(make_shared<Lab>("Mechanical Lab "+to_string(i),40));
    mech.addResource(make_shared<Lab>("CAD Lab",90));

    auto &param = G.buildings[11];
    for(int i=1;i<=3;i++) param.addResource(make_shared<Lab>("Lab "+to_string(i),90));

    auto &aryabhatt = G.buildings[12];
    for(int i=1;i<=3;i++) aryabhatt.addResource(make_shared<Lab>("Lab "+to_string(i),90));

    // ───── EDGES (CONNECTIONS) ─────
    G.addEdge(0, 23, 10, true,  "road");   // Main Gate → Medical Room
    G.addEdge(0, 19, 120, false, "road");  // Main Gate → Main Ground
    G.addEdge(0, 1, 450, true, "road");    // Main Gate → Gate 2
    G.addEdge(0, 2, 100, true,  "road");   // Main Gate → Gate 3
    G.addEdge(0, 26, 50, true,  "road");   // Main Gate → Convention Centre

    G.addEdge(1, 24, 10, true,  "road");   // Gate 2 → Parking 1

    G.addEdge(2, 4, 20, true,   "road");   // Gate 3 → CSIT Block
    G.addEdge(2, 22, 90, true,  "road");   // Gate 3 → Laxmi Bai Hostel

    G.addEdge(3, 19, 120, true, "road");   // B.Tech → Main Ground
    G.addEdge(3, 14, 100, true, "road");   // B.Tech → Ravi Canteen
    G.addEdge(3, 27, 60, true,  "road");   // B.Tech → Happiness Cafeteria
    G.addEdge(3, 17, 30, true,  "road");   // B.Tech → Badminton Court
    G.addEdge(3, 10, 20, true,  "road");   // B.Tech → Library
    G.addEdge(3, 12, 40, true,  "road");   // B.Tech → Aryabhatt Lab
    G.addEdge(3, 15, 100, true, "road");   // B.Tech → Tuck Shop 1
    //G.addEdge(3, 20, 90, true, "road");    //B.tech -> CSA

    G.addEdge(4, 22, 80, true,  "Tunnel");   // CSIT → Laxmi Bai Hostel
    G.addEdge(4, 16, 10, true,  "road");   // CSIT → Tuck Shop 2
    G.addEdge(4, 25, 5, true,   "road");   // CSIT → Parking 2
    G.addEdge(4, 5, 20, true,   "road");   // CSIT → Paramedical Block

    G.addEdge(5, 25, 5, true,   "road");   // Paramedical → Parking 2

    G.addEdge(6, 11, 30, true,  "road");   // Old MCA → Param Lab
    //G.addEdge(6, 8, 30, true,   "road");   // Old MCA → GEU Block
    G.addEdge(6, 13, 20, true,  "road");   // Old MCA → Quick Bite Cafeteria
    G.addEdge(6, 9, 10, true,   "road");   // Old MCA → Chanakya Block
    G.addEdge(6, 7, 40, true,   "road");   // Old MCA → Mechanical Block

    G.addEdge(7, 24, 10, true,  "road");   // Mechanical → Parking 1
    G.addEdge(7, 13, 20, true,  "road");   // Mechanical → Quick Bite

    //G.addEdge(8, 9, 40, true,   "road");   // GEU → Chanakya Block
    G.addEdge(8, 11, 40, true,  "road");   // GEU → Param Lab
    G.addEdge(8, 18, 10, true,  "road");   // GEU → Basketball Court

    G.addEdge(9, 24, 30, true, "road");    // Chanakya → Parking 1

    G.addEdge(10, 28, 40, true, "road");   // Library → Civil Block
    G.addEdge(10, 18, 30, true, "road");   // Library → Basketball Court
    G.addEdge(10, 11, 10, true, "road");   // Library → Param Lab
    G.addEdge(10, 12, 30, true, "road");   // Library → Aryabhatt Lab

    G.addEdge(11, 12, 20, true, "road");   // Param Lab → Aryabhatt Lab

    G.addEdge(12, 13, 40, true, "road");   // Aryabhatt → Quick Bite
  //G.addEdge(12, 15, 80, true, "road");   // Aryabhatt → Tuck Shop 1

    G.addEdge(14, 27, 30, true, "road");   // Ravi Canteen → Happiness Cafeteria
    G.addEdge(14, 19, 120, false, "road"); // Ravi Canteen → Main Ground
    G.addEdge(14, 20, 20, true, "road");   // Ravi Canteen → CSA Hostel

    G.addEdge(15, 23, 5, true, "road");    // Tuck Shop 1 → Medical Room
    G.addEdge(15, 19, 100, true, "road");  // Tuck Shop 1 → Main Ground

    G.addEdge(16, 25, 5, true, "road");    // Tuck Shop 2 → Parking 2

    G.addEdge(17, 27, 40, true, "road");   // Badminton → Happiness Cafeteria
    G.addEdge(17, 28, 20, true, "road");   // Badminton → Civil Block

    G.addEdge(18, 28, 20, true, "road");   // Basketball → Civil Block

    G.addEdge(19, 20, 100, false, "road"); // Ground → CSA Hostel
    G.addEdge(19, 21, 100, false, "road"); // Ground → SP Hostel
    //G.addEdge(19, 23, 100, false, "road"); // Ground → Medical Room
    G.addEdge(19, 22, 100, false, "road"); // Ground → Laxmi Bai Hostel

    G.addEdge(20, 21, 10, true, "road");   // CSA → SP Hostel
    G.addEdge(21, 22, 20, true, "road");   // SP → Laxmi Bai Hostel

    G.addEdge(26, 2, 160, true, "road");   // Convention → Gate 3

    G.addEdge(27, 29, 0, false, "indoor");   // Happiness Cafeteria → Gym
    return G;

}
#endif
