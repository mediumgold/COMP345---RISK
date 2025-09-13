//
// Created by Rahath on 2025-09-09.
//

#ifndef COMP345_RISK_MAP_H
#define COMP345_RISK_MAP_H

#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

class Map {
    // Constructor: Read a .map file and initialize the Map object.
    public:
        struct territory {
            string name;
            string continent;
            vector<string> adjacentTerritories;
        };

        struct territoryNode {
            string name;
            string continent;
            vector<int> adjacentIndices;
        };

        Map();
        Map(string name, unordered_map<string, int> continents, vector<territory> territories);

        bool validate();

        string toString();
        string getName() const { return name; }
    private:
        string name;
        // Unordered map (effectively a dictionary) of continents and their control values.
        unordered_map<string, int> continents;
        // List of territories.
        vector<territoryNode> territoryNodes;

        bool territoriesConnected(const territoryNode& start, const territoryNode& target, const string requiredContinent = "", bool debug = false);
};

#endif