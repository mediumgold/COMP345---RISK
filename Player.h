//
// Created by Rahath on 2025-09-09.
//

#ifndef COMP345_RISK_PLAYER_H
#define COMP345_RISK_PLAYER_H

#include "Map.h"    // required for Map::territoryNode
#include <iostream>
#include <vector>
#include <string>
#include <set>

// Forward declarations to avoid circular includes
class Order;
class OrdersList;
class Hand;
class Card;
class Deck;
class Map;

using namespace std;


class Player {
private:
    string* name;
    vector<Map::territoryNode*>* ownedTerritories;
    Hand* hand;
    OrdersList* ordersList;
    int reinforcementPool;

    Map* mapRef = nullptr;

    //allow Player to play a card using the shared deck
    Deck* deckRef = nullptr;

    // part 4- Track players we cannot attack this turn (negotiate)
    set<Player*>* cannotAttackPlayers;

    // part 4- Track if player conquered a territory this turn (for card reward)
    bool* conqueredTerritoryThisTurn;

public:
    Player(const string& n = "Player");
    Player(const Player& other);
    Player& operator=(const Player& other);
    ~Player();

    vector<Map::territoryNode*> toDefend() const;
    vector<Map::territoryNode*> toAttack() const;
    void issueOrder(Order* order);
    void issueOrder();

    void setMap(Map* m) { mapRef = m; }
    Map* getMap() const { return mapRef; }

    void setDeck(Deck* d) { deckRef = d; }
    Deck* getDeck() const { return deckRef; }

    void addTerritory(Map::territoryNode* t);
    void addCard(Card* c);

    string getName() const;
    const vector<Map::territoryNode*>* getOwnedTerritories() const;
    OrdersList* getOrdersList() const;
    Hand* getHand() const;
    int getReinforcementPool() const;
    void setReinforcementPool(int armies);
    void addReinforcements(int armies);
    void clearTerritories();

    // part 4- Additional method for removing reinforcements
    void removeFromReinforcementPool(int armies);

    // part 4- Negotiation management
    void addNegotiatedPlayer(Player* p);
    bool isNegotiatedWith(Player* p) const;
    void clearNegotiations();

    // part 4- Territory conquest tracking
    void setConqueredThisTurn(bool conquered);
    bool hasConqueredThisTurn() const;

    friend ostream& operator<<(ostream& os, const Player& p);
};

void testPlayers();

#endif // COMP345_RISK_PLAYER_H