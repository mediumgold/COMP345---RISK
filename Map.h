//
// Created by Rahath on 2025-09-09.
//

#ifndef COMP345_RISK_MAP_H
#define COMP345_RISK_MAP_H

#include <string>
#include <unordered_map>
using namespace std;

class Map {
    // Constructor: Read a .map file and initialize the Map object.
    public:
        Map();
        Map(string name, unordered_map<string, int> continents);
        string toString();
        string getName() const { return name; }
    private:
        string name;
        // Unordered map (effectively a dictionary) of continents and their control values.
        unordered_map<string, int> continents;
};

#endif