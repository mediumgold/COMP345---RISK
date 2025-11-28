cmake_minimum_required(VERSION 3.28)
project(COMP345___RISK)

set(CMAKE_CXX_STANDARD 17)

# === Include project directories ===
include_directories(.)
include_directories(Maps)
include_directories(Commands)

# === Common source files shared by all drivers ===
set(SOURCES
        Cards.cpp
        GameEngine.cpp
        Map.cpp
        MapLoader.cpp
        Orders.cpp
        Player.cpp
        CommandProcessing.cpp
        LoggingObserver.cpp
        PlayerStrategies.cpp
)

# === Executables for each module test ===
# Each of these has its own main() and tests that specific part.
add_executable(MapDriver         ${SOURCES} MapDriver.cpp)
add_executable(PlayerDriver      ${SOURCES} PlayerDriver.cpp)
add_executable(OrdersDriver      ${SOURCES} OrdersDriver.cpp)
add_executable(CardsDriver       ${SOURCES} CardsDriver.cpp)
add_executable(GameEngineDriver  ${SOURCES} GameEngineDriver.cpp)
add_executable(CommandProcessingDriver  ${SOURCES} CommandProcessingDriver.cpp)
add_executable(LoggingObserverDriver  ${SOURCES} LoggingObserverDriver.cpp)

add_executable(PlayerStrategiesDriver ${SOURCES} PlayerStrategiesDriver.cpp)

add_executable(TournamentDriver  ${SOURCES} TournamentDriver.cpp)

# === Full integrated demo (runs all tests in sequence) ===
# Main integrated driver (runs everything)
add_executable(MainDriver
        ${SOURCES}
        MainDriver.cpp
        MapDriver.cpp
        MapLoader.cpp
        OrdersDriver.cpp
        PlayerDriver.cpp
        CardsDriver.cpp
        GameEngineDriver.cpp
        CommandProcessingDriver.cpp
        LoggingObserverDriver.cpp
        PlayerStrategiesDriver.cpp
        TournamentDriver.cpp
)

# === Make MainDriver the default startup/run target ===
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MainDriver)

# === Copy entire Maps folder into build directory ===
file(COPY ${CMAKE_SOURCE_DIR}/Maps DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/Commands DESTINATION ${CMAKE_BINARY_DIR})



