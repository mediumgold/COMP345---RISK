// Created by Aidan Catriel on 2025-10-17.
// Command processing classes for the game engine.
// Updated for Assignment 3 Part 2: Tournament Mode

#include "CommandProcessing.h"
#include "LoggingObserver.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include <sstream>
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

string toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return result;
}

// Split a string by delimiter
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        string trimmed = trim(token);
        if (!trimmed.empty()) {
            tokens.push_back(trimmed);
        }
    }
    return tokens;
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
    tournamentParams = other.tournamentParams;  // Part 2
}

// Assignment operator for Command.
Command& Command::operator=(const Command& other)
{
    if (this != &other)
    {
        type = other.type;
        parameter = other.parameter;
        effect = other.effect;
        tournamentParams = other.tournamentParams;  // Part 2
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
    case Tournament:  // Part 2
        os << "Tournament";
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

// Part 2: Check if a strategy name is valid (only computer strategies allowed)
bool CommandProcessor::isValidStrategy(const string& strategy) {
    string lower = toLower(strategy);
    return lower == "aggressive" ||
           lower == "benevolent" ||
           lower == "neutral" ||
           lower == "cheater";
}

// Part 2: Parse tournament command string
// Format: tournament -M <mapfiles> -P <strategies> -G <numgames> -D <maxturns>
bool CommandProcessor::parseTournamentCommand(const string& commandString, TournamentParameters& params) {
    // Reset parameters
    params = TournamentParameters();

    string input = commandString;

    // Find positions of each flag
    size_t mPos = input.find("-M ");
    size_t pPos = input.find("-P ");
    size_t gPos = input.find("-G ");
    size_t dPos = input.find("-D ");

    if (mPos == string::npos || pPos == string::npos ||
        gPos == string::npos || dPos == string::npos) {
        cout << "[Tournament] Missing required parameters. Format: tournament -M <maps> -P <strategies> -G <games> -D <turns>\n";
        return false;
    }

    // Extract each section
    // Map files: between -M and -P
    string mapsSection = trim(input.substr(mPos + 3, pPos - mPos - 3));
    // Strategies: between -P and -G
    string strategiesSection = trim(input.substr(pPos + 3, gPos - pPos - 3));
    // Number of games: between -G and -D
    string gamesSection = trim(input.substr(gPos + 3, dPos - gPos - 3));
    // Max turns: after -D
    string turnsSection = trim(input.substr(dPos + 3));

    // Parse map files (comma separated)
    params.mapFiles = split(mapsSection, ',');
    if (params.mapFiles.empty() || params.mapFiles.size() > 5) {
        cout << "[Tournament] Invalid number of maps. Must be 1-5 maps.\n";
        return false;
    }

    // Parse strategies (comma separated)
    vector<string> strategies = split(strategiesSection, ',');
    for (const string& s : strategies) {
        string stratLower = toLower(s);
        if (!isValidStrategy(stratLower)) {
            cout << "[Tournament] Invalid strategy: " << s << ". Valid strategies: Aggressive, Benevolent, Neutral, Cheater\n";
            return false;
        }
        //capitalize first letter for consistency
        string formatted = stratLower;
        if (!formatted.empty()) {
            formatted[0] = static_cast<char>(toupper(formatted[0]));
        }
        params.playerStrategies.push_back(formatted);
    }

    if (params.playerStrategies.size() < 2 || params.playerStrategies.size() > 4) {
        cout << "[Tournament] Invalid number of strategies. Must be 2-4 strategies.\n";
        return false;
    }

    //parse number of games
    try {
        params.numberOfGames = stoi(gamesSection);
        if (params.numberOfGames < 1 || params.numberOfGames > 5) {
            cout << "[Tournament] Invalid number of games. Must be 1-5.\n";
            return false;
        }
    } catch (...) {
        cout << "[Tournament] Invalid number of games format.\n";
        return false;
    }

    // Parse max turns
    try {
        params.maxTurns = stoi(turnsSection);
        if (params.maxTurns < 10 || params.maxTurns > 50) {
            cout << "[Tournament] Invalid max turns. Must be 10-50.\n";
            return false;
        }
    } catch (...) {
        cout << "[Tournament] Invalid max turns format.\n";
        return false;
    }

    return params.isValid();
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
        string lowerCmd = toLower(inputs[0]);

        if (lowerCmd == "loadmap") {
            commandType = LoadMap;
        }
        else if (lowerCmd == "validatemap") {
            commandType = ValidateMap;
        }
        else if (lowerCmd == "addplayer") {
            commandType = AddPlayer;
        }
        else if (lowerCmd == "gamestart") {
            commandType = GameStart;
        }
        else if (lowerCmd == "replay") {
            commandType = Replay;
        }
        else if (lowerCmd == "quit") {
            commandType = Quit;
        }
        else if (lowerCmd == "tournament") {  // Part 2: Tournament command
            commandType = Tournament;
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

        //handle tournament command specially
        if (commandType == Tournament) {
            //reconstruct full command string
            string fullCommand = inputs[0] + " " + inputs[1];
            TournamentParameters params;

            if (!parseTournamentCommand(fullCommand, params)) {
                cout << "Tournament command parsing failed. Please check format:\n";
                cout << "tournament -M <map1,map2,...> -P <strat1,strat2,...> -G <numgames> -D <maxturns>\n";
                continue;
            }

            Command* command = new Command(commandType, inputs[1], State::Start);
            command->setTournamentParams(params);
            saveCommand(command);
            validCommand = true;

            cout << "[Tournament] Parameters validated:\n" << params << "\n";
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
            if (!inputs[1].empty() && commandType != Tournament) {
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
        case Tournament:
            effect = State::Start;  // Tournament handles its own state
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
    notify(*this);
    if (command) command->notify(*command);
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
    case Tournament:  // Part 2: Tournament is valid in Start state
        validInState = (currentState == State::Start);
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

// Override of stringToLog method for CommandProcessor object
std::string CommandProcessor::stringToLog() const {
    if (!commands.empty() && commands.back())
    {
        const Command* c = commands.back();
        return std::string("CommandProcessor::saveCommand -> ") +
            "type=" + std::to_string(static_cast<int>(c->getType())) +
            " param=\"" + c->getParameter() + "\" effect=" + GameEngine::name(c->getEffect());
    }
    return "CommandProcessor::saveCommand -> (no command)";
}

// Override of stringToLog method for Command object
std::string Command::stringToLog() const {
    string typeStr;
    switch (type) {
        case Tournament: typeStr = "Tournament"; break;
        default: typeStr = to_string(static_cast<int>(type)); break;
    }
    return std::string("Command: type=") + typeStr +
        " param=\"" + parameter + "\" effect=" + GameEngine::name(effect);
}