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

using namespace std;

void testMainGameLoop();
void testTournament();
void testTournamentInteractive();

static void printUsage(const char* prog) {
    cout << "Usage:\n"
         << "  " << prog << " -console\n"
         << "  " << prog << " -file <filename>\n";
}


int main(int argc, char* argv[]) {
    cout << "==============================" << endl;
    cout << " COMP345 - Assignment 2 Demo " << endl;
    cout << "==============================" << endl << endl;

    try {
        // ---------- Parse args (-console | -file <path>) ----------
        bool useConsole = false;
        string filepath;

        if (argc < 2) {
            printUsage(argv[0]);
            cout << "\n(No args supplied --> defaulting to -console)\n";
            useConsole = true;
        } else {
            const string mode = argv[1];
            if (mode == "-console") {
                useConsole = true;
            } else if (mode == "-file") {
                if (argc < 3) {
                    cerr << "[ERR] Missing filename after -file.\n";
                    printUsage(argv[0]);
                    return 1;
                }
                filepath = argv[2];
            } else {
                cerr << "[ERR] Unknown argument: " << mode << "\n";
                printUsage(argv[0]);
                return 1;
            }
        }

        // ---------- PART 1: Command Processor & Adapter driver ----------
        cout << "\n>>> PART 1: Command Processor & Command Adapter <<<\n";
        // console input, file input, and state-based validation
        testCommandProcessor(argc, argv);

        // ---------- PART 2: Game Startup Phase via GameEngine ----------
        cout << "\n>>> PART 2: GameEngine Startup Phase (using same source) <<<\n";

        // Build the appropriate command source
        unique_ptr<CommandProcessor> processor;
        if (useConsole) {
            processor = make_unique<CommandProcessor>();
            cout << "[MainDriver] Using console as command source.\n";
        } else {
            processor = make_unique<FileCommandProcessorAdapter>(filepath);
            cout << "[MainDriver] Using file as command source: " << filepath << "\n";
        }

        // Attach logger to both subjects so all actions go to gamelog.txt
        LogObserver logger("gamelog.txt");
        GameEngine engine;
        engine.attach(&logger);
        processor->attach(&logger);

        // Run the startup phase (maps folder name is "Maps")
        // GameEngine::startupPhase takes a CommandProcessor& and a map directory.
        engine.startupPhase(*processor, "Maps"); // prints available maps, expects: loadmap/validatemap/addplayer/gamestart

        cout << "\n[MainDriver] Final engine state: " << GameEngine::name(engine.state()) << "\n";
        cout << "[MainDriver] Log written to: gamelog.txt (in your build dir)\n";

        // ---------- PART 3: Gameplay main game loop ----------
        cout << "\n>>> PART 3: Gameplay main game loop<<<\n";
        testMainGameLoop();

        // ---------- PART 4: Order Execution ----------
        cout << "\n>>> PART 4: Order Execution <<<\n";
        testOrderExecution();

        // ---------- PART 5: Order Execution ----------
        cout << "\n>>> PART 5: Game log Observer<<<\n";
        testLoggingObserver();
// Tournament
        testTournament();
        testTournamentInteractive();
    }

    catch (const std::exception& e) {
        cerr << "An error occurred: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
