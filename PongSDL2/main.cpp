// Define SDL_MAIN_HANDLED to prevent SDL from defining main() automatically.
#define SDL_MAIN_HANDLED
#include "Game.h"
#include <SDL2/SDL.h>
#include <Windows.h>

// Entry point for Windows applications using the Windows API (APIENTRY) 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    try
    {
        // Create an instance of the Game class and run the game loop
        Game game;
        game.run();
    }
    catch (const std::exception& e)
    {
        // If an exception occurs, log the error message using SDL
        SDL_Log("Error %s", e.what());
        // system("pause"); // Optional: Used for pausing the console on error (not needed for release builds)
        return -1; // Return non-zero to indicate failure
    }

    return 0;
}

// Standard entry point for console applications (main function)
int main(int argc, char* argv[])
{
    try
    {
        // Create an instance of the Game class and run the game loop
        Game game;
        game.run();
    }
    catch (const std::exception& e)
    {
        // Handle errors and log the message to the console
        // SDL_Log("Error %s", e.what()); // Uncomment this for SDL logging
        system("pause"); // Pause the console to view the error message (useful for debugging)
        return -1; // Return non-zero to indicate failure
    }

    return 0; // Return 0 to indicate success
}
