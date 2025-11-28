//
// Created by Aidan Catriel on 2025-11-19.
// FIXED VERSION
//

#ifndef COMP345_RISK_PLAYERSTRATEGIES_H
#define COMP345_RISK_PLAYERSTRATEGIES_H

#include "Player.h"
#include <vector>

// Forward declarations
class Map;

/**
 * Abstract base class for player strategies
 * Defines the interface for issuing orders and determining attack/defend targets
 */
class PlayerStrategy {
protected:
    Player* p;  // Protected so derived classes can access it

public:
    PlayerStrategy() : p(nullptr) {}
    virtual ~PlayerStrategy() = default;

    void setPlayer(Player* player) { p = player; }
    Player* getPlayer() const { return p; }

    // Pure virtual methods that must be implemented by concrete strategies
    virtual void issueOrder() = 0;
    virtual vector<Map::territoryNode*> toAttack() = 0;
    virtual vector<Map::territoryNode*> toDefend() = 0;

    // Virtual method for strategy name (useful for debugging)
    virtual string getStrategyName() const = 0;
};

/**
 * Human Player Strategy
 * Requires user interaction to make decisions
 */
class HumanPlayerStrategy : public PlayerStrategy {
public:
    HumanPlayerStrategy() : PlayerStrategy() {}
    explicit HumanPlayerStrategy(Player* player) : PlayerStrategy() { p = player; }

    // Copy constructor
    HumanPlayerStrategy(const HumanPlayerStrategy& other) : PlayerStrategy() {
        p = other.p;
    }

    // Assignment operator
    HumanPlayerStrategy& operator=(const HumanPlayerStrategy& other) {
        if (this != &other) {
            p = other.p;
        }
        return *this;
    }

    // Stream insertion operator
    friend ostream& operator<<(ostream& os, const HumanPlayerStrategy& hps) {
        os << "HumanPlayerStrategy";
        if (hps.p) os << " for player: " << hps.p->getName();
        return os;
    }

    void issueOrder() override;
    vector<Map::territoryNode*> toAttack() override;
    vector<Map::territoryNode*> toDefend() override;
    string getStrategyName() const override { return "Human"; }
};

/**
 * Aggressive Player Strategy
 * Focuses on attack: deploys to strongest territory, always advances to enemy territories
 */
class AggressivePlayerStrategy : public PlayerStrategy {
public:
    AggressivePlayerStrategy() : PlayerStrategy() {}
    explicit AggressivePlayerStrategy(Player* player) : PlayerStrategy() { p = player; }

    AggressivePlayerStrategy(const AggressivePlayerStrategy& other) : PlayerStrategy() {
        p = other.p;
    }

    AggressivePlayerStrategy& operator=(const AggressivePlayerStrategy& other) {
        if (this != &other) {
            p = other.p;
        }
        return *this;
    }

    friend ostream& operator<<(ostream& os, const AggressivePlayerStrategy& aps) {
        os << "AggressivePlayerStrategy";
        if (aps.p) os << " for player: " << aps.p->getName();
        return os;
    }

    void issueOrder() override;
    vector<Map::territoryNode*> toAttack() override;
    vector<Map::territoryNode*> toDefend() override;
    string getStrategyName() const override { return "Aggressive"; }
};

/**
 * Benevolent Player Strategy
 * Focuses on protecting weak countries: deploys to weakest, never attacks
 */
class BenevolentPlayerStrategy : public PlayerStrategy {
public:
    BenevolentPlayerStrategy() : PlayerStrategy() {}
    explicit BenevolentPlayerStrategy(Player* player) : PlayerStrategy() { p = player; }

    BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other) : PlayerStrategy() {
        p = other.p;
    }

    BenevolentPlayerStrategy& operator=(const BenevolentPlayerStrategy& other) {
        if (this != &other) {
            p = other.p;
        }
        return *this;
    }

    friend ostream& operator<<(ostream& os, const BenevolentPlayerStrategy& bps) {
        os << "BenevolentPlayerStrategy";
        if (bps.p) os << " for player: " << bps.p->getName();
        return os;
    }

    void issueOrder() override;
    vector<Map::territoryNode*> toAttack() override;
    vector<Map::territoryNode*> toDefend() override;
    string getStrategyName() const override { return "Benevolent"; }
};

/**
 * Neutral Player Strategy
 * Never issues orders. Becomes aggressive when attacked.
 */
class NeutralPlayerStrategy : public PlayerStrategy {
public:
    NeutralPlayerStrategy() : PlayerStrategy() {}
    explicit NeutralPlayerStrategy(Player* player) : PlayerStrategy() { p = player; }

    NeutralPlayerStrategy(const NeutralPlayerStrategy& other) : PlayerStrategy() {
        p = other.p;
    }

    NeutralPlayerStrategy& operator=(const NeutralPlayerStrategy& other) {
        if (this != &other) {
            p = other.p;
        }
        return *this;
    }

    friend ostream& operator<<(ostream& os, const NeutralPlayerStrategy& nps) {
        os << "NeutralPlayerStrategy";
        if (nps.p) os << " for player: " << nps.p->getName();
        return os;
    }

    void issueOrder() override;
    vector<Map::territoryNode*> toAttack() override;
    vector<Map::territoryNode*> toDefend() override;
    string getStrategyName() const override { return "Neutral"; }
};

/**
 * Cheater Player Strategy
 * Automatically conquers all adjacent enemy territories once per turn
 */
class CheaterPlayerStrategy : public PlayerStrategy {
public:
    CheaterPlayerStrategy() : PlayerStrategy() {}
    explicit CheaterPlayerStrategy(Player* player) : PlayerStrategy() { p = player; }

    CheaterPlayerStrategy(const CheaterPlayerStrategy& other) : PlayerStrategy() {
        p = other.p;
    }

    CheaterPlayerStrategy& operator=(const CheaterPlayerStrategy& other) {
        if (this != &other) {
            p = other.p;
        }
        return *this;
    }

    friend ostream& operator<<(ostream& os, const CheaterPlayerStrategy& cps) {
        os << "CheaterPlayerStrategy";
        if (cps.p) os << " for player: " << cps.p->getName();
        return os;
    }

    void issueOrder() override;
    vector<Map::territoryNode*> toAttack() override;
    vector<Map::territoryNode*> toDefend() override;
    string getStrategyName() const override { return "Cheater"; }
};

// Driver function
void testPlayerStrategies();

#endif // COMP345_RISK_PLAYERSTRATEGIES_H