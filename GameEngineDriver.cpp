//
// Created by Nathan on 2025-10-05.
//
#include "GameEngine.h"
#include "CommandProcessing.h"
#include "Player.h"
#include "Map.h"
#include "Cards.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

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

}

void testStartupPhase()
{
    std::cout << "=== GameEngine Startup Phase Driver ===\n";
    std::cout << "Provide commands via '-console' or '-file <path>'. Default is console input.\n";
    std::cout << "Startup commands: loadmap <map>, validatemap, addplayer <name>, gamestart.\n";
    std::cout << "When using console mode, type commands exactly as listed above.\n";
    std::cout << "> ";

    std::string modeLine;
    if (!std::getline(std::cin, modeLine))
    {
        std::cout << "Input stream closed.\n";
        return;
    }

    std::istringstream iss(modeLine);
    std::string mode;
    iss >> mode;

    std::unique_ptr<CommandProcessor> processor;

    if (mode == "-file")
    {
        std::string path;
        std::getline(iss, path);
        path = trim(path);
        if (path.empty())
        {
            std::cout << "No file path provided. Falling back to console mode.\n";
            processor = std::make_unique<CommandProcessor>();
        }
        else
        {
            try
            {
                processor = std::make_unique<FileCommandProcessorAdapter>(path);
                std::cout << "Reading commands from '" << path << "'.\n";
            }
            catch (const std::exception& e)
            {
                std::cout << "Failed to open command file: " << e.what() << "\n";
                return;
            }
        }
    }
    else
    {
        processor = std::make_unique<CommandProcessor>();
        if (mode != "-console")
        {
            std::cout << "Using console mode.\n";
        }
    }

    GameEngine engine;
    try
    {
        engine.startupPhase(*processor);
    }
    catch (const std::exception& e)
    {
        std::cout << "Startup phase aborted: " << e.what() << "\n";
        return;
    }

    if (engine.state() != State::AssignReinforcement)
    {
        std::cout << "Startup phase did not complete. Current state: " << GameEngine::name(engine.state()) << "\n";
        return;
    }

    std::cout << "\n=== Startup Summary ===\n";

    if (const Map* map = engine.getLoadedMap())
    {
        std::cout << "Map: " << map->getName() << "\n";
    }
    else
    {
        std::cout << "No map loaded.\n";
    }

    const auto& players = engine.getPlayers();
    for (size_t i = 0; i < players.size(); ++i)
    {
        const auto& player = players[i];
        std::cout << i + 1 << ". " << player->getName()
                  << " | Territories: " << player->getOwnedTerritories()->size()
                  << " | Reinforcement Pool: " << player->getReinforcementPool()
                  << " | Hand Size: " << player->getHand()->size() << "\n";

        std::cout << "   Territories: ";
        const auto* owned = player->getOwnedTerritories();
        if (owned && !owned->empty())
        {
            for (size_t t = 0; t < owned->size(); ++t)
            {
                std::cout << owned->at(t)->name;
                if (t + 1 < owned->size())
                {
                    std::cout << ", ";
                }
            }
        }
        else
        {
            std::cout << "(none)";
        }
        std::cout << "\n";
    }

    std::cout << "=== End of Startup Summary ===\n";
}

/**
int main() {
	testGameStates();
}
*/
