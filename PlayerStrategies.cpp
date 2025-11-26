//
// Created by Aidan Catriel on 2025-11-19.
//

#include "PlayerStrategies.h"
#include "Map.h"
#include "Orders.h"
#include "Cards.h"
#include <iostream>
#include <algorithm>
#include <limits>
using namespace std;

// Human strategy: requires user interactions to make decisions
vector<Map::territoryNode*> HumanPlayerStrategy::toDefend() {
    if (!p || !p->getOwnedTerritories()) return {};
    return *p->getOwnedTerritories();
}

vector<Map::territoryNode*> HumanPlayerStrategy::toAttack() {
    vector<Map::territoryNode*> result;
    if (!p || !p->getMap() || !p->getOwnedTerritories()) return result;

    auto& nodes = p->getMap()->getTerritoryNodes();
    for (auto* owned : *p->getOwnedTerritories()) {
        for (int idx : owned->adjacentIndices) {
            Map::territoryNode* neigh = const_cast<Map::territoryNode*>(&nodes[idx]);
            if (!neigh || neigh->owner == p) continue;
            if (p->isNegotiatedWith(neigh->owner)) continue;
            if (find(result.begin(), result.end(), neigh) == result.end())
                result.push_back(neigh);
        }
    }
    return result;
}

static int readChoice(int minVal, int maxVal) {
    int choice;
    while (true) {
        cout << "> ";
        if (cin >> choice && choice >= minVal && choice <= maxVal) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        }
        cout << "Invalid choice. Try again.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void HumanPlayerStrategy::issueOrder() {
    if (!p) return;

    cout << "\n[HumanPlayerStrategy] Player " << p->getName()
         << " issuing orders. Reinforcement pool = "
         << p->getReinforcementPool() << "\n";

    // 1) DEPLOY
    auto defendList = toDefend();
    if (!defendList.empty() && p->getReinforcementPool() > 0) {
        cout << "Choose a territory to DEPLOY armies to:\n";
        for (size_t i = 0; i < defendList.size(); ++i) {
            cout << "  " << i << ") " << defendList[i]->name
                 << " (armies=" << defendList[i]->armyCount << ")\n";
        }
        int idx = readChoice(0, static_cast<int>(defendList.size() - 1));

        cout << "How many armies to deploy (0.." << p->getReinforcementPool() << ")?\n";
        int amount = readChoice(0, p->getReinforcementPool());

        if (amount > 0) {
            p->getOrdersList()->addOrder(new Deploy(p, amount, defendList[idx]));
            p->removeFromReinforcementPool(amount);
            cout << "  -> Deploy(" << amount << ", " << defendList[idx]->name << ")\n";
        }
    }

    // 2) ADVANCE
    auto attackList = toAttack();
    auto* owned = p->getOwnedTerritories();
    if (owned && !owned->empty()) {
        cout << "\nDo you want to issue an ADVANCE order? (1=yes, 0=no)\n";
        int yesNo = readChoice(0, 1);
        if (yesNo == 1) {
            cout << "Choose source territory index:\n";
            for (size_t i = 0; i < owned->size(); ++i) {
                cout << "  " << i << ") " << (*owned)[i]->name
                     << " (armies=" << (*owned)[i]->armyCount << ")\n";
            }
            int sIdx = readChoice(0, static_cast<int>(owned->size() - 1));

            cout << "Advance to (0=owned defend, 1=enemy attack)?\n";
            int mode = readChoice(0, 1);

            Map::territoryNode* target = nullptr;
            if (mode == 0) {
                auto defend = toDefend();
                if (!defend.empty()) {
                    cout << "Choose defend target index:\n";
                    for (size_t i = 0; i < defend.size(); ++i) {
                        cout << "  " << i << ") " << defend[i]->name
                             << " (armies=" << defend[i]->armyCount << ")\n";
                    }
                    int tIdx = readChoice(0, static_cast<int>(defend.size() - 1));
                    target = defend[tIdx];
                }
            } else {
                if (!attackList.empty()) {
                    cout << "Choose attack target index:\n";
                    for (size_t i = 0; i < attackList.size(); ++i) {
                        cout << "  " << i << ") " << attackList[i]->name
                             << " (armies=" << attackList[i]->armyCount << ")\n";
                    }
                    int tIdx = readChoice(0, static_cast<int>(attackList.size() - 1));
                    target = attackList[tIdx];
                }
            }

            if (target) {
                Map::territoryNode* source = (*owned)[sIdx];
                cout << "How many armies to advance (1.." << source->armyCount - 1 << ")?\n";
                int maxMove = max(1, source->armyCount - 1);
                int amount = readChoice(1, maxMove);

                p->getOrdersList()->addOrder(
                    new Advance(p, amount, source, target, p->getDeck(), p->getMap())
                );
                cout << "  -> Advance(" << amount << ", "
                     << source->name << " -> " << target->name << ")\n";
            }
        }
    }

    // 3) CARD
    if (p->getHand() && p->getHand()->size() > 0) {
        cout << "\nDo you want to PLAY a card? (1=yes, 0=no)\n";
        int yesNo = readChoice(0, 1);
        if (yesNo == 1) {
            auto* cards = p->getHand()->getCards();
            cout << "Choose card index to play:\n";
            for (size_t i = 0; i < cards->size(); ++i) {
                cout << "  " << i << ") " << cards->at(i)->getType() << "\n";
            }
            int cIdx = readChoice(0, static_cast<int>(cards->size() - 1));
            Card* c = cards->at(cIdx);
            c->play(p, p->getDeck(), p->getHand());
        }
    }

    cout << "[HumanPlayerStrategy] " << p->getName() << " finished issuing orders.\n\n";
}

// Aggressive strategy: Deploy or advance armies on the strongest territory, then always enemy territories. Use any cards with aggressive behaviour.
void AggressivePlayerStrategy::issueOrder() {
    toDefend();
    toAttack();

    // Use any Aggressive cards in hand (e.g., Airlift, Bomb).
    if (p->getHand() && p->getHand()->size() > 0) {
        vector<int> cardsToPlay;
        for (size_t i = 0; i < p->getHand()->size(); ++i) {
            Card* card = p->getHand()->getCards()->at(i);
            if (card->getType() == "airlift" || card->getType() == "bomb") {
                cardsToPlay.push_back(i);
            }
        }

        // Play the identified cards.
        for (int index : cardsToPlay) {
            Card* card = p->getHand()->getCards()->at(index);
            if (p->getDeck()) {
                card->play(p, p->getDeck(), p->getHand());
            } else {
                p->getHand()->removeCard(card);
            }
        }
    }
}

vector<Map::territoryNode*> AggressivePlayerStrategy::toAttack() {
    vector<Map::territoryNode*> result;
    if (!p || !p->getMap() || !p->getOwnedTerritories()) return result;

    // For each owned territory: if it has an adjacent enemy territory, advance all its enemies to the strongest one.
    auto& nodes = p->getMap()->getTerritoryNodes();
    for (auto* src : *p->getOwnedTerritories()) {
        if (!src) continue;
        if (src->armyCount <= 1) continue;

        // Find the strongest adjacent enemy territory.
        Map::territoryNode* strongestEnemy = nullptr;
        int maxArmies = -1;

        for (int idx : src->adjacentIndices) {
            Map::territoryNode* adjacentTerritory = const_cast<Map::territoryNode*>(&nodes[idx]);
            if (!adjacentTerritory || adjacentTerritory->owner == p) continue;
            if (p->isNegotiatedWith(adjacentTerritory->owner)) continue;

            if (adjacentTerritory->armyCount > maxArmies) {
                maxArmies = adjacentTerritory->armyCount;
                strongestEnemy = adjacentTerritory;
            }
        }

        if (strongestEnemy) {
            int move = src->armyCount - 1; // Move all but one army.
            p->getOrdersList()->addOrder(new Advance(p, move, src, strongestEnemy, p->getDeck(), p->getMap()));
            if (find(result.begin(), result.end(), strongestEnemy) == result.end())
                result.push_back(strongestEnemy);
        }
    }
    return result;
}

vector<Map::territoryNode*> AggressivePlayerStrategy::toDefend() {
    vector<Map::territoryNode*> result;
    if (!p || !p->getOwnedTerritories()) return result;

    // Find the strongest territory.
    Map::territoryNode* strongest = nullptr;
    int maxArmies = -1;
    for (auto* t : *p->getOwnedTerritories()) {
        if (t->armyCount > maxArmies) {
            maxArmies = t->armyCount;
            strongest = t;
        }
    }

    // Add all reinforcements to the strongest territory.
    if (strongest && p->getReinforcementPool() > 0) {
        p->getOrdersList()->addOrder(new Deploy(p, p->getReinforcementPool(), strongest));
        p->setReinforcementPool(0);
        result.push_back(strongest);
    }
    return result;
}

// Benevolent strategy: Deploy or advance armies on the weakest territories, never attack. Use any cards with benevolent behaviour.
void BenevolentPlayerStrategy::issueOrder() {
    toDefend();
    // Don't attack.

    // Use any Benevolent cards in hand (e.g., Diplomacy, Blockade], Reinforcement).
    if (p->getHand() && p->getHand()->size() > 0) {
        vector<int> cardsToPlay;
        for (size_t i = 0; i < p->getHand()->size(); ++i) {
            Card* card = p->getHand()->getCards()->at(i);
            if (card->getType() == "diplomacy" || card->getType() == "blockade" || card->getType() == "reinforcement") {
                cardsToPlay.push_back(i);
            }
        }

        // Play the identified cards.
        for (int index : cardsToPlay) {
            Card* card = p->getHand()->getCards()->at(index);
            if (p->getDeck()) {
                card->play(p, p->getDeck(), p->getHand());
            } else {
                p->getHand()->removeCard(card);
            }
        }
    }
}

vector<Map::territoryNode*> BenevolentPlayerStrategy::toAttack() {
    // Benevolent players do not attack.
    cout << "[BenevolentPlayerStrategy::toAttack] Benevolent player " << p->getName() << " does not attack.\n";
    return vector<Map::territoryNode*>();
}

vector<Map::territoryNode*> BenevolentPlayerStrategy::toDefend() {
    vector<Map::territoryNode*> result;
    if (!p || !p->getOwnedTerritories()) return result;

    // Find the weakest territories and reinforce them.
    auto territories = *p->getOwnedTerritories();
    sort(territories.begin(), territories.end(),
         [](Map::territoryNode* a, Map::territoryNode* b){
             return a->armyCount < b->armyCount;
         });

    int reinforcements = p->getReinforcementPool();
    size_t index = 0;

    // Distribute reinforcements to the 3 weakest territories.
    Map::territoryNode* weakest1 = territories.size() > 0 ? territories[0] : nullptr;
    Map::territoryNode* weakest2 = territories.size() > 1 ? territories[1] : nullptr;
    Map::territoryNode* weakest3 = territories.size() > 2 ? territories[2] : nullptr;

    int sharePerTerritory = reinforcements / 3;
    int remainder = reinforcements % 3;

    // Allocate any remainder to the weakest territory.
    if (weakest1) {
        int toDeploy = sharePerTerritory + remainder;
        p->getOrdersList()->addOrder(new Deploy(p, toDeploy, weakest1));
        reinforcements -= toDeploy;
        remainder = max(0, remainder - 1);
        result.push_back(weakest1);
    }

    if (weakest2) {
        int toDeploy = sharePerTerritory;
        p->getOrdersList()->addOrder(new Deploy(p, toDeploy, weakest2));
        reinforcements -= toDeploy;
        result.push_back(weakest2);
    }

    if (weakest3) {
        int toDeploy = sharePerTerritory;
        p->getOrdersList()->addOrder(new Deploy(p, toDeploy, weakest3));
        reinforcements -= toDeploy;
        result.push_back(weakest3);
    }

    p->setReinforcementPool(reinforcements);
    return result;
}

// Neutral strategy: Does nothing. If attacked, will be converted into an aggressive player.
void NeutralPlayerStrategy::issueOrder() {
    
}

// Neutral: does nothing in both attack and defend.
vector<Map::territoryNode*> NeutralPlayerStrategy::toAttack() {
    // Neutral players do nothing on their own turn.
    cout << "[NeutralPlayerStrategy::toAttack] Neutral player "
              << p->getName() << " does not attack.\n";
    return vector<Map::territoryNode*>();
}

vector<Map::territoryNode*> NeutralPlayerStrategy::toDefend() {
    // Neutral players also do not actively defend (no orders issued).
    cout << "[NeutralPlayerStrategy::toDefend] Neutral player "
              << p->getName() << " does not defend.\n";
    return vector<Map::territoryNode*>();
}

// Cheater strategy: Automatically conquers all adjacent enemy territories each turn. Does not use cards.
void CheaterPlayerStrategy::issueOrder() {
    // Don't defend.
    toAttack();
}

vector<Map::territoryNode*> CheaterPlayerStrategy::toAttack() {
    vector<Map::territoryNode*> territoriesToConquer;
    if (!p || !p->getMap() || !p->getOwnedTerritories()) return territoriesToConquer;

    auto& nodes = p->getMap()->getTerritoryNodes();

    // Identify all adjacent enemy territories.
    for (auto* src : *p->getOwnedTerritories()) {
        if (!src) continue;

        for (int idx : src->adjacentIndices) {
            Map::territoryNode* adjacentTerritory = const_cast<Map::territoryNode*>(&nodes[idx]);
            if (!adjacentTerritory || adjacentTerritory->owner == p) continue;

            // Mark this territory for conquest (only once per territory).
            if (find(territoriesToConquer.begin(), territoriesToConquer.end(), adjacentTerritory) == territoriesToConquer.end()) {
                territoriesToConquer.push_back(adjacentTerritory);
            }
        }
    }

    // Conquer all identified territories (only once per turn).
    for (auto* target : territoriesToConquer) {
        // Conquer the territory with 1 more army than it has.
        int armiesToMove = target->armyCount + 1;
        // Find a source territory to move from
        Map::territoryNode* source = nullptr;
        for (auto* src : *p->getOwnedTerritories()) {
            for (int idx : src->adjacentIndices) {
                if (&nodes[idx] == target) {
                    source = src;
                    break;
                }
            }
            if (source) break;
        }
        if (source && source->armyCount > 1) {
            p->getOrdersList()->addOrder(new Advance(p, min(armiesToMove, source->armyCount - 1), source, target, p->getDeck(), p->getMap()));
        }
    }
    return territoriesToConquer;
}

// Cheater: no defensive logic, only auto-conquer in toAttack().
vector<Map::territoryNode*> CheaterPlayerStrategy::toDefend() {
    // Cheater focuses only on aggressive auto-conquest; no defensive orders.
    cout << "[CheaterPlayerStrategy::toDefend] Cheater player "
              << p->getName() << " does not defend.\n";
    return vector<Map::territoryNode*>();
}