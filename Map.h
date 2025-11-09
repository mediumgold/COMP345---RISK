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
        // A territory as read from the map file. Adjacent territories are stored as names.
        struct territory {
            string name;
            string continent;
            vector<string> adjacentTerritories;
        };

        // A territory node used internally in the Map class. Adjacent territories are stored as indices to other stored territoryNodes.
        struct territoryNode {
            string name;
            string continent;
            vector<int> adjacentIndices;
        };

        Map();
        Map(string name, unordered_map<string, int> continents, vector<territory> territories);
        Map(const Map& other);
        Map& operator=(const Map& other);

        bool validate();

        string toString();

        //  Stream insertion operator (replaces toString)
        friend ostream& operator<<(ostream& os, const Map& map);
        string getName() const { return name; }
        vector<territoryNode>& getTerritoryNodes();
        const vector<territoryNode>& getTerritoryNodes() const;
    private:
        string name;
        // Unordered map (effectively a dictionary) of continents and their control values.
        unordered_map<string, int> continents;
        // List of territories.
        vector<territoryNode> territoryNodes;
        int armyCount;
        // TODO: Switch this with a Player reference.
        int ownerID;

        bool territoriesConnected(const territoryNode& start, const territoryNode& target, const string requiredContinent = "", bool debug = false);
};

#endif
