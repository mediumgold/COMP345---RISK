// Created by Aidan Catriel on 2025-10-17.
// Command processing classes for the game engine.

#include "CommandProcessing.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
using namespace std;

namespace {

string trim(const string& value)
{
    size_t first = value.find_first_not_of(" \t\r\n");
    if (first == string::npos)
    {
        return "";
    }
    size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

}

// Default Command constructor.
Command::Command()
{
    type = LoadMap;
    parameter = "";
    effect = State::Start;
}

// Parameterized Command constructor.
Command::Command(CommandTypes type, string parameter, State effect)
{
    this->type = type;
    this->parameter = parameter;
    this->effect = effect;
}

// Copy Command constructor.
Command::Command(const Command& other)
{
    type = other.type;
    parameter = other.parameter;
    effect = other.effect;
}

// Assignment operator for Command.
Command& Command::operator=(const Command& other)
{
    if (this != &other)
    {
        type = other.type;
        parameter = other.parameter;
        effect = other.effect;
    }
    return *this;
}

ostream& operator<<(ostream& os, const Command& cmd)
{
    os << "Command Type: ";
    switch (cmd.type)
    {
    case LoadMap:
        os << "LoadMap";
        break;
    case ValidateMap:
        os << "ValidateMap";
        break;
    case AddPlayer:
        os << "AddPlayer";
        break;
    case GameStart:
        os << "GameStart";
        break;
    case Replay:
        os << "Replay";
        break;
    case Quit:
        os << "Quit";
        break;
    default:
        os << "Unknown";
        break;
    }
    os << ", Parameter: " << cmd.parameter;
    os << ", Effect: " << GameEngine::name(cmd.effect);
    return os;
}

CommandProcessor::CommandProcessor()
{
    commands = {};
    currentState = State::Start;
}

CommandProcessor::CommandProcessor(const CommandProcessor& other)
{
    currentState = other.currentState;
    // Deep copy of commands
    for (Command* cmd : other.commands)
    {
        commands.push_back(new Command(*cmd));
    }
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other)
{
    if (this != &other)
    {
        // Clean up existing commands
        for (Command* cmd : commands)
        {
            delete cmd;
        }
        commands.clear();

        currentState = other.currentState;
        // Deep copy of commands
        for (Command* cmd : other.commands)
        {
            commands.push_back(new Command(*cmd));
        }
    }
    return *this;
}

ostream& operator<<(ostream& os, const CommandProcessor& cp)
{
    os << "CommandProcessor State: " << GameEngine::name(cp.currentState) << endl;
    os << "Commands:" << endl;
    for (const Command* cmd : cp.commands)
    {
        os << *cmd << endl;
    }
    return os;
}

CommandProcessor::~CommandProcessor()
{
    // Clean up allocated Command objects.
    for (Command* cmd : commands)
    {
        delete cmd;
    }
}

// Try to read a command until successful.
Command* CommandProcessor::getCommand()
{
    bool validCommand = false;
    while (!validCommand)
    {
        string* inputs = readCommand();

        // Find the command type.
        CommandTypes commandType;
        if (inputs[0] == "loadmap") {
            commandType = LoadMap;
        }
        else if (inputs[0] == "validatemap") {
            commandType = ValidateMap;
        }
        else if (inputs[0] == "addplayer") {
            commandType = AddPlayer;
        }
        else if (inputs[0] == "gamestart") {
            commandType = GameStart;
        }
        else if (inputs[0] == "replay") {
            commandType = Replay;
        }
        else if (inputs[0] == "quit") {
            commandType = Quit;
        }
        else {
            cout << "Invalid command entered." << endl;
            continue;
        }

        bool validInState = validateCommand(commandType);
        if (!validInState) {
            cout << "Command not valid in the current state." << endl;
            continue;
        }

        // Only some commands require parameters.
        if (commandType == LoadMap || commandType == AddPlayer) {
            if (inputs[1].empty()) {
                cout << "Command requires a parameter." << endl;
                continue;
            }
        }
        else {
            if (!inputs[1].empty()) {
                cout << "Command does not take a parameter. Ignoring." << endl;
            }
        }

        // Determine the effect state of the command.
        State effect;
        switch (commandType) {
        case LoadMap:
            effect = State::MapLoaded;
            break;
        case ValidateMap:
            effect = State::MapValidated;
            break;
        case AddPlayer:
            effect = State::PlayersAdded;
            break;
        case GameStart:
            effect = State::AssignReinforcement;
            break;
        case Replay:
            effect = State::Start;
            break;
        case Quit:
            effect = State::Finished;
            break;
        default:
            effect = currentState; // No state change for unknown commands.
            break;
        }

        Command* command = new Command(commandType, inputs[1], effect);

        saveCommand(command);
        validCommand = true;
    }

    return commands.back();
}

void CommandProcessor::setState(State state)
{
    currentState = state;
}

string* CommandProcessor::readCommand()
{
    // Take a user input command from console.
    cout << "Enter command: ";
    string input;
    getline(cin, input);

    static string inputs[2];

    size_t spacePos = input.find_first_of(" \t");
    if (spacePos != string::npos) {
        inputs[0] = input.substr(0, spacePos);
        inputs[1] = trim(input.substr(spacePos + 1));
    }
    else {
        inputs[0] = input;
        inputs[1] = "";
    }

    std::transform(inputs[0].begin(), inputs[0].end(), inputs[0].begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    return inputs;
}

// Save a command to the list.
void CommandProcessor::saveCommand(Command* command)
{
    commands.push_back(command);
}

// Validate if a command is allowed in the current state.
bool CommandProcessor::validateCommand(CommandTypes commandType)
{
    bool validInState = false;
    switch (commandType) {
    case LoadMap:
        validInState = (currentState == State::Start) || (currentState == State::MapLoaded);
        break;
    case ValidateMap:
        validInState = (currentState == State::MapLoaded);
        break;
    case AddPlayer:
        validInState = (currentState == State::MapValidated) || (currentState == State::PlayersAdded);
        break;
    case GameStart:
        validInState = (currentState == State::PlayersAdded);
        break;
    case Replay:
        validInState = (currentState == State::Win);
        break;
    case Quit:
        validInState = (currentState == State::Win);
        break;
    }
    return validInState;
}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const string& filepath)
{
    filePath = filepath;
    inputFile.open(filepath);
    if (!inputFile.is_open()) {
        throw runtime_error("Could not open command file: " + filepath);
    }
}

ostream& operator<<(ostream& os, const FileCommandProcessorAdapter& fcp)
{
    os << "FileCommandProcessorAdapter file:" << fcp.filePath << endl;
    os << static_cast<const CommandProcessor&>(fcp);
    return os;
}

FileCommandProcessorAdapter::~FileCommandProcessorAdapter()
{
    if (inputFile.is_open()) {
        inputFile.close();
    }
}

// Read a command from the file.
string* FileCommandProcessorAdapter::readCommand()
{
    static string inputs[2];

    if (!inputFile.is_open() || inputFile.eof()) {
        cout << "Command file is not open or has reached EOF." << endl;
        throw runtime_error("Command file is not open or has reached EOF.");
    }

    string line;
    getline(inputFile, line);

    size_t spacePos = line.find_first_of(" \t");
    if (spacePos != string::npos) {
        inputs[0] = line.substr(0, spacePos);
        inputs[1] = trim(line.substr(spacePos + 1));
    }
    else {
        inputs[0] = line;
        inputs[1] = "";
    }

    std::transform(inputs[0].begin(), inputs[0].end(), inputs[0].begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    cout << "Read command from file: " << inputs[0] << " " << inputs[1] << endl;

    return inputs;
}
