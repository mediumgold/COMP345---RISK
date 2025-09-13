//
// Created by Aidan Catriel on 2025-09-13.
// Reads a .map file and creates a Map object.
//

#ifndef COMP345_RISK_MAPLOADER_H
#define COMP345_RISK_MAPLOADER_H

#include <string>
#include "Map.h"
using namespace std;

class MapLoader {
    // Constructor: Read a .map file and initialize the Map object.
    public:
        MapLoader(string filepath);

        string getFilepath() const { return filepath; }
        Map getMap() const { return map; }
    private:
        string filepath;
        Map map;
};

#endif