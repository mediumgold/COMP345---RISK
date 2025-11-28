//
// Created by Aidan Catriel on 2025-11-19.
// COMPLETE IMPLEMENTATION
// FIXED: Aggressive now attacks from ANY border territory, not just strongest
//

#include "PlayerStrategies.h"
#include "Player.h"
#include "Map.h"
#include "Orders.h"
#include "Cards.h"
#include <algorithm>
#include <iostream>
#include <limits>

using namespace std;

// ==================== HUMAN PLAYER STRATEGY ====================

void HumanPlayerStrategy::issueOrder() {
    if (!p) {
        cout << "[HumanPlayerStrategy] No player assigned.\n";
        return;
    }

    cout << "\n=== " << p->getName() << "'s Turn (Human Player) ===\n";
    cout << "Reinforcement Pool: " << p->getReinforcementPool() << "\n";

    // Show owned territories
    auto owned = p->getOwnedTerritories();
    if (!owned || owned->empty()) {
        cout << "No territories owned!\n";
        return;
    }

    cout << "\nYour territories:\n";
    for (size_t i = 0; i < owned->size(); ++i) {
        cout << "  " << i + 1 << ". " << (*owned)[i]->name
             << " (Armies: " << (*owned)[i]->armyCount << ")\n";
    }

    // Deploy reinforcements
    while (p->getReinforcementPool() > 0) {
        cout << "\nReinforcements remaining: " << p->getReinforcementPool() << "\n";
        cout << "Enter territory number to deploy to (0 to stop): ";

        int choice;
        cin >> choice;

        if (choice == 0) break;
        if (choice < 1 || choice > owned->size()) {
            cout << "Invalid choice.\n";
            continue;
        }

        cout << "Enter number of armies to deploy: ";
        int armies;
        cin >> armies;

        if (armies <= 0 || armies > p->getReinforcementPool()) {
            cout << "Invalid number of armies.\n";
            continue;
        }

        p->issueOrder(new Deploy(p, armies, (*owned)[choice - 1]));
        p->removeFromReinforcementPool(armies);
    }

    // Ask about advance orders
    cout << "\nIssue advance orders? (y/n): ";
    char response;
    cin >> response;

    if (response == 'y' || response == 'Y') {
        cout << "Enter source territory number: ";
        int source;
        cin >> source;

        if (source >= 1 && source <= owned->size()) {
            Map::territoryNode* sourceTerr = (*owned)[source - 1];

            // Show adjacent territories
            Map* map = p->getMap();
            if (map) {
                auto& allTerr = map->getTerritoryNodes();
                cout << "\nAdjacent territories:\n";
                for (int adjIdx : sourceTerr->adjacentIndices) {
                    cout << "  " << allTerr[adjIdx].name
                         << " (Owner: " << (allTerr[adjIdx].owner ? allTerr[adjIdx].owner->getName() : "None")
                         << ", Armies: " << allTerr[adjIdx].armyCount << ")\n";
                }

                cout << "Enter target territory name: ";
                string targetName;
                cin.ignore();
                getline(cin, targetName);

                // Find target
                Map::territoryNode* target = nullptr;
                for (int adjIdx : sourceTerr->adjacentIndices) {
                    if (allTerr[adjIdx].name == targetName) {
                        target = &allTerr[adjIdx];
                        break;
                    }
                }

                if (target) {
                    cout << "Enter number of armies to move: ";
                    int armies;
                    cin >> armies;

                    if (armies > 0 && armies <= sourceTerr->armyCount) {
                        p->issueOrder(new Advance(p, armies, sourceTerr, target, p->getDeck(), map));
                    }
                }
            }
        }
    }

    // Ask about playing cards
    if (p->getHand() && p->getHand()->size() > 0) {
        cout << "\nYou have " << p->getHand()->size() << " cards. Play a card? (y/n): ";
        cin >> response;

        if (response == 'y' || response == 'Y') {
            auto cards = p->getHand()->getCards();
            cout << "Your cards:\n";
            for (size_t i = 0; i < cards->size(); ++i) {
                cout << "  " << i + 1 << ". " << (*cards)[i]->getType() << "\n";
            }

            cout << "Enter card number to play (0 to skip): ";
            int cardChoice;
            cin >> cardChoice;

            if (cardChoice >= 1 && cardChoice <= cards->size()) {
                (*cards)[cardChoice - 1]->play(p, p->getDeck(), p->getHand());
            }
        }
    }
}

vector<Map::territoryNode*> HumanPlayerStrategy::toDefend() {
    // Return all owned territories (human decides priority)
    auto owned = p->getOwnedTerritories();
    if (!owned) return vector<Map::territoryNode*>();
    return *owned;
}

vector<Map::territoryNode*> HumanPlayerStrategy::toAttack() {
    // Return all enemy adjacent territories
    vector<Map::territoryNode*> attackList;

    if (!p) return attackList;

    Map* map = p->getMap();
    auto owned = p->getOwnedTerritories();

    if (!map || !owned) return attackList;

    auto& allTerr = map->getTerritoryNodes();

    for (auto* territory : *owned) {
        for (int adjIdx : territory->adjacentIndices) {
            Map::territoryNode* adjTerr = &allTerr[adjIdx];
            if (adjTerr->owner != p) {
                // Add if not already in list
                if (find(attackList.begin(), attackList.end(), adjTerr) == attackList.end()) {
                    attackList.push_back(adjTerr);
                }
            }
        }
    }

    return attackList;
}

// ==================== AGGRESSIVE PLAYER STRATEGY ====================

void AggressivePlayerStrategy::issueOrder() {
    if (!p) {
        cout << "[AggressivePlayerStrategy] No player assigned.\n";
        return;
    }

    cout << "\n=== " << p->getName() << "'s Turn (Aggressive) ===\n";

    auto owned = p->getOwnedTerritories();
    if (!owned || owned->empty()) return;

    // Find strongest territory
    Map::territoryNode* strongest = nullptr;
    int maxArmies = -1;
    for (auto* terr : *owned) {
        if (terr->armyCount > maxArmies) {
            maxArmies = terr->armyCount;
            strongest = terr;
        }
    }
    if (!strongest) return;

    // Deploy to strongest
    int virtualArmyCount = strongest->armyCount;
    if (p->getReinforcementPool() > 0) {
        int armies = p->getReinforcementPool();
        cout << "[Aggressive] Deploying " << armies << " to " << strongest->name << "\n";
        p->issueOrder(new Deploy(p, armies, strongest));
        p->removeFromReinforcementPool(armies);
        virtualArmyCount += armies;
    }

    // SIMPLE ATTACK LOGIC: Attack from STRONGEST (with virtual armies) OR from ANY border
    Map* map = p->getMap();
    if (!map) return;

    auto& allTerr = map->getTerritoryNodes();

    // Try strongest first
    if (virtualArmyCount > 1) {
        for (int adjIdx : strongest->adjacentIndices) {
            Map::territoryNode* target = &allTerr[adjIdx];
            if (target->owner != p) {
                int armiesToMove = virtualArmyCount - 1;
                cout << "[Aggressive] Advancing " << armiesToMove << " from " << strongest->name << " to " << target->name << "\n";
                p->issueOrder(new Advance(p, armiesToMove, strongest, target, p->getDeck(), map));
                return;  // Done!
            }
        }
    }

    // Strongest has no enemies - attack from STRONGEST territory with enemies, regardless of army count
    Map::territoryNode* attackFrom = nullptr;
    Map::territoryNode* attackTarget = nullptr;
    int maxBorderArmies = -1;

    // Find the territory with MOST armies that has an adjacent enemy
    for (auto* terr : *owned) {
        for (int adjIdx : terr->adjacentIndices) {
            Map::territoryNode* target = &allTerr[adjIdx];
            if (target->owner != p && terr->armyCount > maxBorderArmies) {
                maxBorderArmies = terr->armyCount;
                attackFrom = terr;
                attackTarget = target;
            }
        }
    }

    // Attack even with 0 armies (it will fail, but that's OK - at least we try!)
    if (attackFrom && attackTarget) {
        int armiesToMove = (attackFrom->armyCount > 0) ? attackFrom->armyCount : 1;
        cout << "[Aggressive] Advancing " << armiesToMove << " from " << attackFrom->name << " to " << attackTarget->name << "\n";
        p->issueOrder(new Advance(p, armiesToMove, attackFrom, attackTarget, p->getDeck(), map));
    }

    // Use cards
    if (p->getHand()) {
        auto cards = p->getHand()->getCards();
        for (auto* card : *cards) {
            if (card->getType() == "bomb") {
                cout << "[Aggressive] Playing bomb card\n";
                card->play(p, p->getDeck(), p->getHand());
                break;
            }
        }
    }
}

vector<Map::territoryNode*> AggressivePlayerStrategy::toDefend() {
    // Return strongest territory first
    auto owned = p->getOwnedTerritories();
    if (!owned || owned->empty()) return vector<Map::territoryNode*>();

    vector<Map::territoryNode*> defendList = *owned;

    // Sort by army count (descending)
    sort(defendList.begin(), defendList.end(),
         [](Map::territoryNode* a, Map::territoryNode* b) {
             return a->armyCount > b->armyCount;
         });

    return defendList;
}

vector<Map::territoryNode*> AggressivePlayerStrategy::toAttack() {
    // Return all enemy adjacent territories
    vector<Map::territoryNode*> attackList;

    if (!p) return attackList;

    Map* map = p->getMap();
    auto owned = p->getOwnedTerritories();

    if (!map || !owned) return attackList;

    auto& allTerr = map->getTerritoryNodes();

    for (auto* territory : *owned) {
        for (int adjIdx : territory->adjacentIndices) {
            Map::territoryNode* adjTerr = &allTerr[adjIdx];
            if (adjTerr->owner != p) {
                if (find(attackList.begin(), attackList.end(), adjTerr) == attackList.end()) {
                    attackList.push_back(adjTerr);
                }
            }
        }
    }

    return attackList;
}

// ==================== BENEVOLENT PLAYER STRATEGY ====================

void BenevolentPlayerStrategy::issueOrder() {
    if (!p) {
        cout << "[BenevolentPlayerStrategy] No player assigned.\n";
        return;
    }

    cout << "\n=== " << p->getName() << "'s Turn (Benevolent) ===\n";

    auto owned = p->getOwnedTerritories();
    if (!owned || owned->empty()) return;

    // Find weakest territory (fewest armies)
    Map::territoryNode* weakest = nullptr;
    int minArmies = numeric_limits<int>::max();

    for (auto* terr : *owned) {
        if (terr->armyCount < minArmies) {
            minArmies = terr->armyCount;
            weakest = terr;
        }
    }

    // Deploy all reinforcements to weakest territory
    if (p->getReinforcementPool() > 0 && weakest) {
        int armies = p->getReinforcementPool();
        cout << "[Benevolent] Deploying " << armies << " to weakest territory " << weakest->name << "\n";
        p->issueOrder(new Deploy(p, armies, weakest));
        p->removeFromReinforcementPool(armies);
    }

    // Move armies from strong territories to weak ones
    if (owned->size() > 1) {
        // Find strongest
        Map::territoryNode* strongest = nullptr;
        int maxArmies = -1;

        for (auto* terr : *owned) {
            if (terr->armyCount > maxArmies && terr != weakest) {
                maxArmies = terr->armyCount;
                strongest = terr;
            }
        }

        // Advance from strongest to weakest if adjacent
        if (strongest && weakest && strongest->armyCount > 1) {
            Map* map = p->getMap();
            if (map) {
                // Check if they are adjacent
                bool adjacent = false;
                for (int adjIdx : strongest->adjacentIndices) {
                    if (&map->getTerritoryNodes()[adjIdx] == weakest) {
                        adjacent = true;
                        break;
                    }
                }

                if (adjacent) {
                    int armiesToMove = strongest->armyCount / 2; // Move half
                    if (armiesToMove > 0) {
                        cout << "[Benevolent] Moving " << armiesToMove
                             << " from " << strongest->name << " to " << weakest->name << "\n";
                        p->issueOrder(new Advance(p, armiesToMove, strongest, weakest, p->getDeck(), map));
                    }
                }
            }
        }
    }

    // Never use aggressive cards, may use defensive ones (blockade)
    if (p->getHand()) {
        auto cards = p->getHand()->getCards();
        for (auto* card : *cards) {
            if (card->getType() == "reinforcement" || card->getType() == "diplomacy") {
                cout << "[Benevolent] Playing " << card->getType() << " card\n";
                card->play(p, p->getDeck(), p->getHand());
                break;
            }
        }
    }
}

vector<Map::territoryNode*> BenevolentPlayerStrategy::toDefend() {
    // Return weakest territories first
    auto owned = p->getOwnedTerritories();
    if (!owned || owned->empty()) return vector<Map::territoryNode*>();

    vector<Map::territoryNode*> defendList = *owned;

    // Sort by army count (ascending)
    sort(defendList.begin(), defendList.end(),
         [](Map::territoryNode* a, Map::territoryNode* b) {
             return a->armyCount < b->armyCount;
         });

    return defendList;
}

vector<Map::territoryNode*> BenevolentPlayerStrategy::toAttack() {
    // Never attacks - return empty list
    return vector<Map::territoryNode*>();
}

// ==================== NEUTRAL PLAYER STRATEGY ====================

void NeutralPlayerStrategy::issueOrder() {
    if (!p) {
        cout << "[NeutralPlayerStrategy] No player assigned.\n";
        return;
    }

    cout << "\n=== " << p->getName() << "'s Turn (Neutral) ===\n";
    cout << "[Neutral] Player does nothing.\n";

    // Do nothing - neutral player issues no orders
}

vector<Map::territoryNode*> NeutralPlayerStrategy::toDefend() {
    // Return empty - neutral doesn't defend
    return vector<Map::territoryNode*>();
}

vector<Map::territoryNode*> NeutralPlayerStrategy::toAttack() {
    // Return empty - neutral doesn't attack
    return vector<Map::territoryNode*>();
}

// ==================== CHEATER PLAYER STRATEGY ====================

void CheaterPlayerStrategy::issueOrder() {
    if (!p) {
        cout << "[CheaterPlayerStrategy] No player assigned.\n";
        return;
    }

    cout << "\n=== " << p->getName() << "'s Turn (Cheater) ===\n";

    Map* map = p->getMap();
    auto owned = p->getOwnedTerritories();

    if (!map || !owned || owned->empty()) return;

    // Automatically conquer all adjacent enemy territories
    auto& allTerr = map->getTerritoryNodes();
    vector<Map::territoryNode*> toConquer;

    for (auto* territory : *owned) {
        for (int adjIdx : territory->adjacentIndices) {
            Map::territoryNode* adjTerr = &allTerr[adjIdx];
            if (adjTerr->owner != p) {
                // Check if not already in list
                if (find(toConquer.begin(), toConquer.end(), adjTerr) == toConquer.end()) {
                    toConquer.push_back(adjTerr);
                }
            }
        }
    }

    // Conquer all identified territories
    for (auto* target : toConquer) {
        cout << "[Cheater] Automatically conquering " << target->name << "\n";

        // Remove from old owner
        if (target->owner) {
            auto* oldOwned = const_cast<vector<Map::territoryNode*>*>(target->owner->getOwnedTerritories());
            oldOwned->erase(remove(oldOwned->begin(), oldOwned->end(), target), oldOwned->end());
        }

        // Transfer ownership
        target->owner = p;
        p->addTerritory(target);
        target->armyCount = 1; // Set minimum armies
    }

    if (!toConquer.empty()) {
        p->setConqueredThisTurn(true);
    }
}

vector<Map::territoryNode*> CheaterPlayerStrategy::toDefend() {
    // Return all owned territories
    auto owned = p->getOwnedTerritories();
    if (!owned) return vector<Map::territoryNode*>();
    return *owned;
}

vector<Map::territoryNode*> CheaterPlayerStrategy::toAttack() {
    // Return all enemy adjacent territories (they will be conquered automatically)
    vector<Map::territoryNode*> attackList;

    if (!p) return attackList;

    Map* map = p->getMap();
    auto owned = p->getOwnedTerritories();

    if (!map || !owned) return attackList;

    auto& allTerr = map->getTerritoryNodes();

    for (auto* territory : *owned) {
        for (int adjIdx : territory->adjacentIndices) {
            Map::territoryNode* adjTerr = &allTerr[adjIdx];
            if (adjTerr->owner != p) {
                if (find(attackList.begin(), attackList.end(), adjTerr) == attackList.end()) {
                    attackList.push_back(adjTerr);
                }
            }
        }
    }

    return attackList;
}