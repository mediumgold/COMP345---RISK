//
// Created by Aidan Catriel on 2025-09-13.
// Driver program to manage maps.
//

#include "Map.h"
#include "MapLoader.h"
#include <iostream>

Map testLoadMaps(string filepath)
{
    MapLoader loader(filepath);
    return loader.getMap();
}

int main() {
    // Test loading a map.
    Map map = testLoadMaps("Maps/Americas 1792.map");
    cout << map.toString() << endl;
    return 0;
}