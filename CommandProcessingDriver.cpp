#include "CommandProcessingDriver.h"
#include "CommandProcessing.h"
#include "GameEngine.h"

// Free function.
void testCommandProcessor(int argc, char* argv[])
{
    // Read through the arguments. Either -console or -file <filename> is expected.
    std::unique_ptr<CommandProcessor> cp;

    if (argc > 1)
    {
        string mode = argv[1];
        if (mode == "-file" && argc > 2)
        {
            string filename = argv[2];
            FileCommandProcessorAdapter fileCp(filename);
            cp = std::make_unique<FileCommandProcessorAdapter>(filename);
        }
        else if (mode == "-console")
        {
            CommandProcessor consoleCp;
            cp = std::make_unique<CommandProcessor>();
        }
        else
        {
            cout << "Invalid arguments. Usage: <program> [-console | -file <filename>]" << endl;
            return;
        }
    }
    else
    {
        cout << "Invalid arguments. Usage: <program> [-console | -file <filename>]" << endl;
        return;
    }

    State currentState = State::Start;
    cp->setState(currentState);
    cout << "Initial State: " << GameEngine::name(cp->getState()) << endl;
    while (currentState != State::Finished)
    {
        Command* cmd = cp->getCommand();
        currentState = cmd->getEffect();
        if (currentState == State::AssignReinforcement)
        {
            currentState = State::Win; // Skip to Win for testing.
        }
        cp->setState(currentState);
        cout << "Saved Command: " << *cmd << endl;
        cout << "Current State updated to: " << GameEngine::name(cp->getState()) << endl;
    }

    cout << "Final State reached. Summary of commands:" << endl;
    cout << *cp << endl;
}

int main(int argc, char* argv[]) {
    // Test using command processor via console or file input
   testCommandProcessor(argc, argv);
    return 0;
}
