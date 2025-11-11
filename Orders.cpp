#include "Orders.h"
#include "Player.h" 
#include "Cards.h"
#include "LoggingObserver.h" //added by Nathan
#include <sstream>
#include <random>    
#include <algorithm> 


//default constructor for Order
Order::Order() {
    executed = new bool(false);
    effect = new std::string("");
    issuingPlayer = nullptr;
}

//part 4- constructor with player
Order::Order(Player* player) {
    executed = new bool(false);
    effect = new std::string("");
    issuingPlayer = player;
}

//copy constructor for Order
Order::Order(const Order& other) {
    executed = new bool(*(other.executed));
    effect = new std::string(*(other.effect));
    //part 4
    issuingPlayer = other.issuingPlayer;
}

//destructor for Order
Order::~Order() {
    delete executed;
    delete effect;
}

//assignment operator for Order
Order& Order::operator=(const Order& other) {
    if (this != &other) {
        delete executed;
        delete effect;

        executed = new bool(*(other.executed));
        effect = new std::string(*(other.effect));
       //part 4
        issuingPlayer = other.issuingPlayer;
    }
    return *this;
}

//check if order has been executed
bool Order::isExecuted() const {
    return *executed;
}

//get the effect of the order
std::string Order::getEffect() const {
    return *effect;
}

//part 4- getter for issuing player
Player* Order::getIssuingPlayer() const {
    return issuingPlayer;
}

//stream insertion operator for Order
std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << order.getDescription();
    if (order.isExecuted() && !order.getEffect().empty()) {
        os << " - Effect: " << order.getEffect();
    }
    return os;
}




// DEPLOY ORDER IMPLEMENTATION

//default constructor for Deploy
Deploy::Deploy() : Order() {
    armyUnits = new int(0);
    targetTerritory = new std::string("");
    //part 4
    targetTerritoryNode = nullptr;
}

//parameterized constructor for Deploy
Deploy::Deploy(int armies, const std::string& territory) : Order() {
    armyUnits = new int(armies);
    targetTerritory = new std::string(territory);
    //part 4
    targetTerritoryNode = nullptr;
}

// part 4 -constructor with player and territory node
Deploy::Deploy(Player* player, int armies, Map::territoryNode* target) : Order(player) {
    armyUnits = new int(armies);
    targetTerritoryNode = target;
    if (target) {
        targetTerritory = new std::string(target->name);
    } else {
        targetTerritory = new std::string("");
    }
}

//copy constructor for Deploy
Deploy::Deploy(const Deploy& other) : Order(other) {
    armyUnits = new int(*(other.armyUnits));
    targetTerritory = new std::string(*(other.targetTerritory));
    //part 4
    targetTerritoryNode = other.targetTerritoryNode;
}

//destructor for Deploy
Deploy::~Deploy() {
    delete armyUnits;
    delete targetTerritory;
}

//assignment operator for Deploy
Deploy& Deploy::operator=(const Deploy& other) {
    if (this != &other) {
        Order::operator=(other);

        delete armyUnits;
        delete targetTerritory;

        armyUnits = new int(*(other.armyUnits));
        targetTerritory = new std::string(*(other.targetTerritory));
        //part 4
        targetTerritoryNode = other.targetTerritoryNode;
    }
    return *this;
}

//validate Deploy order, basic validation
bool Deploy::validate() {
    if (*armyUnits <= 0) return false;        // must deploy positive armies
    if (targetTerritory->empty()) return false; // must specify target territory

    //part 4 - Advanced validation
    if (!targetTerritoryNode) return false;  //must have valid territory node
    if (!issuingPlayer) return false;         //must have issuing player

    //check if target territory belongs to the player issuing the order
    if (targetTerritoryNode->owner != issuingPlayer) {
        return false;
    }

    //check if player has enough armies in reinforcement pool
    if (issuingPlayer->getReinforcementPool() < *armyUnits) {
        return false;
    }

    return true;
}

//execute Deploy order
void Deploy::execute() {
    if (validate()) {
        // part 4- actual deployment logic
        //remove armies from reinforcement pool
        issuingPlayer->removeFromReinforcementPool(*armyUnits);

        //add armies to target territory
        targetTerritoryNode->armyCount += *armyUnits;

        std::ostringstream oss;
        oss << "Deployed " << *armyUnits << " army units to " << *targetTerritory
            << ".Territory now has " << targetTerritoryNode->armyCount << " armies.";
        *effect = oss.str();
        *executed = true;
    } else {
        *effect = "Deploy order is invalid and was not executed";
        *executed = true;
    }
    //Nathan:
    notify(*this);                         // Log deploy order
}

//clone Deploy order for deep copying
Order* Deploy::clone() const {
    return new Deploy(*this);
}

//get description of Deploy order
std::string Deploy::getDescription() const {
    std::ostringstream oss;
    oss << "Deploy Order: " << *armyUnits << " army units to " << *targetTerritory;
    return oss.str();
}

//get army units for Deploy order
int Deploy::getArmyUnits() const {
    return *armyUnits;
}

//get target territory for Deploy order
std::string Deploy::getTargetTerritory() const {
    return *targetTerritory;
}

//part 4
Map::territoryNode* Deploy::getTargetTerritoryNode() const {
    return targetTerritoryNode;
}




// ADVANCE ORDER IMPLEMENTATION

//default constructor for Advance
Advance::Advance() : Order() {
    armyUnits = new int(0);
    sourceTerritory = new std::string("");
    targetTerritory = new std::string("");
    //part 4
    sourceTerritoryNode = nullptr;
    targetTerritoryNode = nullptr;
    gameDeck = nullptr;
    gameMap = nullptr;
}

//parameterized constructor for Advance
Advance::Advance(int armies, const std::string& source, const std::string& target) : Order() {
    armyUnits = new int(armies);
    sourceTerritory = new std::string(source);
    targetTerritory = new std::string(target);
    // part 4
    sourceTerritoryNode = nullptr;
    targetTerritoryNode = nullptr;
    gameDeck = nullptr;
    gameMap = nullptr;  // FIXED: Initialize gameMap
}

// part 4 - Constructor with player and territory nodes (FIXED: added Map*)
Advance::Advance(Player* player, int armies, Map::territoryNode* source, Map::territoryNode* target, Deck* deck, Map* map)
    : Order(player) {
    armyUnits = new int(armies);
    sourceTerritoryNode = source;
    targetTerritoryNode = target;
    gameDeck = deck;
    gameMap = map;  // FIXED: Store map reference

    if (source) {
        sourceTerritory = new std::string(source->name);
    } else {
        sourceTerritory = new std::string("");
    }

    if (target) {
        targetTerritory = new std::string(target->name);
    } else {
        targetTerritory = new std::string("");
    }
}

//copy constructor for Advance
Advance::Advance(const Advance& other) : Order(other) {
    armyUnits = new int(*(other.armyUnits));
    sourceTerritory = new std::string(*(other.sourceTerritory));
    targetTerritory = new std::string(*(other.targetTerritory));
    // part 4
    sourceTerritoryNode = other.sourceTerritoryNode;
    targetTerritoryNode = other.targetTerritoryNode;
    gameDeck = other.gameDeck;
    gameMap = other.gameMap;
}

//destructor for Advance
Advance::~Advance() {
    delete armyUnits;
    delete sourceTerritory;
    delete targetTerritory;
}

//assignment operator for Advance
Advance& Advance::operator=(const Advance& other) {
    if (this != &other) {
        Order::operator=(other);

        delete armyUnits;
        delete sourceTerritory;
        delete targetTerritory;

        armyUnits = new int(*(other.armyUnits));
        sourceTerritory = new std::string(*(other.sourceTerritory));
        targetTerritory = new std::string(*(other.targetTerritory));
        // part 4
        sourceTerritoryNode = other.sourceTerritoryNode;
        targetTerritoryNode = other.targetTerritoryNode;
        gameDeck = other.gameDeck;
        gameMap = other.gameMap;
    }
    return *this;
}

//validate Advance order (Proper adjacency checking)
bool Advance::validate() {
    if (*armyUnits <= 0) return false;
    if (sourceTerritory->empty() || targetTerritory->empty()) return false;
    if (*sourceTerritory == *targetTerritory) return false;

    // part 4 - Advanced validation
    if (!sourceTerritoryNode || !targetTerritoryNode) return false;
    if (!issuingPlayer) return false;

    //source territory must belong to the player issuing the order
    if (sourceTerritoryNode->owner != issuingPlayer) {
        return false;
    }

    //source must have enough armies
    if (sourceTerritoryNode->armyCount < *armyUnits) {
        return false;
    }

    //proper adjacency checking
    if (!gameMap) return false;

    bool isAdjacent = false;
    auto& allTerritories = gameMap->getTerritoryNodes();

    //find the index of target territory in the map
    int targetIndex = -1;
    for (size_t i = 0; i < allTerritories.size(); ++i) {
        if (&allTerritories[i] == targetTerritoryNode) {
            targetIndex = i;
            break;
        }
    }

    if (targetIndex == -1) return false;

    //check if targetIndex is in source's adjacency list
    for (int adjIdx : sourceTerritoryNode->adjacentIndices) {
        if (adjIdx == targetIndex) {
            isAdjacent = true;
            break;
        }
    }

    if (!isAdjacent) return false;

    // part 4 - Check negotiation status
    if (targetTerritoryNode->owner && targetTerritoryNode->owner != issuingPlayer) {
        if (issuingPlayer->isNegotiatedWith(targetTerritoryNode->owner)) {
            return false;  // Cannot attack a negotiated player
        }
    }

    return true;
}

//execute Advance order
void Advance::execute() {
    if (validate()) {
        std::ostringstream oss;

        // part 4 - Actual advance logic
        //remove armies from source
        sourceTerritoryNode->armyCount -= *armyUnits;

        //check if target belongs to same player (move) or enemy (attack)
        if (targetTerritoryNode->owner == issuingPlayer) {
            targetTerritoryNode->armyCount += *armyUnits;
            oss << "Advanced " << *armyUnits << " army units from " << *sourceTerritory
                << " to " << *targetTerritory << " (friendly move).";
        } else {
            //attack: simulate battle
            int attackingArmies = *armyUnits;
            int defendingArmies = targetTerritoryNode->armyCount;

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(1, 100);

            oss << "Battle at " << *targetTerritory << ": "
                << attackingArmies << " attackers vs " << defendingArmies << " defenders. ";

            //battle simulation: each attacking army has 60% chance to kill a defender
            //each defending army has 70% chance to kill an attacker
            int attackerKills = 0;
            int defenderKills = 0;

            for (int i = 0; i < attackingArmies && defendingArmies > 0; ++i) {
                if (dis(gen) <= 60) {  // 60% chance
                    attackerKills++;
                }
            }

            for (int i = 0; i < defendingArmies && attackingArmies > 0; ++i) {
                if (dis(gen) <= 70) {  // 70% chance
                    defenderKills++;
                }
            }

            //apply casualties
            defendingArmies -= attackerKills;
            if (defendingArmies < 0) defendingArmies = 0;

            attackingArmies -= defenderKills;
            if (attackingArmies < 0) attackingArmies = 0;

            oss << "Attackers killed " << attackerKills << " defenders. "
                << "Defenders killed " << defenderKills << " attackers. ";

            if (defendingArmies == 0 && attackingArmies > 0) {
                Player* previousOwner = targetTerritoryNode->owner;

                targetTerritoryNode->owner = issuingPlayer;
                targetTerritoryNode->armyCount = attackingArmies;

                //add territory to issuing player's list
                issuingPlayer->addTerritory(targetTerritoryNode);

                //remove from previous owner if they had one
                if (previousOwner) {
                    auto* territories = const_cast<std::vector<Map::territoryNode*>*>(
                        previousOwner->getOwnedTerritories()
                    );
                    territories->erase(
                        std::remove(territories->begin(), territories->end(), targetTerritoryNode),
                        territories->end()
                    );
                }

                oss << "Territory conquered! " << issuingPlayer->getName()
                    << " now owns " << *targetTerritory << " with " << attackingArmies << " armies.";

                // part 4 - Mark that player conquered a territory this turn
                issuingPlayer->setConqueredThisTurn(true);

            } else {
                // defence holds: Defenders survive
                targetTerritoryNode->armyCount = defendingArmies;
                oss << "Defense holds! " << defendingArmies << " defenders remain.";
            }
        }

        *effect = oss.str();
        *executed = true;
    } else {
        *effect = "Advance order is invalid and was not executed";
        *executed = true;
    }
    //Nathan:
    notify(*this);                         // Log Advance order
}

//clone Advance order
Order* Advance::clone() const {
    return new Advance(*this);
}

//get description of Advance order
std::string Advance::getDescription() const {
    std::ostringstream oss;
    oss << "Advance Order: " << *armyUnits << " army units from " << *sourceTerritory
        << " to " << *targetTerritory;
    return oss.str();
}

//getters of Advance
int Advance::getArmyUnits() const { return *armyUnits; }
std::string Advance::getSourceTerritory() const { return *sourceTerritory; }
std::string Advance::getTargetTerritory() const { return *targetTerritory; }

// part 4
Map::territoryNode* Advance::getSourceTerritoryNode() const { return sourceTerritoryNode; }
Map::territoryNode* Advance::getTargetTerritoryNode() const { return targetTerritoryNode; }




// BOMB ORDER IMPLEMENTATION

//default constructor for Bomb
Bomb::Bomb() : Order() {
    targetTerritory = new std::string("");
    // part 4
    targetTerritoryNode = nullptr;
    gameMap = nullptr;
}

//parameterized constructor for Bomb
Bomb::Bomb(const std::string& territory) : Order() {
    targetTerritory = new std::string(territory);
    // part 4
    targetTerritoryNode = nullptr;
    gameMap = nullptr;
}

// part 4 -constructor with player and territory node
Bomb::Bomb(Player* player, Map::territoryNode* target, Map* map) : Order(player) {
    targetTerritoryNode = target;
    gameMap = map;
    if (target) {
        targetTerritory = new std::string(target->name);
    } else {
        targetTerritory = new std::string("");
    }
}

//copy constructor for Bomb
Bomb::Bomb(const Bomb& other) : Order(other) {
    targetTerritory = new std::string(*(other.targetTerritory));
    // part 4
    targetTerritoryNode = other.targetTerritoryNode;
    gameMap = other.gameMap;
}

//destructor for Bomb
Bomb::~Bomb() {
    delete targetTerritory;
}

//assignment operator for Bomb
Bomb& Bomb::operator=(const Bomb& other) {
    if (this != &other) {
        Order::operator=(other);
        delete targetTerritory;
        targetTerritory = new std::string(*(other.targetTerritory));
        // part 4
        targetTerritoryNode = other.targetTerritoryNode;
        gameMap = other.gameMap;
    }
    return *this;
}

//validate Bomb order (Proper adjacency checking)
bool Bomb::validate() {
    if (targetTerritory->empty()) return false;

    // part 4 - Advanced validation
    if (!targetTerritoryNode) return false;
    if (!issuingPlayer) return false;
    if (!gameMap) return false;

    //target must belong to an enemy player
    if (targetTerritoryNode->owner == issuingPlayer) {
        return false;
    }

    auto& allTerritories = gameMap->getTerritoryNodes();

    //find the index of target territory
    int targetIndex = -1;
    for (size_t i = 0; i < allTerritories.size(); ++i) {
        if (&allTerritories[i] == targetTerritoryNode) {
            targetIndex = i;
            break;
        }
    }

    if (targetIndex == -1) return false;

    //check if target is adjacent to any of player's territories
    bool isAdjacentToPlayerTerritory = false;
    auto* playerTerritories = issuingPlayer->getOwnedTerritories();

    for (auto* playerTerritory : *playerTerritories) {
        for (int adjIdx : playerTerritory->adjacentIndices) {
            if (adjIdx == targetIndex) {
                isAdjacentToPlayerTerritory = true;
                break;
            }
        }
        if (isAdjacentToPlayerTerritory) break;
    }

    if (!isAdjacentToPlayerTerritory) {
        return false;
    }

    return true;
}

//execute Bomb order
void Bomb::execute() {
    if (validate()) {
        // part 4 - Actual bombing logic
        int originalArmies = targetTerritoryNode->armyCount;
        targetTerritoryNode->armyCount = targetTerritoryNode->armyCount / 2;

        std::ostringstream oss;
        oss << "Bombed territory " << *targetTerritory
            << ", destroying half of its army units (from " << originalArmies
            << " to " << targetTerritoryNode->armyCount << ").";
        *effect = oss.str();
        *executed = true;
    } else {
        *effect = "Bomb order is invalid and was not executed";
        *executed = true;
    }
    //Nathan:
    notify(*this);                         // Log Bomb order
}

//clone Bomb order
Order* Bomb::clone() const {
    return new Bomb(*this);
}

//get description of Bomb order
std::string Bomb::getDescription() const {
    std::ostringstream oss;
    oss << "Bomb Order: target territory " << *targetTerritory;
    return oss.str();
}

//getters for Bomb
std::string Bomb::getTargetTerritory() const { return *targetTerritory; }

// part 4
Map::territoryNode* Bomb::getTargetTerritoryNode() const { return targetTerritoryNode; }




// BLOCKADE ORDER IMPLEMENTATION

Blockade::Blockade() : Order() {
    targetTerritory = new std::string("");
    // part 4
    targetTerritoryNode = nullptr;
    neutralPlayer = nullptr;
}

//parameterized constructor for Blockade
Blockade::Blockade(const std::string& territory) : Order() {
    targetTerritory = new std::string(territory);
    // part 4
    targetTerritoryNode = nullptr;
    neutralPlayer = nullptr;
}

// part 4 - Constructor with player, territory node, and neutral player
Blockade::Blockade(Player* player, Map::territoryNode* target, Player* neutral) : Order(player) {
    targetTerritoryNode = target;
    neutralPlayer = neutral;
    if (target) {
        targetTerritory = new std::string(target->name);
    } else {
        targetTerritory = new std::string("");
    }
}

//copy constructor for Blockade
Blockade::Blockade(const Blockade& other) : Order(other) {
    targetTerritory = new std::string(*(other.targetTerritory));
    // part 4
    targetTerritoryNode = other.targetTerritoryNode;
    neutralPlayer = other.neutralPlayer;
}

//destructor for Blockade
Blockade::~Blockade() {
    delete targetTerritory;
}

//assignment operator for Blockade
Blockade& Blockade::operator=(const Blockade& other) {
    if (this != &other) {
        Order::operator=(other);
        delete targetTerritory;
        targetTerritory = new std::string(*(other.targetTerritory));
        // part 4
        targetTerritoryNode = other.targetTerritoryNode;
        neutralPlayer = other.neutralPlayer;
    }
    return *this;
}

//validate Blockade order
bool Blockade::validate() {
    if (targetTerritory->empty()) return false;

    // part 4 - advanced validation
    if (!targetTerritoryNode) return false;
    if (!issuingPlayer) return false;
    if (!neutralPlayer) return false;

    // target territory must belong to the player issuing the order
    if (targetTerritoryNode->owner != issuingPlayer) {
        return false;
    }

    return true;
}

//execute Blockade order
void Blockade::execute() {
    if (validate()) {
        // part 4 - Actual blockade logic
        int originalArmies = targetTerritoryNode->armyCount;
        targetTerritoryNode->armyCount *= 2;  // Double the armies

        //remove from issuing player's territories
        auto* territories = const_cast<std::vector<Map::territoryNode*>*>(
            issuingPlayer->getOwnedTerritories()
        );
        territories->erase(
            std::remove(territories->begin(), territories->end(), targetTerritoryNode),
            territories->end()
        );

        // transfer to neutral player
        targetTerritoryNode->owner = neutralPlayer;
        neutralPlayer->addTerritory(targetTerritoryNode);

        std::ostringstream oss;
        oss << "Blockaded territory " << *targetTerritory
            << ", doubled army units (from " << originalArmies
            << " to " << targetTerritoryNode->armyCount
            << ") and transferred to Neutral player.";
        *effect = oss.str();
        *executed = true;
    } else {
        *effect = "Blockade order is invalid and was not executed";
        *executed = true;
    }
    //Nathan:
    notify(*this);                         // Log Blockade order
}

//clone Blockade order
Order* Blockade::clone() const {
    return new Blockade(*this);
}

//get description of Blockade order
std::string Blockade::getDescription() const {
    std::ostringstream oss;
    oss << "Blockade Order: target territory " << *targetTerritory;
    return oss.str();
}

//getters for Blockade
std::string Blockade::getTargetTerritory() const { return *targetTerritory; }

// part 4
Map::territoryNode* Blockade::getTargetTerritoryNode() const { return targetTerritoryNode; }




// AIRLIFT ORDER IMPLEMENTATION

//default constructor for Airlift
Airlift::Airlift() : Order() {
    armyUnits = new int(0);
    sourceTerritory = new std::string("");
    targetTerritory = new std::string("");
    // part 4
    sourceTerritoryNode = nullptr;
    targetTerritoryNode = nullptr;
}

//parameterized constructor for Airlift
Airlift::Airlift(int armies, const std::string& source, const std::string& target) : Order() {
    armyUnits = new int(armies);
    sourceTerritory = new std::string(source);
    targetTerritory = new std::string(target);
    // part 4
    sourceTerritoryNode = nullptr;
    targetTerritoryNode = nullptr;
}

// part 4 - Constructor with player and territory nodes
Airlift::Airlift(Player* player, int armies, Map::territoryNode* source, Map::territoryNode* target)
    : Order(player) {
    armyUnits = new int(armies);
    sourceTerritoryNode = source;
    targetTerritoryNode = target;

    if (source) {
        sourceTerritory = new std::string(source->name);
    } else {
        sourceTerritory = new std::string("");
    }

    if (target) {
        targetTerritory = new std::string(target->name);
    } else {
        targetTerritory = new std::string("");
    }
}

//copy constructor for Airlift
Airlift::Airlift(const Airlift& other) : Order(other) {
    armyUnits = new int(*(other.armyUnits));
    sourceTerritory = new std::string(*(other.sourceTerritory));
    targetTerritory = new std::string(*(other.targetTerritory));
    // part 4
    sourceTerritoryNode = other.sourceTerritoryNode;
    targetTerritoryNode = other.targetTerritoryNode;
}

//destructor for Airlift
Airlift::~Airlift() {
    delete armyUnits;
    delete sourceTerritory;
    delete targetTerritory;
}

//assignment operator for Airlift
Airlift& Airlift::operator=(const Airlift& other) {
    if (this != &other) {
        Order::operator=(other);

        delete armyUnits;
        delete sourceTerritory;
        delete targetTerritory;

        armyUnits = new int(*(other.armyUnits));
        sourceTerritory = new std::string(*(other.sourceTerritory));
        targetTerritory = new std::string(*(other.targetTerritory));
        // part 4
        sourceTerritoryNode = other.sourceTerritoryNode;
        targetTerritoryNode = other.targetTerritoryNode;
    }
    return *this;
}

//validate Airlift order
bool Airlift::validate() {
    if (*armyUnits <= 0) return false;
    if (sourceTerritory->empty() || targetTerritory->empty()) return false;
    if (*sourceTerritory == *targetTerritory) return false;

    // part 4 - Advanced validation
    if (!sourceTerritoryNode || !targetTerritoryNode) return false;
    if (!issuingPlayer) return false;

    //both source and target must belong to the player issuing the order
    if (sourceTerritoryNode->owner != issuingPlayer) {
        return false;
    }

    if (targetTerritoryNode->owner != issuingPlayer) {
        return false;
    }

    //source must have enough armies
    if (sourceTerritoryNode->armyCount < *armyUnits) {
        return false;
    }

    return true;
}

//execute Airlift order
void Airlift::execute() {
    if (validate()) {
        // part 4 - Actual airlift logic
        sourceTerritoryNode->armyCount -= *armyUnits;
        targetTerritoryNode->armyCount += *armyUnits;

        std::ostringstream oss;
        oss << "Airlifted " << *armyUnits << " army units from " << *sourceTerritory
            << " to " << *targetTerritory << ". Target now has "
            << targetTerritoryNode->armyCount << " armies.";
        *effect = oss.str();
        *executed = true;
    } else {
        *effect = "Airlift order is invalid and was not executed";
        *executed = true;
    }
    //Nathan:
    notify(*this);                         // Log Airlift order
}

//clone Airlift order
Order* Airlift::clone() const {
    return new Airlift(*this);
}

//get description of Airlift order
std::string Airlift::getDescription() const {
    std::ostringstream oss;
    oss << "Airlift Order: " << *armyUnits << " army units from " << *sourceTerritory
        << " to " << *targetTerritory;
    return oss.str();
}

//getters for Airlift
int Airlift::getArmyUnits() const { return *armyUnits; }
std::string Airlift::getSourceTerritory() const { return *sourceTerritory; }
std::string Airlift::getTargetTerritory() const { return *targetTerritory; }

// part 4
Map::territoryNode* Airlift::getSourceTerritoryNode() const { return sourceTerritoryNode; }
Map::territoryNode* Airlift::getTargetTerritoryNode() const { return targetTerritoryNode; }




// NEGOTIATE ORDER IMPLEMENTATION

//default constructor for Negotiate
Negotiate::Negotiate() : Order() {
    targetPlayer = new std::string("");
    // part 4
    targetPlayerPtr = nullptr;
}

//parameterized constructor for Negotiate
Negotiate::Negotiate(const std::string& player) : Order() {
    targetPlayer = new std::string(player);
    // part 4
    targetPlayerPtr = nullptr;
}

// part 4 - Constructor with both players
Negotiate::Negotiate(Player* issuingPlayer, Player* target) : Order(issuingPlayer) {
    targetPlayerPtr = target;
    if (target) {
        targetPlayer = new std::string(target->getName());
    } else {
        targetPlayer = new std::string("");
    }
}

//copy constructor for Negotiate
Negotiate::Negotiate(const Negotiate& other) : Order(other) {
    targetPlayer = new std::string(*(other.targetPlayer));
    // part 4
    targetPlayerPtr = other.targetPlayerPtr;
}

//destructor for Negotiate
Negotiate::~Negotiate() {
    delete targetPlayer;
}

// assignment operator for Negotiate
Negotiate& Negotiate::operator=(const Negotiate& other) {
    if (this != &other) {
        Order::operator=(other);
        delete targetPlayer;
        targetPlayer = new std::string(*(other.targetPlayer));
        // part 4
        targetPlayerPtr = other.targetPlayerPtr;
    }
    return *this;
}


//validate Negotiate order
bool Negotiate::validate() {
    if (targetPlayer->empty()) return false;

    // part 4 - Advanced validation
    if (!targetPlayerPtr) return false;
    if (!issuingPlayer) return false;

    //cannot negotiate with yourself
    if (targetPlayerPtr == issuingPlayer) {
        return false;
    }

    return true;
}

//execute Negotiate order
void Negotiate::execute() {
    if (validate()) {
        // part 4 - actual negotiation logic
        //add each player to the other's negotiated list
        issuingPlayer->addNegotiatedPlayer(targetPlayerPtr);
        targetPlayerPtr->addNegotiatedPlayer(issuingPlayer);

        std::ostringstream oss;
        oss << "Negotiated peace with player " << *targetPlayer
            << " until end of turn. No attacks possible between these players.";
        *effect = oss.str();
        *executed = true;
    } else {
        *effect = "Negotiate order is invalid and was not executed";
        *executed = true;
    }
    //Nathan:
    notify(*this);                         // Log Negotiate order
}

//clone Negotiate order
Order* Negotiate::clone() const {
    return new Negotiate(*this);
}

//get description of Negotiate order
std::string Negotiate::getDescription() const {
    std::ostringstream oss;
    oss << "Negotiate Order: with player " << *targetPlayer;
    return oss.str();
}

//getters for Negotiate
std::string Negotiate::getTargetPlayer() const { return *targetPlayer; }

// part 4
Player* Negotiate::getTargetPlayerPtr() const { return targetPlayerPtr; }




// ORDERSLIST IMPLEMENTATION


//default constructor for OrdersList
OrdersList::OrdersList() {
    orders = new std::vector<Order*>();
}

//copy constructor for OrdersList
OrdersList::OrdersList(const OrdersList& other) {
    orders = new std::vector<Order*>();
    for (Order* order : *(other.orders)) {
        orders-> push_back(order->clone());
    }
}

//destructor for OrdersList
OrdersList::~OrdersList() {
    for (Order* order : *orders) {
        delete order;
    }
    delete orders;
}

//assignment operator for OrdersList
OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this != &other) {
        // clean up existing orders
        for (Order* order : *orders) {
            delete order;
        }
        orders->clear();

        // deep copy from other
        for (Order* order : *(other.orders)) {
            orders->push_back(order->clone());
        }
    }
    return *this;
}

//add order to the list
void OrdersList::addOrder(Order* order) {
    if (order != nullptr) {
        orders->push_back(order);
        //Nathan:
        notify(*this);                         // Log order
        if (order) order->notify(*order);      // Log order content
    }

}

//remove order at specified index
bool OrdersList::remove(int index) {
    if (index >= 0 && index < static_cast<int>(orders->size())) {
        delete (*orders)[index];
        orders->erase(orders->begin() + index);
        return true;
    }
    return false;
}

//move order from one position to another
bool OrdersList::move(int fromIndex, int toIndex) {
    if (fromIndex >= 0 && fromIndex < static_cast<int>(orders->size()) &&
        toIndex >= 0 && toIndex < static_cast<int>(orders->size()) &&
        fromIndex != toIndex) {

        Order* order = (*orders)[fromIndex];
        orders->erase(orders->begin() + fromIndex);

        if (fromIndex < toIndex) {
            toIndex--;
        }

        orders->insert(orders->begin() + toIndex, order);
        return true;
    }
    return false;
}

//get order at specified index
Order* OrdersList::getOrder(int index) const {
    if (index >= 0 && index < static_cast<int>(orders->size())) {
        return (*orders)[index];
    }
    return nullptr;
}

//get number of orders in list
int OrdersList::size() const {
    return static_cast<int>(orders->size());
}

//check if list is empty
bool OrdersList::empty() const {
    return orders->empty();
}

//execute all orders in the list
void OrdersList::executeAll() {
    for (Order* order : *orders) {
        if (order && !order->isExecuted()) {
            order->execute();
        }
    }
}

//stream insertion operator for OrdersList
std::ostream& operator<<(std::ostream& os, const OrdersList& ordersList) {
    os << "OrdersList (" << ordersList.size() << " orders):\n";
    for (int i = 0; i < ordersList.size(); ++i) {
        os << "  " << i + 1 << ". " << *(ordersList.getOrder(i)) << "\n";
    }
    return os;
}

//Nathan: stringToLog definition for order objects
std::string Order::stringToLog() const {
    std::string status = isExecuted() ? "executed" : "pending";
    return "Order: " + getDescription() + " | effect=\"" + getEffect() + "\" | " + status;
}

//Nathan stringToLog definition for OrdersList objects
std::string OrdersList::stringToLog() const {
    return std::string("OrdersList::addOrder -> size=") + std::to_string(size());
}
