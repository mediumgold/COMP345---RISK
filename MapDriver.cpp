//
// Created by Aidan Catriel on 2025-09-13.
// Driver program to manage maps.
//

#include "Map.h"
#include "MapLoader.h"
#include <iostream>

Map testLoadMaps(string filepath)
{
    try
    {
        MapLoader loader(filepath);
        return loader.getMap();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return Map(); // Return an empty map on failure.
}

int main() {
    // Test loading a map.
    Map map = testLoadMaps("Maps/Americas 1792.map");
    cout << map.toString() << endl;
    bool valid = map.validate();
    cout << "Map is " << (valid ? "valid" : "invalid") << ".\n";
    return 0;
}