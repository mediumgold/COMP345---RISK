//
// Created by Rahath on 2025-09-09.
// Stores map information.
//

#include "Map.h"
#include <fstream>
using namespace std;

Map::Map()
{
    name = "Unnamed Map";
}

Map::Map(string name, unordered_map<string, int> continents)
{
    this->name = name;
    this->continents = continents;
}

string Map::toString() {
    string result = "Map Name: " + name + "\nContinents:\n";
    for (auto const& [key, val] : continents) {
        result += " - " + key + ": Control Value " + to_string(val) + "\n";
    }
    return result;
}