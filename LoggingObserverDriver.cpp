// created by Nathan 11/10/25
//driver that shows logging for each command, order and gameengine state change

#include "LoggingObserver.h"
#include "CommandProcessing.h"
#include "Orders.h"
#include "GameEngine.h"

#include <fstream>
#include <iostream>
#include <string>

using std::cout;
using std::endl;

void testLoggingObserver()
{
    {
        std::ofstream wipe("gamelog.txt", std::ios::trunc);
        wipe << "=== LoggingObserver Driver ===\n";
    }

    //Observer
    LogObserver logger("gamelog.txt");

    //Subjects
    GameEngine ge;
    CommandProcessor cp;
    OrdersList orders;

    //Attach observer to the subjects
    ge.attach(&logger);
    cp.attach(&logger);
    orders.attach(&logger);

    // tries to convert a pointer to concrete objects into a pointer to base types (Subject/ILoggable)
    //if the pointer is non-null, the object is inheriting from base class.
    //true returns a 1, so the output of 11 | 11 | 11 means that all the subjects/concrete objects are inheriting Subject and ILoggable.
    cout << "Subject/ILoggable checks: "
         << (dynamic_cast<Subject*>(&ge)     != nullptr) << " "
         << (dynamic_cast<ILoggable*>(&ge)   != nullptr) << " | "
         << (dynamic_cast<Subject*>(&cp)     != nullptr) << " "
         << (dynamic_cast<ILoggable*>(&cp)   != nullptr) << " | "
         << (dynamic_cast<Subject*>(&orders) != nullptr) << " "
         << (dynamic_cast<ILoggable*>(&orders)!= nullptr) << endl;

    
    
    //Commands
    cp.setState(State::Start);

    Command* c1 = new Command(LoadMap,"earth.map", State::MapLoaded);
    Command* c2 = new Command(ValidateMap, "",State::MapValidated);
    Command* c3 = new Command(AddPlayer,"Alice",State::PlayersAdded);

    // attach observer to each command
    c1->attach(&logger);
    c2->attach(&logger);
    c3->attach(&logger);

    // save commands
    cp.saveCommand(c1);
    cp.saveCommand(c2);
    cp.saveCommand(c3);

  
    // Create one of each order
    Order* dep = new Deploy (5, "Alaska");
    Order* adv = new Advance(3, "Alaska", "Alberta");
    Order* bmb = new Bomb   ("Ontario");
    Order* blk = new Blockade("Quebec");
    Order* air = new Airlift(2, "Yakutsk", "Siam");
    Order* neg = new Negotiate("Bob");

    // Attach the observer to each Order 
    dep->attach(&logger);
    adv->attach(&logger);
    bmb->attach(&logger);
    blk->attach(&logger);
    air->attach(&logger);
    neg->attach(&logger);

    // Add orders 
    orders.addOrder(dep);
    orders.addOrder(adv);
    orders.addOrder(bmb);
    orders.addOrder(blk);
    orders.addOrder(air);
    orders.addOrder(neg);

    // Execute each order 
    dep->execute();
    adv->execute();
    bmb->execute();
    blk->execute();
    air->execute();
    neg->execute();

    // GameEngine transitions
    ge.apply("loadmap");
    ge.apply("validatemap");
    ge.apply("addplayer");
    ge.apply("assigncountries");
    ge.apply("issueorder");
    ge.apply("endissueorders");
    ge.apply("execorder");
    ge.apply("endexecorders");

    //Print the full contents of gamelog.txt
    std::ifstream in("gamelog.txt");
    if (!in) {
        cout << "\n[WARN] Could not open gamelog.txt for reading.\n";
        return;
    }

    cout << "\n=== BEGIN gamelog.txt ===\n";
    std::string line;
    while (std::getline(in, line)) {
        cout << line << "\n";
    }
    cout << "=== END gamelog.txt ===\n";
}
