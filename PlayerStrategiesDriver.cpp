//
// Created for Part 1: Player Strategy Pattern
//

#include "PlayerStrategies.h"
#include "Player.h"
#include "Map.h"
#include "Cards.h"
#include "Orders.h"
#include <iostream>
#include <vector>
using namespace std;

/**
 * Driver function that demonstrates:
 * 1. Different players can be assigned different strategies that lead to different behavior
 * 2. The strategy adopted by a player can be changed dynamically during play
 * 3. Human player makes decisions according to user interaction, computer players make decisions automatically
 */
void testPlayerStrategies() {
    cout << "\n=== Part 1: Player Strategy Pattern Demonstration ===\n\n";

    // Create a simple map for testing
    Map testMap("TestMap", {{"Continent1", 5}}, {});
    Map::territoryNode t1, t2, t3, t4;
    t1.name = "Territory1";
    t1.continent = "Continent1";
    t1.armyCount = 5;
    t1.adjacentIndices = {1};
    
    t2.name = "Territory2";
    t2.continent = "Continent1";
    t2.armyCount = 3;
    t2.adjacentIndices = {0, 2};
    
    t3.name = "Territory3";
    t3.continent = "Continent1";
    t3.armyCount = 2;
    t3.adjacentIndices = {1, 3};
    
    t4.name = "Territory4";
    t4.continent = "Continent1";
    t4.armyCount = 4;
    t4.adjacentIndices = {2};

    auto& nodes = testMap.getTerritoryNodes();
    nodes.push_back(t1);
    nodes.push_back(t2);
    nodes.push_back(t3);
    nodes.push_back(t4);

    // Create players
    Player* player1 = new Player("HumanPlayer");
    Player* player2 = new Player("AggressivePlayer");
    Player* player3 = new Player("BenevolentPlayer");
    Player* player4 = new Player("NeutralPlayer");

    // Set up map and deck references
    Deck* testDeck = new Deck(20);
    player1->setMap(&testMap);
    player2->setMap(&testMap);
    player3->setMap(&testMap);
    player4->setMap(&testMap);
    
    player1->setDeck(testDeck);
    player2->setDeck(testDeck);
    player3->setDeck(testDeck);
    player4->setDeck(testDeck);

    // Assign territories
    nodes[0].owner = player1;
    nodes[1].owner = player2;
    nodes[2].owner = player3;
    nodes[3].owner = player4;
    
    player1->addTerritory(&nodes[0]);
    player2->addTerritory(&nodes[1]);
    player3->addTerritory(&nodes[2]);
    player4->addTerritory(&nodes[3]);

    // Give players some reinforcements
    player1->setReinforcementPool(10);
    player2->setReinforcementPool(10);
    player3->setReinforcementPool(10);
    player4->setReinforcementPool(10);

    cout << "\n--- Demonstration 1: Different strategies lead to different behavior ---\n";
    
    // Assign different strategies
    player1->setStrategy(new HumanPlayerStrategy(player1));
    player2->setStrategy(new AggressivePlayerStrategy(player2));
    player3->setStrategy(new BenevolentPlayerStrategy(player3));
    player4->setStrategy(new NeutralPlayerStrategy(player4));

    cout << "\nPlayer 1 (Human): Will prompt for user input\n";
    cout << "Player 2 (Aggressive): Will deploy to strongest, then attack\n";
    cout << "Player 3 (Benevolent): Will deploy to weakest, never attack\n";
    cout << "Player 4 (Neutral): Will do nothing\n";

    // Demonstrate different behaviors
    cout << "\n--- Player 2 (Aggressive) issuing orders ---\n";
    player2->issueOrder();
    cout << "Orders issued: " << player2->getOrdersList()->size() << "\n";

    cout << "\n--- Player 3 (Benevolent) issuing orders ---\n";
    player3->issueOrder();
    cout << "Orders issued: " << player3->getOrdersList()->size() << "\n";

    cout << "\n--- Player 4 (Neutral) issuing orders ---\n";
    player4->issueOrder();
    cout << "Orders issued: " << player4->getOrdersList()->size() << "\n";

    cout << "\n--- Demonstration 2: Dynamic strategy change ---\n";
    cout << "Player 4 (Neutral) is being attacked! Changing to Aggressive strategy...\n";
    delete player4->getStrategy(); // Clean up old strategy
    player4->setStrategy(new AggressivePlayerStrategy(player4));
    cout << "Player 4 now has AggressivePlayerStrategy\n";
    
    cout << "\n--- Player 4 (now Aggressive) issuing orders ---\n";
    player4->issueOrder();
    cout << "Orders issued: " << player4->getOrdersList()->size() << "\n";

    cout << "\n--- Demonstration 3: Human vs Computer strategies ---\n";
    cout << "Human player (Player 1) will now issue orders with user interaction:\n";
    cout << "(Note: In a real game, this would prompt for user input)\n";
    // Uncomment the line below to test human interaction:
    // player1->issueOrder();

    // Cleanup
    delete player1->getStrategy();
    delete player2->getStrategy();
    delete player3->getStrategy();
    delete player4->getStrategy();
    delete player1;
    delete player2;
    delete player3;
    delete player4;
    delete testDeck;

    cout << "\n=== End of Part 1 Demonstration ===\n\n";
}

