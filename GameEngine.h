//
// Created by Nathan on 2025-10-05.
// Updated for Assignment 3 Part 2: Tournament Mode
//
#ifndef COMP345_RISK_GAMEENGINE_H
#define COMP345_RISK_GAMEENGINE_H

#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <vector>
#include "Player.h"
#include "Map.h"
#include "Cards.h"
#include "LoggingObserver.h"

class CommandProcessor;
struct TournamentParameters;  //forward declaration

enum class State {
    Start,
    MapLoaded,
    MapValidated,
    PlayersAdded,
    AssignReinforcement,
    IssueOrders,
    ExecuteOrders,
    Win,
    Finished
};

// Part 2: Structure to store tournament results
struct TournamentResult {
    std::string mapName;
    int gameNumber;
    std::string winner;  // Player name or "Draw"
};

class GameEngine : public ILoggable, public Subject {
    public:

        GameEngine();   //default constructor
        GameEngine(const GameEngine& other) = delete;    //copy constructor disabled
        GameEngine& operator=(const GameEngine& other) = delete; //assignment operator disabled
        friend std::ostream& operator<<(std::ostream& os, const GameEngine& g);  //stream insertion operator

        static const char* name(State s);

        bool apply(const std::string& cmd); // returns true if transition happened
        State state() const { return current; }

        void startupPhase(CommandProcessor& commandProcessor, const std::string& mapDirectory = "Maps");

        const std::vector<std::unique_ptr<Player>>& getPlayers() const { return players; }
        const Map* getLoadedMap() const { return loadedMap.get(); }
        bool isMapLoaded() const { return mapLoaded; }
        bool isMapValidated() const { return mapValidated; }

        std::string stringToLog() const override;

        // === Part 3: Turn-loop phases ===
        void mainGameLoop(int maxTurns = 5);
        void assignReinforcementsPhase();
        void reinforcementPhase();
        void issueOrdersPhase();
        void executeOrdersPhase();
        bool isGameOver(size_t* winnerIndex = nullptr) const;

        // === Part 2: Tournament Mode ===
        void tournamentMode(const TournamentParameters& params, const std::string& mapDirectory = "Maps");
        std::string runSingleGame(const std::string& mapFile,
                                  const std::vector<std::string>& strategies,
                                  int maxTurns,
                                  const std::string& mapDirectory);
        void outputTournamentResults(const TournamentParameters& params,
                                     const std::vector<std::vector<std::string>>& results);

        // Reset game state for new game
        void resetGameState();

private:
        static constexpr int INITIAL_REINFORCEMENT_POOL = 50;
        static constexpr int INITIAL_CARD_DRAW = 2;
        static constexpr int STARTING_DECK_SIZE = 50;
        static constexpr size_t MIN_PLAYERS = 2;
        static constexpr size_t MAX_PLAYERS = 6;

        State current;

        // transitions[current_state][command] = next_state
        std::unordered_map<State,std::unordered_map<std::string, State>> transition;

        bool mapLoaded;
        bool mapValidated;
        std::unique_ptr<Map> loadedMap;
        std::vector<std::unique_ptr<Player>> players;
        std::unique_ptr<Deck> deck;

};
void testGameStates();



#endif