//
// Created by Rahath on 2025-09-09.
//
#include "Player.h"
#include "Orders.h"
#include "Cards.h"
#include "PlayerStrategies.h"
#include <algorithm>
#include <iostream>
using namespace std;

//parameterized constructor
Player::Player(const string& n)
        : name(new string(n)),
          ownedTerritories(new vector<Map::territoryNode*>()),
          hand(new Hand()),
          ordersList(new OrdersList()),
          reinforcementPool(0),
          //part 3 - Initialize new fields
          mapRef(nullptr),
          deckRef(nullptr),
          //part 4 - Initialize new fields
          cannotAttackPlayers(new set<Player*>()),
          conqueredTerritoryThisTurn(new bool(false)),
          //Part 1 - Initialize strategy pointer
          strategy(nullptr) {
    cout << "[Player] Created player '" << *name << "'\n";
}

//copy constructor
Player::Player(const Player& other)
        : name(new string(*other.name)),
          ownedTerritories(new vector<Map::territoryNode*>()),
          hand(new Hand(*other.hand)),
          ordersList(new OrdersList(*other.ordersList)),
          reinforcementPool(other.reinforcementPool),
          //part 4- Copy new fields
          cannotAttackPlayers(new set<Player*>(*other.cannotAttackPlayers)),
          conqueredTerritoryThisTurn(new bool(*other.conqueredTerritoryThisTurn)),
          //Part 1 - Strategy not copied (caller must set explicitly)
          strategy(nullptr) {

    //copy territories (shallow)
    for (auto* t : *other.ownedTerritories) {
        ownedTerritories->push_back(t);
    }

    cout << "[Player] Copied player '" << *name << "'\n";
}

//assignment operator
Player& Player::operator=(const Player& other) {
    if (this != &other) {
        *name = *other.name;

        delete ownedTerritories;
        delete hand;
        delete ordersList;
        //part4 - Delete new fields
        delete cannotAttackPlayers;
        delete conqueredTerritoryThisTurn;

        ownedTerritories = new vector<Map::territoryNode*>(*other.ownedTerritories);
        hand = new Hand(*other.hand);
        ordersList = new OrdersList(*other.ordersList);
        reinforcementPool = other.reinforcementPool;
        //part 4-copy new fields
        cannotAttackPlayers = new set<Player*>(*other.cannotAttackPlayers);
        conqueredTerritoryThisTurn = new bool(*other.conqueredTerritoryThisTurn);
        //Part 1 - Strategy not copied (caller must set explicitly)
        strategy = nullptr;
    }
    return *this;
}

//destructor
Player::~Player() {
    cout << "Destroying player object: " << *name << "\n";
    delete name;
    delete ownedTerritories;
    delete hand;
    delete ordersList;
    //part 4 - Delete new fields
    delete cannotAttackPlayers;
    delete conqueredTerritoryThisTurn;
}

//issueOrder() method adds the given order to the player's OrdersList
void Player::issueOrder(Order* order) {
    if (!order) return;
    ordersList->addOrder(order);
    cout << "[Player::issueOrder] " << *name
         << " issued: " << *order << endl;
}

void Player::issueOrder() {
    // Part 1: Delegate to strategy if available
    if (strategy) {
        strategy->issueOrder();
    } else {
        cout << "[Player::issueOrder] No strategy set for player " << *name << ". Cannot issue orders.\n";
    }
}


//addTerritory() method: adds a territory to a players owned territories
void Player::addTerritory(Map::territoryNode* t) {
    if (t) ownedTerritories->push_back(t);
}

//addCard() method: adds a card to a player's hand
void Player::addCard(Card* c) {
    if (c) hand->addCard(c);
}

//toDefend() method: returns a list of territories that are to be defended
//Part 1: Delegate to strategy if available
vector<Map::territoryNode*> Player::toDefend() const {
    if (strategy) {
        return strategy->toDefend();
    }
    // Fallback: return all owned territories
    cout << "[Player::toDefend] No strategy set; returning all owned territories.\n";
    return ownedTerritories ? *ownedTerritories : vector<Map::territoryNode*>();
}


//toAttack() method: returns a list of territories that are to be attacked
//Part 1: Delegate to strategy if available
vector<Map::territoryNode*> Player::toAttack() const {
    if (strategy) {
        return strategy->toAttack();
    }
    // Fallback: return empty list
    cout << "[Player::toAttack] No strategy set; returning empty attack list.\n";
    return vector<Map::territoryNode*>();
}

// Getters
string Player::getName() const { return *name; }
const vector<Map::territoryNode*>* Player::getOwnedTerritories() const { return ownedTerritories; }
OrdersList* Player::getOrdersList() const { return ordersList; }
Hand* Player::getHand() const { return hand; }

int Player::getReinforcementPool() const {
    return reinforcementPool;
}

void Player::setReinforcementPool(int armies) {
    reinforcementPool = armies < 0 ? 0 : armies;
}

void Player::addReinforcements(int armies) {
    if (armies > 0) {
        reinforcementPool += armies;
    }
}

void Player::removeFromReinforcementPool(int armies) {
    if (armies <= 0) return;
    reinforcementPool -= armies;
    if (reinforcementPool < 0) reinforcementPool = 0;
}

void Player::clearTerritories() {
    ownedTerritories->clear();
}

//part 4- Negotiation management
void Player::addNegotiatedPlayer(Player* p) {
    if (p) {
        cannotAttackPlayers->insert(p);
    }
}

bool Player::isNegotiatedWith(Player* p) const {
    return cannotAttackPlayers->find(p) != cannotAttackPlayers->end();
}

void Player::clearNegotiations() {
    cannotAttackPlayers->clear();
}

//part 4-territory conquest tracking
void Player::setConqueredThisTurn(bool conquered) {
    *conqueredTerritoryThisTurn = conquered;
}

bool Player::hasConqueredThisTurn() const {
    return *conqueredTerritoryThisTurn;
}

//Part 1- Strategy pattern: set and get strategy
void Player::setStrategy(PlayerStrategy* newStrategy) {
    strategy = newStrategy;
    if (strategy) {
        strategy->setPlayer(this);
    }
}

PlayerStrategy* Player::getStrategy() const {
    return strategy;
}

//stream insertion operator
ostream& operator<<(ostream& os, const Player& p) {
    os << "Player:" << *p.name
       << ", Number of owned territories: " << p.ownedTerritories->size()
       << ", Number of cards in hand: " << p.hand->size()
       << ", Number of orders: " << p.ordersList->size()
       << ", Reinforcement pool: " << p.reinforcementPool;  // THEIR FORMAT
    return os;
}