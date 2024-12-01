#ifndef GAME_H
#define GAME_H

#include "Window.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

class Game
{
public:
	Game();
	~Game();

	void run();

private:
	void handleEvents();
	void update(float deltaTime);
	void render();
	void resetBall();
	void renderGUI();
	void renderScore();
	void renderPause();

	bool running;
	Window window;
	TTF_Font* font;
	TTF_Font* pauseFont;
	Mix_Chunk* bouceSound;
	Mix_Chunk* scoreSound;
	Mix_Chunk* pauseSound;

	SDL_FRect ball;
	SDL_FRect paddle1, paddle2;
	int ballVelocityX, ballVelocityY;
	float paddleSpeed;

	bool paddle1Up, paddle1Down;
	bool paddle2Up, paddle2Down;
	int score1, score2;

	bool GamePaused;
	bool ShowImGui;
	int GameVolume;
};

#endif // !GAME_H

