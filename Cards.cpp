#include "Cards.h"
#include "Player.h"
#include <iostream>

using namespace std;

//parameterized constructor for Card
Card::Card(const string& t)
: type(new string(CardsUtil::normalizeType(t))) {}

//copy constructor for Card
Card::Card(const Card& other)
: type(new string(*other.type)) {}

//assignment operator for Card
Card& Card::operator=(const Card& other) {
    if (this != &other) {
        *type = *other.type;
    }
    return *this;
}

//destructor for Card
Card::~Card() {
    delete type;
}

//getters for Card
const string& Card::getType() const {
    return *type;
}

//play() method: enables a player to use it during game play by creating
//special orders. Once a card has been played, it is removed from the hand and put back into
//the deck.
void Card::play(Player* p, Deck* d, Hand* h) {
    if (!p || !d || !h) {
        cout << "[Card::play] Missing player/deck/hand context. No-op.\n";
        return;
    }

    Map* map = p->getMap();
    auto defendList = p->toDefend();
    auto attackList = p->toAttack();

    string orderDesc;
    if (*type == "bomb") orderDesc = "Bomb order created by card.";
    else if (*type == "reinforcement") orderDesc = "Reinforcement order created by card.";
    else if (*type == "blockade") orderDesc = "Blockade order created by card.";
    else if (*type == "airlift") orderDesc = "Airlift order created by card.";
    else if (*type == "diplomacy") orderDesc = "Negotiate/Diplomacy order created by card.";
    else orderDesc = "Generic order created by card.";

    // Choose some reasonable defaults if lists are empty
    Map::territoryNode* defendTarget = nullptr;
    Map::territoryNode* attackTarget = nullptr;
    Map::territoryNode* defendSource = nullptr;

    const auto* owned = p->getOwnedTerritories();
    if (!defendList.empty()) defendTarget = defendList.front();
    else if (owned && !owned->empty()) defendTarget = owned->front();

    if (!attackList.empty()) attackTarget = attackList.front();

    if (owned && !owned->empty()) defendSource = owned->front();

    // Create the appropriate order using the rich constructors
    if (*type == "bomb") {
        if (attackTarget && map) {
            p->issueOrder(new Bomb(p, attackTarget, map));
        } else {
            cout << "[Card::play] No valid bomb target; skipping order.\n";
        }
    }
    else if (*type == "reinforcement") {
        // simplest interpretation: add bonus reinforcements then deploy them
        int bonus = 5;  // per your original demo
        if (defendTarget) {
            p->addReinforcements(bonus);
            p->issueOrder(new Deploy(p, bonus, defendTarget));
        } else {
            cout << "[Card::play] No valid reinforcement territory; skipping order.\n";
        }
    }
    else if (*type == "blockade") {
        // you will likely want to wire in a neutral player from the GameEngine;
        // for now, just skip if we don't have one.
        Player* neutral = nullptr; // TODO: inject real neutral player
        if (defendTarget && neutral) {
            p->issueOrder(new Blockade(p, defendTarget, neutral));
        } else {
            cout << "[Card::play] No neutral player / territory for blockade; skipping.\n";
        }
    }
    else if (*type == "airlift") {
        if (defendSource && defendTarget) {
            p->issueOrder(new Airlift(p, 3, defendSource, defendTarget));
        } else {
            cout << "[Card::play] No valid airlift source/target; skipping order.\n";
        }
    }
    else if (*type == "diplomacy") {
        // Needs access to another Player*; right now Card::play only knows about `p`.
        // You can: (a) leave this as a stub, or (b) redesign to pass in a target player.
        cout << "[Card::play] Diplomacy card not fully wired to real target player yet.\n";
    }
    else {
        // fallback: generic advance if possible
        if (defendSource && attackTarget && map) {
            p->issueOrder(new Advance(p, 2, defendSource, attackTarget, p->getDeck(), map));
        } else {
            cout << "[Card::play] No valid generic advance target; skipping order.\n";
        }
    }

    // hand/deck behavior stays the same
    h->removeCard(this);
    d->returnCard(this);

    cout << "[Card::play] Played '" << *type << "': " << orderDesc
         << " Card returned to deck.\n";
}


//stream insertion operator for Card
ostream& operator<<(ostream& os, const Card& c) {
    os << "Card(" << *c.type << ")";
    return os;
}

//default constructor for Deck
Deck::Deck() : cards(new vector<Card*>) {}

//parameterized constructor for Deck
Deck::Deck(int size) : cards(new vector<Card*>) {
    const auto& types = CardsUtil::validTypes();
    for (int i = 0; i < size; ++i) {
        cards->push_back(new Card(types[static_cast<size_t>(i) % types.size()]));
    }
}

//copy constructor for Deck
Deck::Deck(const Deck& other) : cards(new vector<Card*>) {
    cards->reserve(other.cards->size());
    for (const auto* c : *other.cards) {
        cards->push_back(new Card(*c));
    }
}

//stream insertion operator for Deck
Deck& Deck::operator=(const Deck& other) {
    if (this != &other) {
        for (auto* c : *cards) delete c;
        cards->clear();
        for (const auto* c : *other.cards) {
            cards->push_back(new Card(*c));
        }
    }
    return *this;
}

//deconstructor for Deck
Deck::~Deck() {
    cout << "Deleting deck objects...\n";
    for (auto* c : *cards) delete c;
    delete cards;
}

//randomize the deck card contents
mt19937& Deck::rng() {
    static random_device rd;
    static mt19937 gen(rd());
    return gen;
}

//draw() method: allows a player to draw a card at random from the cards
//remaining in the deck and place it in their hand.
Card* Deck::draw() {
    if (cards->empty()) {
        cout << "[Deck::draw] Deck is empty.\n";
        return nullptr;
    }
    uniform_int_distribution<size_t> dist(0, cards->size() - 1);
    size_t idx = dist(rng());
    Card* c = (*cards)[idx];
    (*cards)[idx] = cards->back();
    cards->pop_back();
    cout << "[Deck::draw] Drew " << *c << ". Remaining in deck: " << cards->size() << "\n";
    return c;
}

//return method: remove a card from a player's hand and
// returns the card into the deck
void Deck::returnCard(Card* c) {
    if (!c) return;
    cards->push_back(c);
    cout << "[Deck::returnCard] Returned " << *c << ". Deck size: " << cards->size() << "\n";
}

size_t Deck::size() const { return cards->size(); }

//getters for Deck
const vector<Card*>* Deck::getCards() const { return cards; }

//stream insertion operator for Deck
ostream& operator<<(ostream& os, const Deck& d) {
    os << "Deck(size=" << d.size() << ")[";
    for (size_t i = 0; i < d.cards->size(); ++i) {
        os << d.cards->at(i)->getType();
        if (i + 1 < d.cards->size()) os << ", ";
    }
    os << "]";
    return os;
}

//default constructor for Hand
Hand::Hand() : cards(new vector<Card*>) {}

//parameterized constructor for Hand
Hand::Hand(const Hand& other) : cards(new vector<Card*>) {
    for (const auto* c : *other.cards) {
        cards->push_back(new Card(*c));
    }
}

//assignment operator for Hand
Hand& Hand::operator=(const Hand& other) {
    if (this != &other) {
        for (auto* c : *cards) delete c;
        cards->clear();
        for (const auto* c : *other.cards) {
            cards->push_back(new Card(*c));
        }
    }
    return *this;
}

//deconstructor for Hand
Hand::~Hand() {
    cout << "Destroying hand objects...\n";
    for (auto* c : *cards) delete c;
    delete cards;
}

//add card into player's hand
void Hand::addCard(Card* c) { if (c) cards->push_back(c); }

//remove card from player's hand
void Hand::removeCard(Card* c) {
    if (!c) return;
    auto it = find(cards->begin(), cards->end(), c);
    if (it != cards->end()) cards->erase(it);
}

//number of cards in player's hand
size_t Hand::size() const { return cards->size(); }

//getter for cards in player's hand
const vector<Card*>* Hand::getCards() const { return cards; }

//stream insertion operator for Hand
ostream& operator<<(ostream& os, const Hand& h) {
    os << "Hand(size=" << h.size() << ")[";
    for (size_t i = 0; i < h.cards->size(); ++i) {
        os << h.cards->at(i)->getType();
        if (i + 1 < h.cards->size()) os << ", ";
    }
    os << "]";
    return os;
}

void testCards() {
    cout << "=== Part 4: Cards deck/hand demo ===\n";


    Deck deck(15);              //set deck size
    cout << "Deck contents: " << deck << "\n\n";


    Player alice("Alice");       //Create a player



    cout << alice << "\n" << *(alice.getHand()) << "\n\n";

    //Draw 7 cards from the deck and add it to the hand
    for (int i = 0; i < 7; ++i) {
        Card* c = deck.draw();
        if (c) alice.getHand()->addCard(c);
    }

    cout << "After drawing:\n";
    cout << "  " << deck << "\n";
    cout << "  " << alice.getName() <<": "<< *(alice.getHand()) << "\n";
    cout << "  "<< alice << "\n\n";

    //Play every card in player's hand
    vector<Card*> toPlay = *alice.getHand()->getCards();
    for (auto* c : toPlay) {
        cout << "Playing " << *c << "...\n";
        c->play(&alice, &deck, alice.getHand());
    }

    cout << "\nAfter playing all drawn cards:\n";
    cout << "  " << deck << "\n";
    cout << "  " << *(alice.getHand()) << "\n";
    cout << "  " << alice << "\n";

    cout << "=== End of Part 4 demo ===\n";
}