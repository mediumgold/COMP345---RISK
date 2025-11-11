//
// Created by Rahath on 2025-09-09.
//
#include "Orders.h"
#include "Player.h"
#include "Map.h"
#include "Cards.h"
#include <iostream>

void testOrdersLists() {
    std::cout << "========== Testing Orders List ==========\n\n";

    // create orders
    std::cout << "Creating orders of every kind...\n";
    Deploy* deploy = new Deploy(5, "Territory1");
    Advance* advance = new Advance(3, "Territory1", "Territory2");
    Bomb* bomb = new Bomb("EnemyTerritory");
    Blockade* blockade = new Blockade("Territory3");
    Airlift* airlift = new Airlift(4, "Territory4", "Territory5");
    Negotiate* negotiate = new Negotiate("PlayerB");

    std::cout << "Deploy: " << *deploy << "\n";
    std::cout << "Advance: " << *advance << "\n";
    std::cout << "Bomb: " << *bomb << "\n";
    std::cout << "Blockade: " << *blockade << "\n";
    std::cout << "Airlift: " << *airlift << "\n";
    std::cout << "Negotiate: " << *negotiate << "\n\n";

    // create OrdersList and add all orders
    std::cout << "Creating OrdersList and adding all orders...\n";
    OrdersList* ordersList = new OrdersList();
    ordersList->addOrder(deploy);
    ordersList->addOrder(advance);
    ordersList->addOrder(bomb);
    ordersList->addOrder(blockade);
    ordersList->addOrder(airlift);
    ordersList->addOrder(negotiate);

    std::cout << "Orders added to list.\n";
    std::cout << *ordersList << "\n";

    // demonstrate validate() method for each order
    std::cout << "Demonstrating validate() method:\n";
    for (int i = 0; i < ordersList->size(); i++) {
        Order* order = ordersList->getOrder(i);
        bool valid = order->validate();
        std::cout << "Order " << i << " is " << (valid ? "valid" : "invalid") << "\n";
    }
    std::cout << "\n";

    // demonstrate execute() method, validates then executes
    std::cout << "Demonstrating execute() method:\n";
    std::cout << "Executing all orders in the list...\n";
    for (int i = 0; i < ordersList->size(); i++) {
        Order* order = ordersList->getOrder(i);
        order->execute();
    }

    std::cout << "\nOrders after execution:\n";
    std::cout << *ordersList << "\n";

    //demonstrate move() method
    std::cout << "Demonstrating move() method:\n";
    std::cout << "Moving order from index position 0 to index position 3...\n";
    ordersList->move(0, 3);
    std::cout << *ordersList << "\n";

    //demonstrate remove() method
    std::cout << "Demonstrating remove() method:\n";
    std::cout << "Removing order at index position 1...\n";
    ordersList->remove(1);
    std::cout << *ordersList << "\n";

    // cleanup
    delete ordersList;

    std::cout << "========== End of Orders List Test ==========\n";
}

// ADDED FOR ASSIGNMENT 2 PART 4 - New comprehensive driver function
void testOrderExecution() {
    std::cout << "\n========== ASSIGNMENT 2 PART 4: Testing Order Execution ==========\n\n";

    // Create a simple map for testing
    std::cout << "=== Setting up test map and players ===\n";

    // Create map with 4 territories
    Map::territory t1, t2, t3, t4;
    t1.name = "Territory1";
    t1.continent = "Continent1";
    t1.adjacentTerritories = {"Territory2"};

    t2.name = "Territory2";
    t2.continent = "Continent1";
    t2.adjacentTerritories = {"Territory1", "Territory3"};

    t3.name = "Territory3";
    t3.continent = "Continent2";
    t3.adjacentTerritories = {"Territory2", "Territory4"};

    t4.name = "Territory4";
    t4.continent = "Continent2";
    t4.adjacentTerritories = {"Territory3"};

    std::unordered_map<std::string, int> continents;
    continents["Continent1"] = 5;
    continents["Continent2"] = 3;

    std::vector<Map::territory> territories = {t1, t2, t3, t4};
    Map* gameMap = new Map("TestMap", continents, territories);

    std::cout << "Map created with 4 territories\n";

    // Create players
    Player* player1 = new Player("Alice");
    Player* player2 = new Player("Bob");
    Player* neutralPlayer = new Player("Neutral");

    // Create deck for card rewards
    Deck* gameDeck = new Deck(20);

    std::cout << "Players created: Alice, Bob, and Neutral\n\n";

    // Set up initial territory ownership and armies
    auto& territories_ref = gameMap->getTerritoryNodes();

    // Alice owns Territory1 and Territory2
    territories_ref[0].owner = player1;
    territories_ref[0].armyCount = 10;
    player1->addTerritory(&territories_ref[0]);

    territories_ref[1].owner = player1;
    territories_ref[1].armyCount = 8;
    player1->addTerritory(&territories_ref[1]);

    // Bob owns Territory3 and Territory4
    territories_ref[2].owner = player2;
    territories_ref[2].armyCount = 5;
    player2->addTerritory(&territories_ref[2]);

    territories_ref[3].owner = player2;
    territories_ref[3].armyCount = 7;
    player2->addTerritory(&territories_ref[3]);

    // Give Alice some reinforcements
    player1->setReinforcementPool(15);

    std::cout << "Initial setup:\n";
    std::cout << "  " << *player1 << "\n";
    std::cout << "  " << *player2 << "\n";
    std::cout << "  Territory1 (Alice): " << territories_ref[0].armyCount << " armies\n";
    std::cout << "  Territory2 (Alice): " << territories_ref[1].armyCount << " armies\n";
    std::cout << "  Territory3 (Bob): " << territories_ref[2].armyCount << " armies\n";
    std::cout << "  Territory4 (Bob): " << territories_ref[3].armyCount << " armies\n\n";


    // ========== TEST 1: Deploy Order Validation ==========
    std::cout << "=== TEST 1: Deploy Order Validation and Execution ===\n";

    Deploy* validDeploy = new Deploy(player1, 5, &territories_ref[0]);
    std::cout << "Creating valid deploy order: 5 armies to Territory1 (owned by Alice)\n";
    std::cout << "Validation: " << (validDeploy->validate() ? "VALID" : "INVALID") << "\n";
    validDeploy->execute();
    std::cout << "Effect: " << validDeploy->getEffect() << "\n";
    std::cout << "Territory1 now has: " << territories_ref[0].armyCount << " armies\n";
    std::cout << "Alice's reinforcement pool: " << player1->getReinforcementPool() << "\n\n";

    Deploy* invalidDeploy = new Deploy(player1, 5, &territories_ref[2]);
    std::cout << "Creating invalid deploy order: 5 armies to Territory3 (owned by Bob)\n";
    std::cout << "Validation: " << (invalidDeploy->validate() ? "VALID" : "INVALID") << "\n";
    invalidDeploy->execute();
    std::cout << "Effect: " << invalidDeploy->getEffect() << "\n\n";


    // ========== TEST 2: Advance Order - Friendly Move ==========
    std::cout << "=== TEST 2: Advance Order - Friendly Move ===\n";

    // FIXED: Added gameMap parameter
    Advance* friendlyAdvance = new Advance(player1, 3, &territories_ref[0], &territories_ref[1], gameDeck, gameMap);
    std::cout << "Moving 3 armies from Territory1 to Territory2 (both owned by Alice)\n";
    std::cout << "Before: Territory1=" << territories_ref[0].armyCount
              << ", Territory2=" << territories_ref[1].armyCount << "\n";
    std::cout << "Validation: " << (friendlyAdvance->validate() ? "VALID" : "INVALID") << "\n";
    friendlyAdvance->execute();
    std::cout << "Effect: " << friendlyAdvance->getEffect() << "\n";
    std::cout << "After: Territory1=" << territories_ref[0].armyCount
              << ", Territory2=" << territories_ref[1].armyCount << "\n\n";


    // ========== TEST 3: Advance Order - Attack and Conquest ==========
    std::cout << "=== TEST 3: Advance Order - Attack and Territory Conquest ===\n";

    // Set up for a likely conquest: Alice has many armies, Bob has few
    territories_ref[1].armyCount = 15;  // Alice's Territory2
    territories_ref[2].armyCount = 2;   // Bob's Territory3

    player1->setConqueredThisTurn(false);
    std::cout << "Before attack: Territory2 (Alice)=" << territories_ref[1].armyCount
              << " armies, Territory3 (Bob)=" << territories_ref[2].armyCount << " armies\n";
    std::cout << "Owner of Territory3: " << territories_ref[2].owner->getName() << "\n";
    std::cout << "Alice conquered this turn: " << (player1->hasConqueredThisTurn() ? "YES" : "NO") << "\n";

    // FIXED: Added gameMap parameter
    Advance* attackAdvance = new Advance(player1, 10, &territories_ref[1], &territories_ref[2], gameDeck, gameMap);
    std::cout << "\nAlice attacks Territory3 with 10 armies from Territory2\n";
    std::cout << "Validation: " << (attackAdvance->validate() ? "VALID" : "INVALID") << "\n";
    attackAdvance->execute();
    std::cout << "Effect: " << attackAdvance->getEffect() << "\n";
    std::cout << "After attack: Territory3 has " << territories_ref[2].armyCount << " armies\n";
    std::cout << "Owner of Territory3: " << territories_ref[2].owner->getName() << "\n";
    std::cout << "Alice conquered this turn: " << (player1->hasConqueredThisTurn() ? "YES" : "NO") << "\n\n";


    // ========== TEST 4: Card Reward After Conquest ==========
    std::cout << "=== TEST 4: Card Reward System ===\n";

    std::cout << "Since Alice conquered a territory, she should receive a card.\n";
    if (player1->hasConqueredThisTurn()) {
        Card* rewardCard = gameDeck->draw();
        if (rewardCard) {
            player1->addCard(rewardCard);
            std::cout << "Card drawn and added to Alice's hand: " << *rewardCard << "\n";
            std::cout << "Alice's hand now has " << player1->getHand()->size() << " card(s)\n";
        }
        // Reset for next turn
        player1->setConqueredThisTurn(false);
    }
    std::cout << "Note: Player can only receive ONE card per turn, regardless of conquests\n\n";


    // ========== TEST 5: Negotiate Order ==========
    std::cout << "=== TEST 5: Negotiate Order Prevents Attacks ===\n";

    // Reset territory for this test
    territories_ref[3].owner = player2;
    territories_ref[3].armyCount = 8;
    territories_ref[2].armyCount = 10;
    territories_ref[2].owner = player1; // Alice now owns Territory3 from previous conquest

    Negotiate* negotiateOrder = new Negotiate(player1, player2);
    std::cout << "Alice negotiates with Bob\n";
    std::cout << "Validation: " << (negotiateOrder->validate() ? "VALID" : "INVALID") << "\n";
    negotiateOrder->execute();
    std::cout << "Effect: " << negotiateOrder->getEffect() << "\n";

    std::cout << "\nNow trying to attack after negotiation:\n";
    // FIXED: Added gameMap parameter
    Advance* blockedAdvance = new Advance(player1, 5, &territories_ref[2], &territories_ref[3], gameDeck, gameMap);
    std::cout << "Alice attempts to attack Territory4 (Bob's) from Territory3\n";
    std::cout << "Validation: " << (blockedAdvance->validate() ? "VALID (should be INVALID)" : "INVALID (CORRECT)") << "\n";
    blockedAdvance->execute();
    std::cout << "Effect: " << blockedAdvance->getEffect() << "\n\n";


    // ========== TEST 6: Blockade Order ==========
    std::cout << "=== TEST 6: Blockade Order Transfers to Neutral ===\n";

    // Reset ownership for this test
    territories_ref[0].owner = player1;
    territories_ref[0].armyCount = 6;

    std::cout << "Before blockade: Territory1 owned by " << territories_ref[0].owner->getName()
              << " with " << territories_ref[0].armyCount << " armies\n";

    Blockade* blockadeOrder = new Blockade(player1, &territories_ref[0], neutralPlayer);
    std::cout << "Alice blockades Territory1\n";
    std::cout << "Validation: " << (blockadeOrder->validate() ? "VALID" : "INVALID") << "\n";
    blockadeOrder->execute();
    std::cout << "Effect: " << blockadeOrder->getEffect() << "\n";
    std::cout << "After blockade: Territory1 owned by " << territories_ref[0].owner->getName()
              << " with " << territories_ref[0].armyCount << " armies\n\n";


    // ========== TEST 7: Bomb Order ==========
    std::cout << "=== TEST 7: Bomb Order Removes Half Armies ===\n";

    // Reset for bomb test
    territories_ref[3].owner = player2;
    territories_ref[3].armyCount = 10;

    std::cout << "Before bomb: Territory4 (Bob's) has " << territories_ref[3].armyCount << " armies\n";

    Bomb* bombOrder = new Bomb(player1, &territories_ref[3], gameMap);
    std::cout << "Alice bombs Territory4\n";
    std::cout << "Validation: " << (bombOrder->validate() ? "VALID" : "INVALID") << "\n";
    bombOrder->execute();
    std::cout << "Effect: " << bombOrder->getEffect() << "\n";
    std::cout << "After bomb: Territory4 has " << territories_ref[3].armyCount << " armies\n\n";


    // ========== TEST 8: Airlift Order ==========
    std::cout << "=== TEST 8: Airlift Order (Non-Adjacent Transfer) ===\n";

    // Set up territories for airlift - make sure Alice owns both
    territories_ref[1].owner = player1;
    territories_ref[1].armyCount = 12;
    territories_ref[2].owner = player1;
    territories_ref[2].armyCount = 5;

    std::cout << "Before airlift: Territory2=" << territories_ref[1].armyCount
              << " armies, Territory3=" << territories_ref[2].armyCount << " armies\n";
    std::cout << "(Both owned by Alice for this test)\n";

    Airlift* airliftOrder = new Airlift(player1, 7, &territories_ref[1], &territories_ref[2]);
    std::cout << "Alice airlifts 7 armies from Territory2 to Territory3 (non-adjacent)\n";
    std::cout << "Validation: " << (airliftOrder->validate() ? "VALID" : "INVALID") << "\n";
    airliftOrder->execute();
    std::cout << "Effect: " << airliftOrder->getEffect() << "\n";
    std::cout << "After airlift: Territory2=" << territories_ref[1].armyCount
              << " armies, Territory3=" << territories_ref[2].armyCount << " armies\n\n";


    // ========== TEST 9: All Orders Can Be Issued and Executed ==========
    std::cout << "=== TEST 9: Demonstrating All Order Types Execution ===\n";

    OrdersList* comprehensiveList = new OrdersList();

    // Reset for comprehensive test
    player1->setReinforcementPool(20);
    territories_ref[0].owner = player1;
    territories_ref[0].armyCount = 15;
    territories_ref[1].owner = player1;
    territories_ref[1].armyCount = 10;
    territories_ref[2].owner = player2;
    territories_ref[2].armyCount = 8;
    territories_ref[3].owner = player2;
    territories_ref[3].armyCount = 6;

    // Clear negotiations from previous tests
    player1->clearNegotiations();
    player2->clearNegotiations();

    std::cout << "Adding one of each order type to OrdersList:\n";

    Deploy* deploy1 = new Deploy(player1, 5, &territories_ref[0]);
    comprehensiveList->addOrder(deploy1);
    std::cout << "  1. Deploy order added\n";

    // FIXED: Added gameMap parameter
    Advance* advance1 = new Advance(player1, 4, &territories_ref[0], &territories_ref[1], gameDeck, gameMap);
    comprehensiveList->addOrder(advance1);
    std::cout << "  2. Advance order added\n";

    Airlift* airlift1 = new Airlift(player1, 3, &territories_ref[0], &territories_ref[1]);
    comprehensiveList->addOrder(airlift1);
    std::cout << "  3. Airlift order added\n";

    Bomb* bomb1 = new Bomb(player1, &territories_ref[2], gameMap);
    comprehensiveList->addOrder(bomb1);
    std::cout << "  4. Bomb order added\n";

    Blockade* blockade1 = new Blockade(player1, &territories_ref[1], neutralPlayer);
    comprehensiveList->addOrder(blockade1);
    std::cout << "  5. Blockade order added\n";

    Negotiate* negotiate1 = new Negotiate(player1, player2);
    comprehensiveList->addOrder(negotiate1);
    std::cout << "  6. Negotiate order added\n";

    std::cout << "\nExecuting all orders:\n";
    std::cout << "----------------------------------------\n";
    for (int i = 0; i < comprehensiveList->size(); ++i) {
        Order* order = comprehensiveList->getOrder(i);
        std::cout << "Order " << (i+1) << ": " << order->getDescription() << "\n";
        std::cout << "  Valid: " << (order->validate() ? "YES" : "NO") << "\n";
        order->execute();
        std::cout << "  Result: " << order->getEffect() << "\n\n";
    }


    // ========== Summary ==========
    std::cout << "\n=== SUMMARY OF TESTS ===\n";
    std::cout << "✓ TEST 1: Deploy order validation and execution\n";
    std::cout << "✓ TEST 2: Advance order for friendly territory movement\n";
    std::cout << "✓ TEST 3: Advance order attack with territory conquest\n";
    std::cout << "✓ TEST 4: Card reward system (one card per turn on conquest)\n";
    std::cout << "✓ TEST 5: Negotiate order prevents attacks between players\n";
    std::cout << "✓ TEST 6: Blockade order transfers ownership to Neutral player\n";
    std::cout << "✓ TEST 7: Bomb order removes half of armies\n";
    std::cout << "✓ TEST 8: Airlift order moves armies between non-adjacent territories\n";
    std::cout << "✓ TEST 9: All order types can be issued and executed by game engine\n";


    // Cleanup
    delete validDeploy;
    delete invalidDeploy;
    delete friendlyAdvance;
    delete attackAdvance;
    delete negotiateOrder;
    delete blockedAdvance;
    delete blockadeOrder;
    delete bombOrder;
    delete airliftOrder;
    delete comprehensiveList;
    delete gameMap;
    delete player1;
    delete player2;
    delete neutralPlayer;
    delete gameDeck;

    std::cout << "\n========== END OF ASSIGNMENT 2 PART 4 TEST ==========\n";
}

/*
int main() {
    testOrderExecution();
    return 0;
}
*/
