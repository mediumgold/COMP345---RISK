//
// Created by Rahath on 2025-09-09.
// Stores map information.
//

#include "Map.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

Map::Map()
{
    name = "Unnamed Map";
}

Map::Map(string name, unordered_map<string, int> continents, const vector<territory> territories)
{
    this->name = name;
    this->continents = continents;
    
    // Convert territories to territoryNodes and set up adjacency.
    // Start by creating all territoryNodes without adjacencies.
    for (const auto& terr : territories) {
        territoryNode node;
        node.name = terr.name;

        // Check if the continent exists.
        if (!continents[terr.continent]) {
            throw runtime_error("Continent " + terr.continent + " for territory " + terr.name + " does not exist in continents map.");
        }

        node.continent = terr.continent;
        territoryNodes.push_back(node);
    }

    // Now set up adjacencies.
    for (int i = 0; i < territories.size(); i++) {
        for (const auto& adjName : territories[i].adjacentTerritories) {
            if (adjName == territoryNodes[i].name) {
                throw runtime_error("Territory " + territoryNodes[i].name + " cannot be adjacent to itself.");
            }
            //cout << "Setting adjacency for " << territories[i].name << " to " << adjName << ".\n";

            // Find the index of the adjacent territory.
            int index = -1;
            for (int j = 0; j < territoryNodes.size(); ++j) {
                if (territoryNodes[j].name == adjName) {
                    index = j;
                    break;
                }
            }
            if (index != -1)
            {
                // Verify that the adjacency is not already recorded to avoid duplicates.
                if (find(territoryNodes[i].adjacentIndices.begin(), territoryNodes[i].adjacentIndices.end(), index) == territoryNodes[i].adjacentIndices.end())
                {
                    territoryNodes[i].adjacentIndices.push_back(index);
                }
                // Also add the reverse adjacency to avoid errors from sloppy map files.
                if (find(territoryNodes[index].adjacentIndices.begin(), territoryNodes[index].adjacentIndices.end(), i) == territoryNodes[index].adjacentIndices.end())
                {
                    territoryNodes[index].adjacentIndices.push_back(i);
                }
            }
            else {
                throw runtime_error("Adjacent territory " + adjName + " for territory " + territories[i].name + " does not exist.");
            }
        }
        // Debug: Print actual adjacencies.
        string actualAdjacents = "";
        for (int idx : territoryNodes[i].adjacentIndices) {
            actualAdjacents += territoryNodes[idx].name + ", ";
        }
    }
}

bool Map::validate() {
    // First validation: Ensure that the map is a connected graph.
    // From any starting territory, we should be able to reach all other territories.
    if (territoryNodes.empty()) {
        return false;
    }

    for (const auto& target : territoryNodes) {
        if (target.name != territoryNodes[0].name) { // Skip self-comparison.
            if (!territoriesConnected(territoryNodes[0], target)) {
                cout << "Map validation failed: Territory " << target.name << " is not connected to " << territoryNodes[0].name << ".\n";
                return false; // Not all territories are connected.
            }
        }
    }

    // Second validation: Ensure each continent is a connected subgraph.
    for (const auto& [continentName, _] : continents) {
        // Collect all territories in this continent.
        vector<territoryNode> continentTerritories;
        bool foundTerritory = false;
        for (const auto& terr : territoryNodes) {
            if (terr.continent == continentName) {
                continentTerritories.push_back(terr);
                if (!foundTerritory) {
                    foundTerritory = true;
                }
            }
            else if (foundTerritory) {
                // Since territoryNodes are sorted by continent, we can break early.
                break;
            }
        }

        if (continentTerritories.empty()) {
            cout << "Map validation failed: Continent " << continentName << " has no territories.\n";
            return false; // Continent has no territories.
        }

        if (continentTerritories.size() == 2) {
            return true; // A single territory is trivially connected.
        }

        for (const auto& target : continentTerritories) {
            bool debug = false;
            //cout << "Checking connectivity in continent " << continentName << " between " << continentTerritories[0].name << " and " << target.name << ".\n";
            if (target.name != continentTerritories[0].name) { // Skip self-comparison.
                if (!territoriesConnected(continentTerritories[0], target, continentName, debug)) {
                    cout << "Map validation failed: In continent " << continentName << ", territory " << target.name << " is not connected to " << continentTerritories[0].name << ".\n";
                    return false; // Not all territories in the continent are connected.
                }
            }
        }
    }

    // Third validation: Each territory has a valid continent.
    for (const auto& terr : territoryNodes) {
        if (continents.find(terr.continent) == continents.end()) {
            cout << "Map validation failed: Territory " << terr.name << " has invalid continent " << terr.continent << ".\n";
            return false; // Territory has an invalid continent.
        }
    }

    return true;
}

bool Map::territoriesConnected(const territoryNode& start, const territoryNode& target, const string requiredContinent, bool debug) {
    // Simple BFS to check connectivity.
    vector<bool> visited(territoryNodes.size(), false);

    vector<int> queue;

    // Find the start index.
    for (int i = 0; i < territoryNodes.size(); ++i) {
        if (territoryNodes[i].name == start.name) {
            // Mark the start territory as visited and enqueue it.
            queue.push_back(i);
            visited[i] = true;
            break;
        }
    }

    //cout << "Starting BFS from " << start.name << " to find " << target.name << (requiredContinent.empty() ? "" : (" within continent " + requiredContinent)) << ".\n";
    
    while (!queue.empty()) {
        int currentIndex = queue.front();
        queue.erase(queue.begin());

        if (territoryNodes[currentIndex].name == target.name) {
            if (debug)
            {
                cout << "Reached target " << target.name << " from adjacent " << territoryNodes[currentIndex].name << ".\n";
            }
            return true; // Found the target.
        }

        for (int adjIndex : territoryNodes[currentIndex].adjacentIndices) {
            if (!visited[adjIndex]) {
                // Check if the adjacent territory is in the considered territories.
                if (requiredContinent.empty() || requiredContinent == "" || territoryNodes[adjIndex].continent == requiredContinent)
                {
                    visited[adjIndex] = true;
                    queue.push_back(adjIndex);
                }
                if (debug)
                {
                    cout << "Visiting " << territoryNodes[adjIndex].name << " from " << territoryNodes[currentIndex].name << ".\n";
                }
            }
        }
    }

    return false;
}

string Map::toString() {
    string result = "Map Name: " + name + "\nContinents:\n";
    for (auto const& [key, val] : continents) {
        result += " - " + key + ": Control Value " + to_string(val) + "\n";
    }
    result += "Territories:\n";
    for (const auto& terr : territoryNodes) {
        result += " - " + terr.name + " (Continent: " + terr.continent + ")\n";
        result += "   Adjacent Territories: ";

        for (const auto& adj : terr.adjacentIndices) {
                result += territoryNodes[adj].name + ", ";
        }
        if (!terr.adjacentIndices.empty()) {
            result = result.substr(0, result.size() - 2); // Remove trailing comma and space.
        }
        result += "\n";
    }
    return result;
}