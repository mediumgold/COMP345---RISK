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
            // FIX: Don't remove from pool here - Deploy::execute() handles it
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

// ============================================================================
// AGGRESSIVE PLAYER STRATEGY
// Focus on strongest territory, always attack
// Deploy/advance armies on strongest territory, then attack enemy territories
// ============================================================================

vector<Map::territoryNode*> AggressivePlayerStrategy::toDefend() {
    vector<Map::territoryNode*> result;
    if (!p || !p->getOwnedTerritories() || p->getOwnedTerritories()->empty()) return result;

    // Find the strongest territory (most armies)
    Map::territoryNode* strongest = nullptr;
    int maxArmies = -1;
    for (auto* t : *p->getOwnedTerritories()) {
        if (t && t->armyCount > maxArmies) {
            maxArmies = t->armyCount;
            strongest = t;
        }
    }

    if (strongest) {
        result.push_back(strongest);
    }
    return result;
}

vector<Map::territoryNode*> AggressivePlayerStrategy::toAttack() {
    vector<Map::territoryNode*> result;
    if (!p || !p->getMap() || !p->getOwnedTerritories()) return result;

    auto& nodes = p->getMap()->getTerritoryNodes();

    // Find ALL adjacent enemy territories from our owned territories
    for (auto* owned : *p->getOwnedTerritories()) {
        if (!owned) continue;
        for (int idx : owned->adjacentIndices) {
            Map::territoryNode* neigh = const_cast<Map::territoryNode*>(&nodes[idx]);
            if (!neigh || neigh->owner == p) continue;
            if (p->isNegotiatedWith(neigh->owner)) continue;
            if (find(result.begin(), result.end(), neigh) == result.end()) {
                result.push_back(neigh);
            }
        }
    }
    return result;
}

void AggressivePlayerStrategy::issueOrder() {
    if (!p || !p->getMap()) return;

    auto& nodes = p->getMap()->getTerritoryNodes();

    // Find the strongest territory that HAS adjacent enemies (for attacking)
    Map::territoryNode* attackSource = nullptr;
    Map::territoryNode* attackTarget = nullptr;
    int maxArmies = -1;

    for (auto* t : *p->getOwnedTerritories()) {
        if (!t) continue;

        // Check if this territory has any adjacent enemies
        Map::territoryNode* adjacentEnemy = nullptr;
        for (int idx : t->adjacentIndices) {
            Map::territoryNode* neigh = const_cast<Map::territoryNode*>(&nodes[idx]);
            if (!neigh || neigh->owner == p) continue;
            if (p->isNegotiatedWith(neigh->owner)) continue;
            adjacentEnemy = neigh;
            break;
        }

        // If this territory has enemies AND is stronger than current best, use it
        if (adjacentEnemy && t->armyCount > maxArmies) {
            maxArmies = t->armyCount;
            attackSource = t;
            attackTarget = adjacentEnemy;
        }
    }

    // If no territory with adjacent enemies found, just find strongest for deployment
    Map::territoryNode* deployTarget = attackSource;
    if (!deployTarget) {
        for (auto* t : *p->getOwnedTerritories()) {
            if (!deployTarget || (t && t->armyCount > deployTarget->armyCount)) {
                deployTarget = t;
            }
        }
    }

    if (!deployTarget) {
        cout << "[Aggressive] " << p->getName() << " has no territories.\n";
        return;
    }

    // Step 1: Deploy ALL reinforcements to the attack source (or strongest if no attack possible)
    int deployedAmount = 0;
    if (p->getReinforcementPool() > 0) {
        deployedAmount = p->getReinforcementPool();
        p->getOrdersList()->addOrder(new Deploy(p, deployedAmount, deployTarget));
        // FIX: Don't modify reinforcement pool - Deploy::execute() handles it
        cout << "[Aggressive] " << p->getName() << " deploys " << deployedAmount
             << " armies to " << deployTarget->name << endl;
    }

    // Step 2: Attack if we have a valid source and target
    if (attackSource && attackTarget) {
        // Calculate effective armies (current + pending deployment if same territory)
        int effectiveArmies = attackSource->armyCount;
        if (attackSource == deployTarget) {
            effectiveArmies += deployedAmount;
        }

        if (effectiveArmies > 1) {
            int moveArmies = effectiveArmies - 1;  // Leave 1 behind
            p->getOrdersList()->addOrder(
                new Advance(p, moveArmies, attackSource, attackTarget, p->getDeck(), p->getMap())
            );
            cout << "[Aggressive] " << p->getName() << " advances " << moveArmies
                 << " from " << attackSource->name << " to " << attackTarget->name << endl;
        }
    } else {
        cout << "[Aggressive] " << p->getName() << " has no adjacent enemies to attack.\n";
    }

    // Step 3: Play aggressive cards (bomb, airlift)
    // FIX: Collect cards first, then play (to avoid index invalidation)
    if (p->getHand() && p->getHand()->size() > 0) {
        vector<Card*> cardsToPlay;
        for (size_t i = 0; i < p->getHand()->size(); ++i) {
            Card* card = p->getHand()->getCards()->at(i);
            if (card->getType() == "airlift" || card->getType() == "bomb") {
                cardsToPlay.push_back(card);
            }
        }

        for (Card* card : cardsToPlay) {
            if (p->getDeck()) {
                card->play(p, p->getDeck(), p->getHand());
            }
        }
    }
}

// ============================================================================
// BENEVOLENT PLAYER STRATEGY
// Focus on weakest territories, never attack
// Deploy/advance armies on weakest territories, never attack
// ============================================================================

vector<Map::territoryNode*> BenevolentPlayerStrategy::toDefend() {
    vector<Map::territoryNode*> result;
    if (!p || !p->getOwnedTerritories() || p->getOwnedTerritories()->empty()) return result;

    // Return territories sorted by army count (weakest first)
    result = *p->getOwnedTerritories();
    sort(result.begin(), result.end(),
         [](Map::territoryNode* a, Map::territoryNode* b) {
             return a->armyCount < b->armyCount;
         });
    return result;
}

vector<Map::territoryNode*> BenevolentPlayerStrategy::toAttack() {
    // Benevolent players do not attack
    return vector<Map::territoryNode*>();
}

void BenevolentPlayerStrategy::issueOrder() {
    if (!p) return;

    // Step 1: Deploy reinforcements to weakest territories
    auto defendList = toDefend();
    int reinforcements = p->getReinforcementPool();

    if (!defendList.empty() && reinforcements > 0) {
        // Distribute to the 3 weakest territories
        int numTargets = min(3, (int)defendList.size());
        int sharePerTerritory = reinforcements / numTargets;
        int remainder = reinforcements % numTargets;

        for (int i = 0; i < numTargets; ++i) {
            int toDeploy = sharePerTerritory + (i == 0 ? remainder : 0);
            if (toDeploy > 0) {
                p->getOrdersList()->addOrder(new Deploy(p, toDeploy, defendList[i]));
                cout << "[Benevolent] " << p->getName() << " deploys " << toDeploy
                     << " armies to " << defendList[i]->name << endl;
            }
        }
        // FIX: Don't modify reinforcement pool - Deploy::execute() handles it
    }

    // Step 2: NO attacks (benevolent players never attack)

    // Step 3: Play benevolent cards (diplomacy, reinforcement)
    // FIX: Collect cards first, then play (to avoid index invalidation)
    if (p->getHand() && p->getHand()->size() > 0) {
        vector<Card*> cardsToPlay;
        for (size_t i = 0; i < p->getHand()->size(); ++i) {
            Card* card = p->getHand()->getCards()->at(i);
            if (card->getType() == "diplomacy" || card->getType() == "reinforcement") {
                cardsToPlay.push_back(card);
            }
            // Note: blockade is risky for benevolent, skip it
        }

        for (Card* card : cardsToPlay) {
            if (p->getDeck()) {
                card->play(p, p->getDeck(), p->getHand());
            }
        }
    }
}

// ============================================================================
// NEUTRAL PLAYER STRATEGY
// Does nothing. If attacked, becomes Aggressive (handled in Advance::execute)
// ============================================================================

vector<Map::territoryNode*> NeutralPlayerStrategy::toDefend() {
    // Neutral players don't actively defend, but need territories for deployment
    if (!p || !p->getOwnedTerritories()) return {};
    return *p->getOwnedTerritories();
}

vector<Map::territoryNode*> NeutralPlayerStrategy::toAttack() {
    // Neutral players do not attack
    return vector<Map::territoryNode*>();
}

void NeutralPlayerStrategy::issueOrder() {
    if (!p) return;

    // Neutral players must deploy their reinforcements (otherwise they accumulate)
    // But they don't attack or do anything else
    auto defendList = toDefend();
    int reinforcements = p->getReinforcementPool();

    if (!defendList.empty() && reinforcements > 0) {
        // Deploy evenly across all territories
        int perTerritory = reinforcements / defendList.size();
        int remainder = reinforcements % defendList.size();

        for (size_t i = 0; i < defendList.size(); ++i) {
            int toDeploy = perTerritory + (i < (size_t)remainder ? 1 : 0);
            if (toDeploy > 0) {
                p->getOrdersList()->addOrder(new Deploy(p, toDeploy, defendList[i]));
            }
        }
        // FIX: Don't modify reinforcement pool - Deploy::execute() handles it
    }

    cout << "[Neutral] " << p->getName() << " does nothing this turn.\n";
}

// ============================================================================
// CHEATER PLAYER STRATEGY
// Automatically conquers all adjacent enemy territories (no combat)
// ============================================================================

vector<Map::territoryNode*> CheaterPlayerStrategy::toDefend() {
    // Cheater doesn't focus on defense
    if (!p || !p->getOwnedTerritories()) return {};
    return *p->getOwnedTerritories();
}

vector<Map::territoryNode*> CheaterPlayerStrategy::toAttack() {
    vector<Map::territoryNode*> result;
    if (!p || !p->getMap() || !p->getOwnedTerritories()) return result;

    auto& nodes = p->getMap()->getTerritoryNodes();

    // Find all adjacent enemy territories
    for (auto* owned : *p->getOwnedTerritories()) {
        if (!owned) continue;
        for (int idx : owned->adjacentIndices) {
            Map::territoryNode* neigh = const_cast<Map::territoryNode*>(&nodes[idx]);
            if (!neigh || neigh->owner == p) continue;
            if (find(result.begin(), result.end(), neigh) == result.end()) {
                result.push_back(neigh);
            }
        }
    }
    return result;
}

void CheaterPlayerStrategy::issueOrder() {
    if (!p || !p->getMap()) return;

    // Step 1: Deploy reinforcements (even cheaters need to deploy)
    auto defendList = toDefend();
    if (!defendList.empty() && p->getReinforcementPool() > 0) {
        int amount = p->getReinforcementPool();
        p->getOrdersList()->addOrder(new Deploy(p, amount, defendList[0]));
        // FIX: Don't modify reinforcement pool - Deploy::execute() handles it
    }

    // Step 2: CHEAT - Automatically conquer all adjacent enemy territories
    // Cheater doesn't use Advance orders with combat - directly takes ownership
    auto& nodes = p->getMap()->getTerritoryNodes();
    vector<Map::territoryNode*> toConquer;

    // Collect all adjacent enemy territories
    for (auto* owned : *p->getOwnedTerritories()) {
        if (!owned) continue;
        for (int idx : owned->adjacentIndices) {
            Map::territoryNode* target = const_cast<Map::territoryNode*>(&nodes[idx]);
            if (!target || target->owner == p) continue;
            if (find(toConquer.begin(), toConquer.end(), target) == toConquer.end()) {
                toConquer.push_back(target);
            }
        }
    }

    // Conquer them directly (cheating!)
    for (auto* target : toConquer) {
        Player* previousOwner = target->owner;

        // Remove from previous owner's territory list
        if (previousOwner) {
            auto* prevTerritories = const_cast<vector<Map::territoryNode*>*>(previousOwner->getOwnedTerritories());
            if (prevTerritories) {
                prevTerritories->erase(
                    remove(prevTerritories->begin(), prevTerritories->end(), target),
                    prevTerritories->end()
                );
            }
        }

        // Add to cheater's territory list
        target->owner = p;
        p->addTerritory(target);

        // Set army count to 1 (conquered with minimal force)
        target->armyCount = 1;

        cout << "[Cheater] " << p->getName() << " automatically conquers "
             << target->name << " from "
             << (previousOwner ? previousOwner->getName() : "neutral") << "!\n";
    }

    if (toConquer.empty()) {
        cout << "[Cheater] " << p->getName() << " has no adjacent enemies to conquer.\n";
    }
}