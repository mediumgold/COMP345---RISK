//
// Created by Aidan Catriel on 2025-11-19.
//

#pragma once

#include "Player.h"

// Each strategy defines how a player issues orders.

class PlayerStrategy {
public:
    Player* p;
    ~PlayerStrategy() = default;
    virtual void issueOrder() = 0;
    virtual void toAttack() = 0;
    virtual void toDefend() = 0;
};

class HumanPlayerStrategy : public PlayerStrategy {
public:
    HumanPlayerStrategy(Player* player) { p = player; }
    HumanPlayerStrategy(const PlayerStrategy& other) { p = other.p; }
    HumanPlayerStrategy& operator=(const HumanPlayerStrategy& other) {
        if (this != &other) {
            p = other.p;
        }
        return *this;
    };
    friend ostream& operator<<(ostream& os, const HumanPlayerStrategy& hps) {
        os << "HumanPlayerStrategy for player: " << hps.p->getName();
        return os;
    }
    void issueOrder() override;
    void toAttack() override;
    void toDefend() override;
};

class AggressivePlayerStrategy : public PlayerStrategy {
public:
    AggressivePlayerStrategy(Player* player) { p = player; }
    AggressivePlayerStrategy(const PlayerStrategy& other) { p = other.p; }
    AggressivePlayerStrategy& operator=(const AggressivePlayerStrategy& other) {
        if (this != &other) {
            p = other.p;
        }
        return *this;
    };
    friend ostream& operator<<(ostream& os, const AggressivePlayerStrategy& aps) {
        os << "AggressivePlayerStrategy for player: " << aps.p->getName();
        return os;
    }
    void issueOrder() override;
    void toAttack() override;
    void toDefend() override;
};

class BenevolentPlayerStrategy : public PlayerStrategy {
public:
    BenevolentPlayerStrategy(Player* player) { p = player; }
    BenevolentPlayerStrategy(const PlayerStrategy& other) { p = other.p; }
    BenevolentPlayerStrategy& operator=(const BenevolentPlayerStrategy& other) {
        if (this != &other) {
            p = other.p;
        }
        return *this;
    };
    friend ostream& operator<<(ostream& os, const BenevolentPlayerStrategy& bps) {
        os << "BenevolentPlayerStrategy for player: " << bps.p->getName();
        return os;
    }
    void issueOrder() override;
    void toAttack() override;
    void toDefend() override;
};

class NeutralPlayerStrategy : public PlayerStrategy {
public:
    NeutralPlayerStrategy(Player* player) { p = player; }
    NeutralPlayerStrategy(const PlayerStrategy& other) { p = other.p; }
    NeutralPlayerStrategy& operator=(const NeutralPlayerStrategy& other) {
        if (this != &other) {
            p = other.p;
        }
        return *this;
    };
    friend ostream& operator<<(ostream& os, const NeutralPlayerStrategy& nps) {
        os << "NeutralPlayerStrategy for player: " << nps.p->getName();
        return os;
    }
    void issueOrder() override;
    void toAttack() override;
    void toDefend() override;
};

class NeutralPlayerStrategy : public PlayerStrategy {
public:
    NeutralPlayerStrategy(Player* player) { p = player; }
    NeutralPlayerStrategy(const PlayerStrategy& other) { p = other.p; }
    NeutralPlayerStrategy& operator=(const NeutralPlayerStrategy& other) {
        if (this != &other) {
            p = other.p;
        }
        return *this;
    };
    friend ostream& operator<<(ostream& os, const NeutralPlayerStrategy& nps) {
        os << "NeutralPlayerStrategy for player: " << nps.p->getName();
        return os;
    }
    void issueOrder() override;
    void toAttack() override;
    void toDefend() override;
};

class CheaterPlayerStrategy : public PlayerStrategy {
public:
    CheaterPlayerStrategy(Player* player) { p = player; }
    CheaterPlayerStrategy(const PlayerStrategy& other) { p = other.p; }
    CheaterPlayerStrategy& operator=(const CheaterPlayerStrategy& other) {
        if (this != &other) {
            p = other.p;
        }
        return *this;
    };
    friend ostream& operator<<(ostream& os, const CheaterPlayerStrategy& cps) {
        os << "CheaterPlayerStrategy for player: " << cps.p->getName();
        return os;
    }
    void issueOrder() override;
    void toAttack() override;
    void toDefend() override;
};