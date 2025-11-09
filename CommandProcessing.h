// Created by Aidan Catriel on 2025-10-17.
// Command processing classes for the game engine.

#ifndef COMP345_RISK_COMMANDPROCESSING_H
#define COMP345_RISK_COMMANDPROCESSING_H

#include "GameEngine.h"
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
    Quit
};

// Stores a command object. Its type, the commands it leads to, and if it requires an additional parameter.
class Command
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
    private:
        CommandTypes type;
        string parameter;
        State effect;
};

class CommandProcessor
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
