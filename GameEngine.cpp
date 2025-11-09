//
// Created by Nathan on 2025-10-05.
//

#include "GameEngine.h"
#include "CommandProcessing.h"
#include "MapLoader.h"
#include "Player.h"
#include "Map.h"
#include "Cards.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <system_error>

namespace fs = std::filesystem;

namespace {

std::string trim(const std::string& value)
{
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
    {
        return "";
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string toLowerCopy(const std::string& value)
{
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

std::vector<std::string> collectMapFiles(const std::string& directory)
{
    std::vector<std::string> maps;
    try
    {
        if (!directory.empty() && fs::exists(directory))
        {
            for (const auto& entry : fs::directory_iterator(directory))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".map")
                {
                    maps.push_back(entry.path().filename().string());
                }
            }
        }
        std::sort(maps.begin(), maps.end());
    }
    catch (const fs::filesystem_error& e)
    {
        std::cout << "[StartupPhase] Unable to list map files in '" << directory << "': " << e.what() << "\n";
    }
    return maps;
}

std::string findMatchingMapName(const std::vector<std::string>& availableMaps, const std::string& requested)
{
    const auto requestedLower = toLowerCopy(requested);
    for (const auto& name : availableMaps)
    {
        if (toLowerCopy(name) == requestedLower)
        {
            return name;
        }
    }
    return "";
}

fs::path resolveMapPath(const std::vector<std::string>& availableMaps,
                        const std::string& requested,
                        const std::string& mapDirectory)
{
    const std::string trimmed = trim(requested);
    if (trimmed.empty())
    {
        return {};
    }

    auto validateCandidate = [](const fs::path& candidate) -> fs::path
    {
        if (candidate.empty())
        {
            return {};
        }

        std::error_code ec;
        if (!fs::exists(candidate, ec) || !fs::is_regular_file(candidate, ec))
        {
            return {};
        }

        const auto extension = toLowerCopy(candidate.extension().string());
        if (extension != ".map")
        {
            return {};
        }

        return candidate;
    };

    const fs::path direct(trimmed);
    if (auto candidate = validateCandidate(direct); !candidate.empty())
    {
        return candidate;
    }

    if (direct.extension().empty())
    {
        fs::path withExtension = direct;
        withExtension.replace_extension(".map");
        if (auto candidate = validateCandidate(withExtension); !candidate.empty())
        {
            return candidate;
        }
    }

    if (!mapDirectory.empty())
    {
        fs::path fromDirectory = fs::path(mapDirectory) / direct;
        if (auto candidate = validateCandidate(fromDirectory); !candidate.empty())
        {
            return candidate;
        }

        if (fromDirectory.extension().empty())
        {
            fs::path withExtension = fromDirectory;
            withExtension.replace_extension(".map");
            if (auto candidate = validateCandidate(withExtension); !candidate.empty())
            {
                return candidate;
            }
        }
    }

    const std::string match = findMatchingMapName(availableMaps, trimmed);
    if (!match.empty())
    {
        return fs::path(mapDirectory) / match;
    }

    return {};
}

}


//transition table / transitions[current_state][command] = next_state
GameEngine::GameEngine()
    : current(State::Start),
      mapLoaded(false),
      mapValidated(false),
      loadedMap(nullptr),
      players(),
      deck(std::make_unique<Deck>(STARTING_DECK_SIZE))
{

    // startup
    transition[State::Start]["loadmap"] = State::MapLoaded;
    transition[State::MapLoaded]["loadmap"] = State::MapLoaded;     // self-loop
    transition[State::MapLoaded]["validatemap"] = State::MapValidated;
    transition[State::MapValidated]["addplayer"] = State::PlayersAdded;
    transition[State::PlayersAdded]["addplayer"] = State::PlayersAdded; // self-loop

    // play
    transition[State::PlayersAdded]["assigncountries"] = State::AssignReinforcement;
    transition[State::AssignReinforcement]["issueorder"] = State::IssueOrders;
    transition[State::IssueOrders]["issueorder"] = State::IssueOrders; // self-loop
    transition[State::IssueOrders]["endissueorders"] = State::ExecuteOrders;
    transition[State::ExecuteOrders]["execorder"] = State::ExecuteOrders; // self-loop
    transition[State::ExecuteOrders]["endexecorders"] = State::AssignReinforcement; // back to assign
    transition[State::ExecuteOrders]["win"] = State::Win;

    // terminal / replay
    transition[State::Win]["play"] = State::Start;
    transition[State::Win]["end"] = State::Finished;
}

//stream insertion operator
std::ostream& operator<<(std::ostream& os, const GameEngine& g)
{
    os << "GameEngine state: " << GameEngine::name(g.current);
    return os;
}


//user feedback on input
const char* GameEngine::name(State s) {
    switch (s)
    {
        case State::Start: return "start";
        case State::MapLoaded: return "map loaded";
        case State::MapValidated: return "map validated";
        case State::PlayersAdded: return "players added";
        case State::AssignReinforcement: return "assign reinforcement";
        case State::IssueOrders: return "issue orders";
        case State::ExecuteOrders: return "execute orders";
        case State::Win: return "win";
        case State::Finished: return "finished";
    }
    return "?";
}

//transition validation
bool GameEngine::apply(const std::string& cmd) {

    auto itState = transition.find(current);

    if (itState == transition.end()) //if not found
    {
        return false;
    }

    auto itCmd = itState->second.find(cmd);  //equal to the value of the inner unordered map ( (key) state, (value) [ (key) string , (value) state ])

    if (itCmd == itState->second.end())  
    {
        return false;
    }

    //apply transition 
    current = itCmd->second;  
    return true;
}

void GameEngine::startupPhase(CommandProcessor& commandProcessor, const std::string& mapDirectory)
{
    std::cout << "=== Game Startup Phase ===\n";
    std::vector<std::string> availableMaps = collectMapFiles(mapDirectory);

    if (availableMaps.empty())
    {
        std::cout << "[StartupPhase] No .map files were found in '" << mapDirectory << "'.\n";
    }
    else
    {
        std::cout << "[StartupPhase] Maps available in '" << mapDirectory << "':\n";
        for (const auto& name : availableMaps)
        {
            std::cout << "  - " << name << "\n";
        }
    }

    std::cout << "[StartupPhase] Commands: loadmap <file>, validatemap, addplayer <name>, gamestart\n";

    commandProcessor.setState(state());

    auto loadMapCommand = [&](const std::string& parameter) -> bool {
        const fs::path candidatePath = resolveMapPath(availableMaps, parameter, mapDirectory);
        if (candidatePath.empty())
        {
            std::cout << "[StartupPhase] Map '" << parameter << "' not found in directory '" << mapDirectory << "'.\n";
            return false;
        }

        try
        {
            MapLoader loader(candidatePath.string());
            loadedMap = std::make_unique<Map>(loader.getMap());
            mapLoaded = true;
            mapValidated = false;
            players.clear();
            deck = std::make_unique<Deck>(STARTING_DECK_SIZE);
            availableMaps = collectMapFiles(mapDirectory);
            std::cout << "[StartupPhase] Loaded map: " << loadedMap->getName()
                      << " (" << candidatePath.filename().string() << ")\n";
            return true;
        }
        catch (const std::exception& e)
        {
            std::cout << "[StartupPhase] Failed to load map '" << parameter << "': " << e.what() << "\n";
            loadedMap.reset();
            mapLoaded = false;
            return false;
        }
    };

    auto validateMapCommand = [&]() -> bool {
        if (!mapLoaded || !loadedMap)
        {
            std::cout << "[StartupPhase] No map has been loaded yet.\n";
            return false;
        }

        if (loadedMap->validate())
        {
            mapValidated = true;
            std::cout << "[StartupPhase] Map validation succeeded.\n";
            return true;
        }

        std::cout << "[StartupPhase] Map validation failed. Load a valid map before continuing.\n";
        mapValidated = false;
        return false;
    };

    auto addPlayerCommand = [&](const std::string& parameter) -> bool {
        const std::string playerName = trim(parameter);
        if (playerName.empty())
        {
            std::cout << "[StartupPhase] Player name cannot be empty.\n";
            return false;
        }

        if (players.size() >= MAX_PLAYERS)
        {
            std::cout << "[StartupPhase] Cannot add more than " << MAX_PLAYERS << " players.\n";
            return false;
        }

        const auto duplicate = std::find_if(players.begin(), players.end(), [&](const std::unique_ptr<Player>& p) {
            return toLowerCopy(p->getName()) == toLowerCopy(playerName);
        });

        if (duplicate != players.end())
        {
            std::cout << "[StartupPhase] Player '" << playerName << "' already exists.\n";
            return false;
        }

        players.push_back(std::make_unique<Player>(playerName));
        std::cout << "[StartupPhase] Added player: " << playerName << "\n";
        return true;
    };

    auto startGameCommand = [&]() -> bool {
        if (!mapLoaded || !mapValidated || !loadedMap)
        {
            std::cout << "[StartupPhase] Cannot start the game before a valid map is loaded and validated.\n";
            return false;
        }

        if (players.size() < MIN_PLAYERS)
        {
            std::cout << "[StartupPhase] At least " << MIN_PLAYERS << " players are required to start the game.\n";
            return false;
        }

        if (players.size() > MAX_PLAYERS)
        {
            std::cout << "[StartupPhase] A maximum of " << MAX_PLAYERS << " players is supported.\n";
            return false;
        }

        auto& territories = loadedMap->getTerritoryNodes();
        if (territories.empty())
        {
            std::cout << "[StartupPhase] The loaded map has no territories to distribute.\n";
            return false;
        }

        std::random_device rd;
        std::mt19937 rng(rd());

        std::shuffle(players.begin(), players.end(), rng);

        std::vector<size_t> territoryIndices(territories.size());
        std::iota(territoryIndices.begin(), territoryIndices.end(), 0);
        std::shuffle(territoryIndices.begin(), territoryIndices.end(), rng);

        for (auto& player : players)
        {
            player->clearTerritories();
            player->setReinforcementPool(0);
        }

        for (size_t i = 0; i < territoryIndices.size(); ++i)
        {
            Map::territoryNode* territory = &territories[territoryIndices[i]];
            players[i % players.size()]->addTerritory(territory);
        }

        deck = std::make_unique<Deck>(STARTING_DECK_SIZE);

        for (auto& player : players)
        {
            player->setReinforcementPool(INITIAL_REINFORCEMENT_POOL);
            for (int i = 0; i < INITIAL_CARD_DRAW; ++i)
            {
                if (Card* card = deck->draw())
                {
                    player->addCard(card);
                }
            }
        }

        std::cout << "[StartupPhase] Player order for this game:\n";
        for (size_t i = 0; i < players.size(); ++i)
        {
            std::cout << "  " << (i + 1) << ". " << players[i]->getName()
                      << " (Reinforcements: " << players[i]->getReinforcementPool()
                      << ", Territories: " << players[i]->getOwnedTerritories()->size() << ")\n";
        }

        std::cout << "[StartupPhase] Territories distributed, reinforcements assigned, and initial cards drawn.\n";
        return true;
    };

    while (state() != State::AssignReinforcement)
    {
        Command* command = nullptr;
        try
        {
            command = commandProcessor.getCommand();
        }
        catch (const std::exception& e)
        {
            std::cout << "[StartupPhase] Command processing terminated: " << e.what() << "\n";
            break;
        }

        if (!command)
        {
            std::cout << "[StartupPhase] Received an invalid command.\n";
            continue;
        }

        bool success = false;
        switch (command->getType())
        {
        case LoadMap:
            success = loadMapCommand(trim(command->getParameter()));
            if (success)
            {
                success = apply("loadmap");
            }
            break;
        case ValidateMap:
            success = validateMapCommand();
            if (success)
            {
                success = apply("validatemap");
            }
            break;
        case AddPlayer:
            success = addPlayerCommand(command->getParameter());
            if (success)
            {
                success = apply("addplayer");
            }
            break;
        case GameStart:
            success = startGameCommand();
            if (success)
            {
                success = apply("assigncountries");
            }
            break;
        default:
            std::cout << "[StartupPhase] Command not supported during startup.\n";
            break;
        }

        if (success)
        {
            std::cout << "[StartupPhase] State advanced to '" << name(state()) << "'.\n";
        }
        else
        {
            std::cout << "[StartupPhase] Command failed. State remains '" << name(state()) << "'.\n";
        }

        commandProcessor.setState(state());

        if (state() == State::AssignReinforcement)
        {
            std::cout << "[StartupPhase] Startup complete. Entering play phase.\n";
            break;
        }
    }
}

void testGameStates() {

    GameEngine g;

    std::cout << "Current state: " << GameEngine::name(g.state()) << "\n";
    while (g.state() != State::Finished) 
    {

        std::cout << "Enter command: ";
        std::string cmd;

        if (!std::getline(std::cin, cmd))
        {
            break;
        }

        if (cmd.empty())
        {
            continue;
        }

        if (g.apply(cmd)) 
        {
            std::cout << "OK -> " << GameEngine::name(g.state()) << "\n";
        }
        else 
        {
            std::cout << "Invalid command from '" << GameEngine::name(g.state()) << "': " << cmd << "\n";
        }
    }
    std::cout << "Reached 'finished'. Game end.\n";
}
