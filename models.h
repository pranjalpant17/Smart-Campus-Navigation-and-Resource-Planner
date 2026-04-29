#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
using namespace std;


class Resource {
public:
    string name;
    int capacity;
    bool isOccupied;

    Resource(string n, int cap)
        : name(n), capacity(cap), isOccupied(false) {}

    virtual string getType() const = 0;

    virtual void display() const {
        cout << name << " (" << getType()
             << ", Cap: " << capacity << ") "
             << (isOccupied ? "[Occupied]" : "[Free]") << endl;
    }

    virtual ~Resource() {}
};

class Classroom : public Resource {
public:
    Classroom(string n, int cap) : Resource(n, cap) {}
    string getType() const override { return "Classroom"; }
};

class Lab : public Resource {
public:
    Lab(string n, int cap) : Resource(n, cap) {}
    string getType() const override { return "Lab"; }
};

struct Building {
    int id;
    string name;

    vector<shared_ptr<Resource>> resources;

    Building() : id(-1), name("") {}

    Building(int id, string name)
        : id(id), name(name) {}

    void addResource(shared_ptr<Resource> r) {
        resources.push_back(r);
    }
};

struct Edge {
    int to;
    int weight;
    bool isAccessible;
    string pathType;

    Edge(int to, int weight,
         bool accessible = true,
         string pathType = "road")
        : to(to), weight(weight),
          isAccessible(accessible), pathType(pathType) {}
};

#endif
