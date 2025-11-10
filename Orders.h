#ifndef COMP345_RISK_ORDERS_H
#define COMP345_RISK_ORDERS_H

#include <iostream>
#include <string>
#include <vector>
#include "Map.h" 
using namespace std;

//part 4- Forward declarations
class Player;
class Deck;

class Order {

    protected:
        bool* executed;
        string* effect;
        //part 4- Player who issued the order
        Player* issuingPlayer;

    public:
        Order();
        //part 4- Constructor with player
        Order(Player* player);
        Order(const Order& other);
        virtual ~Order();

        Order& operator=(const Order& other);

        virtual bool validate() =0;
        virtual void execute() =0;
        virtual Order* clone() const =0;
        virtual string getDescription() const = 0;

        bool isExecuted() const;
        string getEffect() const;
        
        //part 4- Getter for issuing player
        Player* getIssuingPlayer() const;

        friend ostream& operator<<(ostream& os, const Order& order);
};

class Deploy : public Order {

    private:
        int* armyUnits;
        string* targetTerritory;
        //part 4- Direct territory reference
        Map::territoryNode* targetTerritoryNode;

    public:
        Deploy();
        Deploy(int armies, const string& territory);
        //part 4- Constructor with player and territory node
        Deploy(Player* player, int armies, Map::territoryNode* target);
        Deploy(const Deploy& other);
        ~Deploy();

        Deploy& operator=(const Deploy& other);
        bool validate() override;
        void execute() override;
        Order* clone() const override;
        string getDescription() const override;

        int getArmyUnits() const;
        string getTargetTerritory() const;
        Map::territoryNode* getTargetTerritoryNode() const;
};

class Advance : public Order {

    private:
        int* armyUnits;
        string* sourceTerritory;
        string* targetTerritory;
        //part 4- Direct territory references
        Map::territoryNode* sourceTerritoryNode;
        Map::territoryNode* targetTerritoryNode;
        Deck* gameDeck;  //part 4- For card rewards
        Map* gameMap;    //part 4- For adjacency checking (FIX)

    public:
        Advance();
        Advance(int armies,const string& source, const string& target);
        //part 4- Constructor with player and territory nodes (FIXED: added Map*)
        Advance(Player* player, int armies, Map::territoryNode* source, Map::territoryNode* target, Deck* deck, Map* map);
        Advance(const Advance& other);
        ~Advance();
        Advance& operator=(const Advance& other);

        bool validate() override;
        void execute() override;
        Order* clone() const override;
        string getDescription() const override;

        int getArmyUnits() const;
        string getSourceTerritory() const;
        string getTargetTerritory() const;
        Map::territoryNode* getSourceTerritoryNode() const;
        Map::territoryNode* getTargetTerritoryNode() const;
};

class Bomb : public Order {

    private:
        string* targetTerritory;
        //part 4- Direct territory reference
        Map::territoryNode* targetTerritoryNode;
        Map* gameMap;  //part 4- For adjacency checking

    public:
        Bomb();
        Bomb(const string& territory);
        //part 4- Constructor with player and territory node
        Bomb(Player* player, Map::territoryNode* target, Map* map);
        Bomb(const Bomb& other);
        ~Bomb();

        Bomb& operator = (const Bomb& other);

        bool validate() override;
        void execute() override;
        Order* clone() const override;
        string getDescription() const override;
        string getTargetTerritory() const;
        Map::territoryNode* getTargetTerritoryNode() const;
};

class Blockade : public Order {
    private:
        string* targetTerritory;
        //part 4- Direct territory reference and neutral player
        Map::territoryNode* targetTerritoryNode;
        Player* neutralPlayer;

    public:
        Blockade();
        Blockade(const string& territory);
        //part 4- Constructor with player, territory node, and neutral player
        Blockade(Player* player, Map::territoryNode* target, Player* neutral);
        Blockade(const Blockade& other);
        ~Blockade();
        Blockade& operator=(const Blockade& other);
        bool validate() override;
        void execute() override;
        Order* clone() const override;
        string getDescription() const override;
        string getTargetTerritory() const;
        Map::territoryNode* getTargetTerritoryNode() const;
};

class Airlift: public Order {

    private:
        int* armyUnits;
        string* sourceTerritory;
        string* targetTerritory;
        //part 4- Direct territory references
        Map::territoryNode* sourceTerritoryNode;
        Map::territoryNode* targetTerritoryNode;

    public:
        Airlift();
        Airlift(int armies, const string& source, const string& target);
        //part 4- Constructor with player and territory nodes
        Airlift(Player* player, int armies, Map::territoryNode* source, Map::territoryNode* target);
        Airlift(const Airlift& other);
        ~Airlift();
        Airlift& operator=(const Airlift& other);

        bool validate() override;
        void execute() override;
        Order* clone() const override;
        string getDescription() const override;
        int getArmyUnits() const;
        string getSourceTerritory() const;
        string getTargetTerritory() const;
        Map::territoryNode* getSourceTerritoryNode() const;
        Map::territoryNode* getTargetTerritoryNode() const;
};

class Negotiate : public Order {

    private:
        string* targetPlayer;
        //part 4- Direct player reference
        Player* targetPlayerPtr;

    public:
        Negotiate();
        Negotiate(const string& player);
        //part 4- Constructor with both players
        Negotiate(Player* issuingPlayer, Player* targetPlayer);
        Negotiate(const Negotiate& other);
        ~Negotiate();

        Negotiate& operator=(const Negotiate& other);
        bool validate() override;
        void execute() override;
        Order* clone() const override;
        string getDescription() const override;
        string getTargetPlayer() const;
        Player* getTargetPlayerPtr() const;
};

class OrdersList {

    private:
        vector<Order*>* orders;

    public:
        OrdersList();
        OrdersList(const OrdersList& other);
        ~OrdersList();
        OrdersList& operator=(const OrdersList& other);

        void addOrder(Order* order);
        bool remove(int index);
        bool move(int fromIndex, int toIndex);
        Order* getOrder(int index) const;

        int size() const;
        bool empty() const;
        void executeAll();
        friend ostream& operator<<(ostream& os, const OrdersList& orders);
};

void testOrdersLists();
//part 4- New driver function
void testOrderExecution();

#endif