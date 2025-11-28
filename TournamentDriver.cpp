// TournamentDriver.cpp
//
// This driver demonstrates:
//The tournament command can be processed and validated by the CommandProcessor
//The tournament is executed by the GameEngine
//Results are output to the log file
//

#include "GameEngine.h"
#include "CommandProcessing.h"
#include "LoggingObserver.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/**
 * testTournament()
 * 
 * Demonstrates the tournament mode functionality:
 * 1-Shows tournament command parsing and validation
 * 2-Runs a sample tournament with multiple maps and strategies
 * 3-Outputs results to console and log file
 */
void testTournament() {
    cout << "\n";
    cout << "ASSIGNMENT 3 PART 2: TOURNAMENT MODE TEST\n";

    //clear the log file for clean output
    {
        ofstream clearLog("gamelog.txt", ios::trunc);
        clearLog << "Tournament Mode Test Log\n\n";
    }
    
    //create the LogObserver
    LogObserver logger("gamelog.txt");
    
    // Create game engine and attach logger
    GameEngine gameEngine;
    gameEngine.attach(&logger);
    
    // Test 1: Demonstrate command parsing validation
    cout << "--- Test 1: Tournament Command Parsing ---\n\n";

    // Test valid tournament parameters
    TournamentParameters params;

    // Example 1: Valid command
    string validCmd = "tournament -M Map1.map,Map2.map -P Aggressive,Benevolent,Neutral -G 3 -D 20";
    cout << "Testing command: " << validCmd << "\n";

    if (CommandProcessor::parseTournamentCommand(validCmd, params)) {
        cout << "SUCCESS: Command parsed successfully!\n";
        cout << params << "\n\n";
    } else {
        cout << "FAILED: Command parsing failed.\n\n";
    }

    // Example 2: Invalid - too many maps
    string invalidCmd1 = "tournament -M M1.map,M2.map,M3.map,M4.map,M5.map,M6.map -P Aggressive,Benevolent -G 3 -D 20";
    cout << "Testing invalid command (6 maps): " << invalidCmd1 << "\n";
    if (!CommandProcessor::parseTournamentCommand(invalidCmd1, params)) {
        cout << "CORRECTLY REJECTED: Too many maps.\n\n";
    }

    // Example 3: Invalid - Human strategy not allowed
    string invalidCmd2 = "tournament -M Map1.map -P Human,Aggressive -G 3 -D 20";
    cout << "Testing invalid command (Human strategy): " << invalidCmd2 << "\n";
    if (!CommandProcessor::parseTournamentCommand(invalidCmd2, params)) {
        cout << "CORRECTLY REJECTED: Human strategy not allowed in tournament.\n\n";
    }

    // Example 4: Invalid - turns out of range
    string invalidCmd3 = "tournament -M Map1.map -P Aggressive,Benevolent -G 3 -D 100";
    cout << "Testing invalid command (100 turns): " << invalidCmd3 << "\n";
    if (!CommandProcessor::parseTournamentCommand(invalidCmd3, params)) {
        cout << "CORRECTLY REJECTED: Max turns must be 10-50.\n\n";
    }

    // Test 2: Run actual tournament
    cout << "\n--- Test 2: Run Tournament ---\n\n";
    
    // Set up tournament parameters programmatically
    TournamentParameters tournamentParams;
    
    // Use existing maps from your Maps directory
    // Using the actual map files from your Maps folder
    tournamentParams.mapFiles = {"Africa.map", "Americas 1792.map"};
    
    // Use computer only strategies
    tournamentParams.playerStrategies = {"Aggressive", "Benevolent", "Cheater"};
    
    // Number of games per map
    tournamentParams.numberOfGames = 5;
    
    // Maximum turns per game (keep low for faster testing)
    tournamentParams.maxTurns = 50;
    
    cout << "Tournament Configuration:\n";
    cout << tournamentParams << "\n\n";
    
    if (tournamentParams.isValid()) {
        cout << "Parameters validated. Starting tournament\n\n";
        
        // Run the tournament
        // Note: Adjust "Maps" to your actual maps directory
        gameEngine.tournamentMode(tournamentParams, "Maps");
        
    } else {
        cout << "ERROR: Invalid tournament parameters!\n";
        cout << "Please ensure you have valid .map files in the Maps directory.\n\n";
        
        // Try with minimal test configuration
        cout << "Attempting minimal test configuration\n";
        tournamentParams.mapFiles = {"test.map"};  // Single map
        tournamentParams.playerStrategies = {"Aggressive", "Benevolent"};  // 2 players
        tournamentParams.numberOfGames = 1;
        tournamentParams.maxTurns = 10;
        
        if (tournamentParams.isValid()) {
            gameEngine.tournamentMode(tournamentParams, "Maps");
        }
    }
    
    // =
    // Test 3: Verify log file output
    // =
    cout << "\n--- Test 3: Log File Contents ---\n";
    cout << "Reading from gamelog.txt:\n";
    cout << "----------------------------\n";
    
    ifstream logFile("gamelog.txt");
    if (logFile.is_open()) {
        string line;
        while (getline(logFile, line)) {
            cout << line << "\n";
        }
        logFile.close();
    } else {
        cout << "Could not open gamelog.txt\n";
    }
    
    cout << "\n=\n";
    cout << "TOURNAMENT MODE TEST COMPLETE\n";
    cout << "=\n";
}

/**
 * testTournamentInteractive()
 * 
 * An interactive version that uses CommandProcessor to read tournament commands
 * from console or file.
 */
void testTournamentInteractive() {
    cout << "\n";
    cout << "INTERACTIVE TOURNAMENT MODE (CommandProcessor)\n";

    // Clear the log file for clean output
    {
        ofstream clearLog("gamelog.txt", ios::trunc);
        clearLog << "=== Tournament Mode Log ===\n\n";
    }

    GameEngine gameEngine;
    CommandProcessor cmdProcessor;
    
    // Create and attach logger
    LogObserver logger("gamelog.txt");
    gameEngine.attach(&logger);
    cmdProcessor.attach(&logger);
    
    cout << "Enter tournament command in format:\n";
    cout << "tournament -M <map1,map2,...> -P <strat1,strat2,...> -G <games> -D <turns>\n\n";
    cout << "Example: tournament -M europe.map,solar.map -P Aggressive,Benevolent,Cheater -G 2 -D 15\n\n";
    cout << "Valid strategies: Aggressive, Benevolent, Neutral, Cheater\n";
    cout << "Constraints: M=1-5 maps, P=2-4 strategies, G=1-5 games, D=10-50 turns\n\n";
    
    // Run startup phase which will process tournament command
    gameEngine.startupPhase(cmdProcessor, "Maps");
}

//main function

//int main() {
//    testTournament();
    //testTournamentInteractive();
//    return 0;
//}
