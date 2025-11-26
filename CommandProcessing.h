// Created by Aidan Catriel on 2025-10-17.
// Command processing classes for the game engine.
// Updated for Assignment 3 Part 2: Tournament Mode

#ifndef COMP345_RISK_COMMANDPROCESSING_H
#define COMP345_RISK_COMMANDPROCESSING_H

#include "GameEngine.h"
#include "LoggingObserver.h"
#include <string>
#include <vector>
#include <fstream>
using namespace std;

// Commands used to progress the game engine.
enum CommandTypes
{
    LoadMap,
    ValidateMap,
    AddPlayer,
    GameStart,
    Replay,
    Quit,
    Tournament  // Part 2: New tournament command type
};

// Part 2: Structure to hold tournament parameters
struct TournamentParameters {
    vector<string> mapFiles;           // M: 1-5 map files
    vector<string> playerStrategies;   // P: 2-4 player strategies
    int numberOfGames;                 // G: 1-5 games per map
    int maxTurns;                      // D: 10-50 max turns per game

    // Default constructor
    TournamentParameters() : numberOfGames(0), maxTurns(0) {}

    // Check if parameters are valid
    bool isValid() const {
        return mapFiles.size() >= 1 && mapFiles.size() <= 5 &&
               playerStrategies.size() >= 2 && playerStrategies.size() <= 4 &&
               numberOfGames >= 1 && numberOfGames <= 5 &&
               maxTurns >= 10 && maxTurns <= 50;
    }

    // Stream insertion operator
    friend ostream& operator<<(ostream& os, const TournamentParameters& tp) {
        os << "Tournament Parameters:\n";
        os << "  Maps (" << tp.mapFiles.size() << "): ";
        for (size_t i = 0; i < tp.mapFiles.size(); ++i) {
            os << tp.mapFiles[i];
            if (i < tp.mapFiles.size() - 1) os << ", ";
        }
        os << "\n  Strategies (" << tp.playerStrategies.size() << "): ";
        for (size_t i = 0; i < tp.playerStrategies.size(); ++i) {
            os << tp.playerStrategies[i];
            if (i < tp.playerStrategies.size() - 1) os << ", ";
        }
        os << "\n  Games per map: " << tp.numberOfGames;
        os << "\n  Max turns: " << tp.maxTurns;
        return os;
    }
};

// Stores a command object. Its type, the commands it leads to, and if it requires an additional parameter.
class Command : public ILoggable, public Subject
{
    public:
        Command();
        Command(CommandTypes type, string parameter, State effect);
        Command(const Command& other);
        Command& operator=(const Command& other);
        friend ostream& operator<<(ostream& os, const Command& cmd);
        string getParameter() const { return parameter; }
        State getEffect() const { return effect; }
        CommandTypes getType() const { return type; }
        std::string stringToLog() const override;

        // Part 2: Tournament parameters getter/setter
        void setTournamentParams(const TournamentParameters& params) { tournamentParams = params; }
        TournamentParameters getTournamentParams() const { return tournamentParams; }

    private:
        CommandTypes type;
        string parameter;
        State effect;
        TournamentParameters tournamentParams;  // Part 2: Store tournament params
};

class CommandProcessor : public ILoggable, public Subject
{
    public:
        CommandProcessor();
        CommandProcessor(const CommandProcessor& other);
        CommandProcessor& operator=(const CommandProcessor& other);
        friend ostream& operator<<(ostream& os, const CommandProcessor& cp);
        virtual ~CommandProcessor();
        Command* getCommand();
        void setState(State state);
        State getState() const { return currentState; }
        std::string stringToLog() const override;
        friend void testLoggingObserver();

        // Part 2: Parse tournament command string
        static bool parseTournamentCommand(const string& commandString, TournamentParameters& params);
        static bool isValidStrategy(const string& strategy);

    protected:
        virtual string* readCommand();
        void saveCommand(Command* command);
    private:
        vector<Command*> commands;
        State currentState;
        bool validateCommand(CommandTypes commandType);
};

class FileCommandProcessorAdapter : public CommandProcessor
{
    public:
        FileCommandProcessorAdapter(const string& filepath);
        FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other) = delete;
        FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter& other) = delete;
        friend ostream& operator<<(ostream& os, const FileCommandProcessorAdapter& fcp);
        ~FileCommandProcessorAdapter() override;
    protected:
        string* readCommand() override;
    private:
        string filePath;
        std::ifstream inputFile;
};

#endif