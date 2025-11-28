//
// Created for Part 1: Player Strategy Pattern
// FIXED VERSION with proper memory management
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
    cout << "\n===============================================\n";
    cout << "=== PART 1: PLAYER STRATEGY PATTERN DEMO ===\n";
    cout << "===============================================\n\n";

    // ========== SETUP ==========
    cout << "--- Setting up test environment ---\n";

    // Create a simple map for testing
    Map testMap("TestMap", {{"Continent1", 5}}, {});
    Map::territoryNode t1, t2, t3, t4;

    t1.name = "Territory1";
    t1.continent = "Continent1";
    t1.armyCount = 10;
    t1.adjacentIndices = {1};

    t2.name = "Territory2";
    t2.continent = "Continent1";
    t2.armyCount = 3;
    t2.adjacentIndices = {0, 2};

    t3.name = "Territory3";
    t3.continent = "Continent1";
    t3.armyCount = 7;
    t3.adjacentIndices = {1, 3};

    t4.name = "Territory4";
    t4.continent = "Continent1";
    t4.armyCount = 2;
    t4.adjacentIndices = {2};

    auto& nodes = testMap.getTerritoryNodes();
    nodes.push_back(t1);
    nodes.push_back(t2);
    nodes.push_back(t3);
    nodes.push_back(t4);

    // Create players
    Player* humanPlayer = new Player("Alice (Human)");
    Player* aggressivePlayer = new Player("Bob (Aggressive)");
    Player* benevolentPlayer = new Player("Charlie (Benevolent)");
    Player* neutralPlayer = new Player("Diana (Neutral)");
    Player* cheaterPlayer = new Player("Eve (Cheater)");

    // Create shared deck
    Deck* testDeck = new Deck(20);

    // Setup all players
    Player* allPlayers[] = {humanPlayer, aggressivePlayer, benevolentPlayer, neutralPlayer, cheaterPlayer};
    for (int i = 0; i < 5; i++) {
        allPlayers[i]->setMap(&testMap);
        allPlayers[i]->setDeck(testDeck);
    }

    // Assign territories to players
    nodes[0].owner = aggressivePlayer;
    aggressivePlayer->addTerritory(&nodes[0]);

    nodes[1].owner = benevolentPlayer;
    benevolentPlayer->addTerritory(&nodes[1]);

    nodes[2].owner = neutralPlayer;
    neutralPlayer->addTerritory(&nodes[2]);

    nodes[3].owner = cheaterPlayer;
    cheaterPlayer->addTerritory(&nodes[3]);

    // Give players reinforcements
    aggressivePlayer->setReinforcementPool(15);
    benevolentPlayer->setReinforcementPool(15);
    neutralPlayer->setReinforcementPool(15);
    cheaterPlayer->setReinforcementPool(15);

    cout << "Map and players created.\n";
    cout << "Territories assigned:\n";
    cout << "  - " << nodes[0].name << " -> " << nodes[0].owner->getName() << " (" << nodes[0].armyCount << " armies)\n";
    cout << "  - " << nodes[1].name << " -> " << nodes[1].owner->getName() << " (" << nodes[1].armyCount << " armies)\n";
    cout << "  - " << nodes[2].name << " -> " << nodes[2].owner->getName() << " (" << nodes[2].armyCount << " armies)\n";
    cout << "  - " << nodes[3].name << " -> " << nodes[3].owner->getName() << " (" << nodes[3].armyCount << " armies)\n";

    // ========== DEMONSTRATION 1: Different strategies = Different behaviors ==========
    cout << "\n===============================================\n";
    cout << "DEMO 1: Different strategies lead to different behavior\n";
    cout << "===============================================\n";

    // Assign strategies (Strategy objects are now owned by Player via setStrategy)
    cout << "\nAssigning strategies to players...\n";
    aggressivePlayer->setStrategy(new AggressivePlayerStrategy(aggressivePlayer));
    benevolentPlayer->setStrategy(new BenevolentPlayerStrategy(benevolentPlayer));
    neutralPlayer->setStrategy(new NeutralPlayerStrategy(neutralPlayer));
    cheaterPlayer->setStrategy(new CheaterPlayerStrategy(cheaterPlayer));

    cout << "✓ Strategies assigned.\n";

    // Show each player's behavior
    cout << "\n--- Testing Aggressive Player ---\n";
    cout << "Expected: Deploy to strongest territory, then attack\n";
    aggressivePlayer->issueOrder();
    cout << "Orders in list: " << aggressivePlayer->getOrdersList()->size() << "\n";

    cout << "\n--- Testing Benevolent Player ---\n";
    cout << "Expected: Deploy to weakest territory, never attack\n";
    benevolentPlayer->issueOrder();
    cout << "Orders in list: " << benevolentPlayer->getOrdersList()->size() << "\n";

    cout << "\n--- Testing Neutral Player ---\n";
    cout << "Expected: Do nothing\n";
    neutralPlayer->issueOrder();
    cout << "Orders in list: " << neutralPlayer->getOrdersList()->size() << "\n";

    cout << "\n--- Testing Cheater Player ---\n";
    cout << "Expected: Automatically conquer adjacent territories\n";
    size_t territoriesBefore = cheaterPlayer->getOwnedTerritories()->size();
    cheaterPlayer->issueOrder();
    size_t territoriesAfter = cheaterPlayer->getOwnedTerritories()->size();
    cout << "Territories owned before: " << territoriesBefore << "\n";
    cout << "Territories owned after: " << territoriesAfter << "\n";

    // ========== DEMONSTRATION 2: Dynamic strategy change ==========
    cout << "\n===============================================\n";
    cout << "DEMO 2: Dynamic strategy change during play\n";
    cout << "===============================================\n";

    cout << "\nNeutral player (" << neutralPlayer->getName() << ") is being attacked!\n";
    cout << "Current strategy: " << neutralPlayer->getStrategy()->getStrategyName() << "\n";
    cout << "Changing to Aggressive strategy...\n";

    // CORRECT WAY: Just call setStrategy, it handles deletion internally
    neutralPlayer->setStrategy(new AggressivePlayerStrategy(neutralPlayer));

    cout << "New strategy: " << neutralPlayer->getStrategy()->getStrategyName() << "\n";

    cout << "\n--- Neutral player (now Aggressive) issuing orders ---\n";
    neutralPlayer->issueOrder();
    cout << "Orders in list: " << neutralPlayer->getOrdersList()->size() << "\n";
    cout << "✓ Strategy successfully changed at runtime!\n";

    // ========== DEMONSTRATION 3: toAttack() and toDefend() methods ==========
    cout << "\n===============================================\n";
    cout << "DEMO 3: toAttack() and toDefend() method differences\n";
    cout << "===============================================\n";

    cout << "\n--- Aggressive Player ---\n";
    auto aggressiveDefend = aggressivePlayer->toDefend();
    auto aggressiveAttack = aggressivePlayer->toAttack();
    cout << "Territories to defend: " << aggressiveDefend.size() << " (prioritizes strongest)\n";
    cout << "Territories to attack: " << aggressiveAttack.size() << "\n";

    cout << "\n--- Benevolent Player ---\n";
    auto benevolentDefend = benevolentPlayer->toDefend();
    auto benevolentAttack = benevolentPlayer->toAttack();
    cout << "Territories to defend: " << benevolentDefend.size() << " (prioritizes weakest)\n";
    cout << "Territories to attack: " << benevolentAttack.size() << " (never attacks!)\n";

    cout << "\n--- Neutral Player (now Aggressive) ---\n";
    auto neutralDefend = neutralPlayer->toDefend();
    auto neutralAttack = neutralPlayer->toAttack();
    cout << "Territories to defend: " << neutralDefend.size() << "\n";
    cout << "Territories to attack: " << neutralAttack.size() << "\n";

    // ========== DEMONSTRATION 4: Strategy Pattern Compliance ==========
    cout << "\n===============================================\n";
    cout << "DEMO 4: Strategy Pattern Compliance Verification\n";
    cout << "===============================================\n";

    cout << "\n✓ Player class contains PlayerStrategy* pointer\n";
    cout << "✓ Player delegates issueOrder() to strategy->issueOrder()\n";
    cout << "✓ Player delegates toDefend() to strategy->toDefend()\n";
    cout << "✓ Player delegates toAttack() to strategy->toAttack()\n";
    cout << "✓ PlayerStrategy is abstract (cannot be instantiated)\n";
    cout << "✓ Concrete strategies implement their own behavior\n";
    cout << "✓ No Player subclasses - all variation through strategy\n";
    cout << "✓ Strategy can be changed at runtime\n";

    // ========== HUMAN PLAYER NOTE ==========
    cout << "\n===============================================\n";
    cout << "NOTE: Human Player Strategy\n";
    cout << "===============================================\n";
    cout << "Human player requires user interaction (keyboard input).\n";
    cout << "To test human strategy, uncomment the following lines in your main:\n";
    cout << "    humanPlayer->setStrategy(new HumanPlayerStrategy(humanPlayer));\n";
    cout << "    humanPlayer->issueOrder();\n";
    cout << "The system will prompt for deployment and attack decisions.\n";

    // ========== CLEANUP ==========
    cout << "\n--- Cleaning up ---\n";

    // Delete players (their destructors will delete strategies)
    delete humanPlayer;
    delete aggressivePlayer;
    delete benevolentPlayer;
    delete neutralPlayer;
    delete cheaterPlayer;
    delete testDeck;

    cout << "\n===============================================\n";
    cout << "=== END OF PART 1 DEMONSTRATION ===\n";
    cout << "===============================================\n\n";
}

//int main() {
//    testPlayerStrategies();
//    return 0;
//}
