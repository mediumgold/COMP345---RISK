//
// Created by Rahath on 2025-09-09.
//
#include "Player.h"
#include "Orders.h"
#include "Cards.h"
#include <algorithm>
#include <iostream>
using namespace std;

//parameterized constructor
Player::Player(const string& n)
        : name(new string(n)),
          ownedTerritories(new vector<Map::territoryNode*>()),
          hand(new Hand()),
          ordersList(new OrdersList()),
          reinforcementPool(0) {
    cout << "[Player] Created player '" << *name << "'\n";
}

//copy constructor
Player::Player(const Player& other)
        : name(new string(*other.name)),
          ownedTerritories(new vector<Map::territoryNode*>()),
          hand(new Hand(*other.hand)),
          ordersList(new OrdersList(*other.ordersList)),
          reinforcementPool(other.reinforcementPool) {

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

        ownedTerritories = new vector<Map::territoryNode*>(*other.ownedTerritories);
        hand = new Hand(*other.hand);
        ordersList = new OrdersList(*other.ordersList);
        reinforcementPool = other.reinforcementPool;
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
}

//issueOrder() method adds the given order to the player’s OrdersList
void Player::issueOrder(Order* order) {
    if (!order) return;
    ordersList->addOrder(order);
    cout << "[Player::issueOrder] " << *name
         << " issued: " << *order << endl;
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
//Currently arbitrary
vector<Map::territoryNode*> Player::toDefend() const {
    cout << "[Player::toDefend] " << *name << " chooses territories to defend.\n";
    return *ownedTerritories;
}

//toAttack() method: returns a list of territories that are to be attacked
//Currently arbitrary
vector<Map::territoryNode*> Player::toAttack() const {
    cout << "[Player::toAttack] " << *name << " chooses territories to attack.\n";
    return *ownedTerritories;
}

// Getters
string Player::getName() const { return *name; }
const vector<Map::territoryNode*>* Player::getOwnedTerritories() const { return ownedTerritories; }
OrdersList* Player::getOrdersList() const { return ordersList; }
Hand* Player::getHand() const { return hand; }
int Player::getReinforcementPool() const { return reinforcementPool; }

void Player::setReinforcementPool(int armies) {
    reinforcementPool = armies < 0 ? 0 : armies;
}

void Player::addReinforcements(int armies) {
    if (armies > 0) {
        reinforcementPool += armies;
    }
}

void Player::clearTerritories() {
    ownedTerritories->clear();
}

//stream insertion operator
ostream& operator<<(ostream& os, const Player& p) {
    os << "Player:" << *p.name
       << ", Number of owned territories: " << p.ownedTerritories->size()
       << ", Number of cards in hand: " << p.hand->size()
       << ", Number of orders: " << p.ordersList->size()
       << ", Reinforcement pool: " << p.reinforcementPool;
    return os;
}
