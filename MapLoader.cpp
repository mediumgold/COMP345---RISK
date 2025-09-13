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
    vector<Map::territory> territories;

    try
    {
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
            else if (inTerritories) {
                // A copy of the line to manipulate.
                string lineCopy = line;
                // Parse territory line, split by ,.
                vector<string> parts;
                string delimiter = ",";
                int pos = 0;

                while ((pos = lineCopy.find(delimiter)) != string::npos) {
                    string part = lineCopy.substr(0, pos);
                    parts.push_back(part);
                    lineCopy.erase(0, pos + delimiter.length());
                }
                parts.push_back(lineCopy); // Add the last part.

                string name = parts[0];
                // Parts 2 and 3 are x and y coordinates (ignored in this console game).
                string continent = parts[3];
                // Territories are parts from index 4 onwards.
                vector<string> adjacentTerritories;
                for (size_t i = 4; i < parts.size(); ++i) {
                    adjacentTerritories.push_back(parts[i]);
                }

                territories.push_back({name, continent, adjacentTerritories});
            }
        }
    }
    catch (const std::exception& e)
    {
        throw runtime_error("Invalid map file: " + string(e.what()));
    }


    file.close();

    // Extract the map name from the file path.
    string name = filepath.substr(filepath.find_last_of("/\\") + 1);

    try
    {
        map = Map(name, continents, territories);
    }
    catch (const runtime_error& e)
    {
        throw;
    }
}