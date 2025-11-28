//
// Created by Rahath on 2025-09-09.
//

#include "Player.h"
#include "Map.h"
#include "Cards.h"
#include "Orders.h"
#include <iostream>
using namespace std;

void testPlayers() {
    cout << "=== Part 2: Player Demo ===\n";

    // Simulate territories (from Map)
    Map::territoryNode quebec;   // default ctor sets armyCount=0, owner=nullptr
    quebec.name = "Quebec";
    quebec.continent = "North America";
    quebec.adjacentIndices.push_back(1);

    Map::territoryNode ontario;
    ontario.name = "Ontario";
    ontario.continent = "North America";
    ontario.adjacentIndices.push_back(0);

    // Create a player
    Player alice("Alice");

    // Assign territories
    alice.addTerritory(&quebec);
    alice.addTerritory(&ontario);

    // Add some cards
    Card* bomb = new Card("bomb");
    Card* airlift = new Card("airlift");
    alice.addCard(bomb);
    alice.addCard(airlift);

    // Create and issue some orders
    Deploy* deploy = new Deploy(5, "Quebec");
    Advance* advance = new Advance(3, "Quebec", "Ontario");

    alice.issueOrder(deploy);
    alice.issueOrder(advance);

    // Show player state
    cout << alice << endl;

    // Defend / Attack lists
    auto defendList = alice.toDefend();
    auto attackList = alice.toAttack();


    //Display possible territories a player can defend and attack
    cout << "\nTerritories to Defend (ARBITRARY): ";
    for (auto* t : defendList) cout << t->name << " ";
    cout << "\nTerritories to Attack (ARBITRARY): ";
    for (auto* t : attackList) cout << t->name << " ";

    cout << "\n\n=== End of Part 2 Demo ===\n";
}

/**
int main() {
    testPlayers();
    return 0;
}
*/