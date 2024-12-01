# Pong Game (C++ with SDL2, ImGui, SDL_mixer, SDL_ttf)
 This repository contains the source code for a simple Pong game created using C++ and SDL2. 
 The game supports two-player gameplay, sound effects, and a pause feature. 
 Additionally, it integrates ImGui for debugging and settings adjustment.

## Features

- Two-player gameplay with paddle controls.
- Ball physics and bounce logic.
- Pause functionality (toggle with the ESC key).
- ImGui integration for displaying game information and adjusting settings such as ball speed, paddle speed, and volume.
- Sound effects for ball bounces, scoring, and pause state.
- Customizable game settings using ImGui.

## Requirements
To run the game, you need the following libraries:

- SDL2 - for window creation, rendering, and event handling.
- SDL_mixer - for handling sound effects.
- SDL_ttf - for rendering text (e.g., the score).
- ImGui - for creating the debug GUI and game settings interface.

## How to Play
1. Player 1 uses the `W` and `S` keys to move the left paddle.
2. Player 2 uses the `UP` and `DOWN` arrow keys to move the right paddle.
3. Pause the game by pressing the `ESC` key.
4. Toggle ImGui Debug Window with the `F1` key to adjust game settings and view current scores.

## Configuration
You can adjust game settings via the ImGui interface:

- Ball Velocity: Change the speed of the ball.
- Paddle Speed: Change the speed of the paddles.
- Game Volume: Adjust the game’s sound volume.


## License

This project is licensed under the MIT License. See the LICENSE file for more details.