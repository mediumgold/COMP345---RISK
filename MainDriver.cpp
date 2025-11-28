//
// Created by Rahath on 2025-10-06.
// Main driver that calls all test functions for COMP345 Assignment 2.
//

#include <iostream>
#include <memory>
#include <string>

#include "MapDriver.h"
#include "MapLoader.h"
#include "Player.h"
#include "Orders.h"
#include "Cards.h"
#include "GameEngine.h"
#include "CommandProcessing.h"
#include "CommandProcessingDriver.h"
#include "LoggingObserverDriver.h"
#include "LoggingObserver.h"
#include "PlayerStrategies.h"

using namespace std;

void testMainGameLoop();
void testTournament();
void testTournamentInteractive();
void testPlayerStrategies();

static void printUsage(const char* prog) {
    cout << "Usage:\n"
         << "  " << prog << " -console\n"
         << "  " << prog << " -file <filename>\n";
}


int main(int argc, char* argv[]) {
    cout << "==============================" << endl;
    cout << " COMP345 - Assignment 3 Demo " << endl;
    cout << "==============================" << endl << endl;

        testPlayerStrategies();
        // Tournament
        testTournament();
        //testTournamentInteractive();

    return 0;
}
