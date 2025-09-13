//
// Created by Aidan Catriel on 2025-09-13.
// Reads a .map file and creates a Map object.
//

#include "Map.h"
#include "MapLoader.h"
#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>
using namespace std;


MapLoader::MapLoader(string filepath) {
    this->filepath = filepath;
    ifstream file(filepath);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filepath);
    }
    
    // Read lines from the file.
    string line;

    // State bools.
    bool inContinents = false;
    bool inTerritories = false;

    // Unordered map (effectively a dictionary) of continents and their control values.
    unordered_map<string, int> continents;

    while (getline(file, line)) {
        if (line == "[Continents]") {
            inContinents = true;
            inTerritories = false;
            continue;
        } else if (line == "[Territories]") {
            inContinents = false;
            inTerritories = true;
            continue;
        } else if (line.empty() || line[0] == ';') {
            continue; // Skip empty lines and comments.
        }

        if (inContinents) {
            // Parse continent line, split by =.
            size_t spacePos = line.find('=');
            if (spacePos != string::npos) {
                string continentName = line.substr(0, spacePos);
                int controlValue = stoi(line.substr(spacePos + 1));
                continents[continentName] = controlValue;
            }
        }
    }


    file.close();

    // Extract the map name from the file path.
    string name = filepath.substr(filepath.find_last_of("/\\") + 1);

    map = Map(name, continents); // Placeholder for actual map initialization.
}