//
// Created by Aidan Catriel on 2025-11-19.
//

#include "PlayerStrategies.h"
#include "Map.h"
#include "Orders.h"
#include "Cards.h"

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

void AggressivePlayerStrategy::toAttack() {
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

            if (adjacentTerritory->armyCount > maxArmies) {
                maxArmies = adjacentTerritory->armyCount;
                strongestEnemy = adjacentTerritory;
            }
        }

        if (strongestEnemy) {
            int move = src->armyCount - 1; // Move all but one army.
            p->getOrdersList()->addOrder(new Advance(p, move, src, strongestEnemy, p->getDeck(), p->getMap()));
        }
    }
}

void AggressivePlayerStrategy::toDefend() {
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
    }
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

void BenevolentPlayerStrategy::toAttack() {
    // Benevolent players do not attack.
    cout << "[BenevolentPlayerStrategy::toAttack] Benevolent player " << p->getName() << " does not attack.\n";
}

void BenevolentPlayerStrategy::toDefend() {
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
        remainder = std::max(0, remainder - 1);
    }

    if (weakest2) {
        int toDeploy = sharePerTerritory;
        p->getOrdersList()->addOrder(new Deploy(p, toDeploy, weakest2));
        reinforcements -= toDeploy;
    }

    if (weakest3) {
        int toDeploy = sharePerTerritory;
        p->getOrdersList()->addOrder(new Deploy(p, toDeploy, weakest3));
        reinforcements -= toDeploy;
    }

    p->setReinforcementPool(reinforcements);
}

// Neutral strategy: Does nothing. If attacked, will be converted into an aggressive player.
void NeutralPlayerStrategy::issueOrder() {
    
}

// Cheater strategy: Automatically conquers all adjacent enemy territories each turn. Does not use cards.
void CheaterPlayerStrategy::issueOrder() {
    // Don't defend.
    toAttack();
}

void CheaterPlayerStrategy::toAttack() {
    auto& nodes = p->getMap()->getTerritoryNodes();
    vector<Map::territoryNode*> territoriesToConquer;

    // Identify all adjacent enemy territories.
    for (auto* src : *p->getOwnedTerritories()) {
        if (!src) continue;

        for (int idx : src->adjacentIndices) {
            Map::territoryNode* adjacentTerritory = const_cast<Map::territoryNode*>(&nodes[idx]);
            if (!adjacentTerritory || adjacentTerritory->owner == p) continue;

            // Mark this territory for conquest.
            territoriesToConquer.push_back(adjacentTerritory);
        }
    }

    // Conquer all identified territories.
    for (auto* target : territoriesToConquer) {
        // Conquer the territory with 1 more army than it has.
        int armiesToMove = target->armyCount + 1;
        p->getOrdersList()->addOrder(new Advance(p, armiesToMove, nullptr, target, p->getDeck(), p->getMap()));
    }
}