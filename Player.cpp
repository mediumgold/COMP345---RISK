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
          reinforcementPool(0),
          //part 3 - Initialize new fields
          mapRef(nullptr),
          deckRef(nullptr),
          //part 4 - Initialize new fields
          cannotAttackPlayers(new set<Player*>()),
          conqueredTerritoryThisTurn(new bool(false)) {
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
          conqueredTerritoryThisTurn(new bool(*other.conqueredTerritoryThisTurn)) {

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
    cout << "[Player::issueOrder] " << *name << " begins issuing orders.\n";

    // 1) DEPLOY: plan to spend all reinforcements on toDefend() territories,
    // but do NOT touch reinforcementPool here. Let Deploy handle it.
    auto defendList = toDefend();
    size_t didx = 0;

    int planned = reinforcementPool;   // local copy to plan how much we want to deploy

    while (planned > 0 && !defendList.empty()) {
        Map::territoryNode* target = defendList[didx % defendList.size()];
        int drop = std::min(3, planned);          // chunks of 3 (or remaining)

        // rich constructor: player + territory node
        ordersList->addOrder(new Deploy(this, drop, target));

        planned -= drop;                          // we "plan" to spend this much
        cout << "  -> Deploy(" << drop << ", " << target->name
             << ")  plannedPool=" << planned << "\n";
        ++didx;
    }

    // 2) ADVANCE: as before (uses armies on territories, not the pool)
    if (!mapRef || !ownedTerritories || ownedTerritories->empty()) {
        cout << "  (No map or territories available for Advance orders)\n";
    } else {
        auto& nodes = mapRef->getTerritoryNodes();

        // Defensive advance (owned -> owned)
        bool defIssued = false;
        for (auto* src : *ownedTerritories) {
            if (!src) continue;
            if (src->armyCount <= 1) continue;

            for (int idx : src->adjacentIndices) {
                Map::territoryNode* dst = const_cast<Map::territoryNode*>(&nodes[idx]);
                if (!dst || dst->owner != this) continue;

                int move = std::min(2, src->armyCount - 1);
                if (move <= 0) continue;

                ordersList->addOrder(new Advance(this, move, src, dst, deckRef, mapRef));
                cout << "  -> Advance(" << move << ", " << src->name
                     << ", " << dst->name << ") [defend]\n";
                defIssued = true;
                break;
            }
            if (defIssued) break;
        }

        // Offensive advance (owned -> enemy/neutral)
        bool offIssued = false;
        for (auto* src : *ownedTerritories) {
            if (!src) continue;
            if (src->armyCount <= 1) continue;

            for (int idx : src->adjacentIndices) {
                Map::territoryNode* tgt = const_cast<Map::territoryNode*>(&nodes[idx]);
                if (!tgt) continue;

                Player* owner = tgt->owner;
                if (owner == this) continue;
                if (owner && isNegotiatedWith(owner)) continue;

                int send = std::min(3, src->armyCount - 1);
                if (send <= 0) continue;

                ordersList->addOrder(new Advance(this, send, src, tgt, deckRef, mapRef));
                cout << "  -> Advance(" << send << ", " << src->name
                     << ", " << tgt->name << ") [attack]\n";
                offIssued = true;
                break;
            }
            if (offIssued) break;
        }
    }

    // 3) CARD: same as before
    if (hand && hand->size() > 0) {
        auto* c = hand->getCards()->front();
        cout << "  -> Playing card: " << c->getType() << "\n";
        if (deckRef) {
            c->play(this, deckRef, hand);
        } else {
            hand->removeCard(c);
            cout << "     (No deck set on player; removed card from hand only)\n";
        }
    }

    cout << "[Player::issueOrder] " << *name << " finished issuing orders.\n";
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

    vector<Map::territoryNode*> defend = *ownedTerritories;
    if (!mapRef || defend.empty()) return defend;

    auto enemyNeighborCount = [&](Map::territoryNode* t) -> int {
        int cnt = 0;
        for (int ni : t->adjacentIndices) {
            const auto& neigh = mapRef->getTerritoryNodes()[ni];
            if (neigh.owner != this) ++cnt;
        }
        return cnt;
    };

    sort(defend.begin(), defend.end(),
         [&](Map::territoryNode* a, Map::territoryNode* b) {
             int ea = enemyNeighborCount(a);
             int eb = enemyNeighborCount(b);
             if (ea != eb) return ea > eb;       // more enemy pressure first
             return a->armyCount < b->armyCount; // then weakest first
         });

    return defend;
}


//toAttack() method: returns a list of territories that are to be attacked
vector<Map::territoryNode*> Player::toAttack() const {
    cout << "[Player::toAttack] " << *name << " chooses territories to attack.\n";

    vector<Map::territoryNode*> attack;
    if (!mapRef || !ownedTerritories || ownedTerritories->empty()) return attack;

    auto& nodes = mapRef->getTerritoryNodes();

    auto pushUnique = [&](Map::territoryNode* t) {
        if (!t) return;
        if (find(attack.begin(), attack.end(), t) == attack.end())
            attack.push_back(t);
    };

    for (auto* owned : *ownedTerritories) {
        for (int ni : owned->adjacentIndices) {
            Map::territoryNode* neigh = const_cast<Map::territoryNode*>(&nodes[ni]);
            if (!neigh) continue;

            Player* enemyOwner = neigh->owner;

            // must be enemy or neutral
            if (enemyOwner == this) continue;
            // cannot attack negotiated partner
            if (enemyOwner && isNegotiatedWith(enemyOwner)) continue;

            pushUnique(neigh);
        }
    }

    // optional: weakest target first
    sort(attack.begin(), attack.end(),
         [](Map::territoryNode* a, Map::territoryNode* b){
             return a->armyCount < b->armyCount;
         });

    return attack;
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

//stream insertion operator
ostream& operator<<(ostream& os, const Player& p) {
    os << "Player:" << *p.name
       << ", Number of owned territories: " << p.ownedTerritories->size()
       << ", Number of cards in hand: " << p.hand->size()
       << ", Number of orders: " << p.ordersList->size()
       << ", Reinforcement pool: " << p.reinforcementPool;  // THEIR FORMAT
    return os;
}