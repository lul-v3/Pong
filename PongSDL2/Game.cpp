#include "Game.h"
#include <chrono>
#include <iostream>
#include <algorithm>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"


Game::Game() : 
	running(true),
	window("Pong", 800, 600),
	ball({300.0f, 290.0f, 20.0f, 20.0f }),
	paddle1({30.0f, 250.0f, 20.0f, 100.0f }),
	paddle2({750.0f, 250.0f, 20.0f, 100.0f }),
	ballVelocityX(300.0f),
	ballVelocityY(300.0f),
	paddleSpeed(400.0f),
	paddle1Up(false),
	paddle1Down(false),
	paddle2Up(false),
	paddle2Down(false),
	score1(0), score2(0),
	GamePaused(false),
	ShowImGui(false),
	GameVolume(64)
{
	// init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForSDLRenderer(window.GetWindow(), window.GetRenderer());
	ImGui_ImplSDLRenderer2_Init(window.GetRenderer());

	// init SDL_TTF
	if (TTF_Init() == -1)
	{
		std::cerr << "[Game::Game] Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
		throw std::runtime_error("Failed to initialize TTF: " + std::string(TTF_GetError()));
	}

	font = TTF_OpenFont("assets/fonts/Font.ttf", 24);
	if (!font)
	{
		std::cerr << "[Game::Game] Failed to load [Font.ttf] form [assets/fonts/Font.ttf]" << std::endl;
		throw std::runtime_error("Failed to load Font: " + std::string(TTF_GetError()));
	}

	pauseFont = TTF_OpenFont("assets/fonts/Font.ttf", 60);
	if (!pauseFont)
	{
		std::cerr << "[Game::Game] Failed to load [Font.ttf] form [assets/fonts/Font.wav]" << std::endl;
		throw std::runtime_error("Failed to load Font: " + std::string(TTF_GetError()));
	}

	// init SDL_Mixer
	if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == 0)
	{
		std::cerr << "[Game::Game] Failed to initialize SDL_ttf: " << Mix_GetError() << std::endl;
		throw std::runtime_error("Failed to initialize SDL_mixer: " + std::string(Mix_GetError()));
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		std::cerr << "[Game::Game] Failed to initialize Audio: " << Mix_GetError() << std::endl;
		throw std::runtime_error("Failed to Initialize Audio: " + std::string(Mix_GetError()));
	}

	// Load Sound Effects
	bouceSound = Mix_LoadWAV("assets/sounds/bounce.wav");
	if(!bouceSound)
	{
		std::cerr << "[Game::Game] Failed to load [bounce.wav] form [assets/sounds/bounce.wav]" << std::endl;
		throw std::runtime_error("Failed to load [bounce.wav] sound: " + std::string(Mix_GetError()));
	}

	scoreSound = Mix_LoadWAV("assets/sounds/score.wav");
	if (!scoreSound)
	{
		std::cerr << "[Game::Game] Failed to load [score.wav] form [assets/sounds/score.wav]" << std::endl;
		throw std::runtime_error("Failed to load [score.wav] sound: " + std::string(Mix_GetError()));
	}

	pauseSound = Mix_LoadWAV("assets/sounds/pause.wav");
	if (!pauseSound)
	{
		std::cerr << "[Game::Game] Failed to load [pause.wav] form [assets/sounds/pause.wav]" << std::endl;
		throw std::runtime_error("Failed to load [pause.wav] sound form [assets/sounds/pause.wav]: " + std::string(Mix_GetError()));
	}

	// set the default sound volume	(50%)
	Mix_VolumeChunk(bouceSound, GameVolume);	
	Mix_VolumeChunk(scoreSound, GameVolume);	
	Mix_VolumeChunk(pauseSound, GameVolume);
	
}

Game::~Game() 
{
	// free font
	if (font) { TTF_CloseFont(font); font = nullptr; }
	if (pauseFont) { TTF_CloseFont(font); pauseFont = nullptr; }
	TTF_Quit();

	// free sound effects
	if (bouceSound) { Mix_FreeChunk(bouceSound); bouceSound = nullptr; }
	if (scoreSound) { Mix_FreeChunk(scoreSound); scoreSound = nullptr; }
	if (pauseSound) { Mix_FreeChunk(pauseSound); pauseSound = nullptr; }

	// free imgui
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void Game::run()
{
	using Clock = std::chrono::high_resolution_clock;
	auto lastTime = Clock::now();

	while (running)
	{
		auto now = Clock::now();
		float deltaTime = std::chrono::duration<float>(now - lastTime).count();
		lastTime = now;

		handleEvents();
		update(deltaTime);
		render();
	}

}

void Game::handleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type)
		{
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_w) paddle1Up = true;
			if (event.key.keysym.sym == SDLK_s) paddle1Down = true;
			if (event.key.keysym.sym == SDLK_UP) paddle2Up = true;
			if (event.key.keysym.sym == SDLK_DOWN) paddle2Down = true;

			// ImGui Shortcut
			if (event.key.keysym.sym == SDLK_F1)
			{
				if (ShowImGui)
					ShowImGui = false;
				else ShowImGui = true;
			}

			// Pause Game Shortcut
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				if (GamePaused) GamePaused = false;
				else GamePaused = true;
				Mix_PlayChannel(-1, pauseSound, 0);
				SDL_Delay(2);
			}

			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_w) paddle1Up = false;
			if (event.key.keysym.sym == SDLK_s) paddle1Down = false;
			if (event.key.keysym.sym == SDLK_UP) paddle2Up = false;
			if (event.key.keysym.sym == SDLK_DOWN) paddle2Down = false;
			break;
		}
	}
}

void Game::update(float deltaTime)
{
	if (!GamePaused)
	{
		// paddle1 movement
		if (paddle1Up) paddle1.y -= paddleSpeed * deltaTime;
		if (paddle1Down) paddle1.y += paddleSpeed * deltaTime;

		// paddle2 movement
		if (paddle2Up) paddle2.y -= paddleSpeed * deltaTime;
		if (paddle2Down) paddle2.y += paddleSpeed * deltaTime;

		// paddle limitation
		paddle1.y = std::max(0.0f, std::min(paddle1.y, 600.0f - paddle1.h));
		paddle2.y = std::max(0.0f, std::min(paddle2.y, 600.0f - paddle2.h));

		// ball movement
		float newBallX = ball.x + ballVelocityX * deltaTime;
		float newBallY = ball.y + ballVelocityY * deltaTime;

		// ball wall collision
		if (newBallY <= 0 || newBallY + ball.h >= 600)
		{
			ballVelocityY = -ballVelocityY;							// Invert Y-velocity
			//ball.y = std::clamp(newBallY, 0.0f, 600.0f - ball.h);	// Ball remains in the visible area
			ball.y = SDL_clamp(newBallY, 0.0f, 600.0f - ball.h);
			Mix_PlayChannel(-1, bouceSound, 0);						// play bounce sound
		}

		// paddle1 collision
		if (SDL_HasIntersectionF(&ball, &paddle1))
		{
			float relativeIntersectY = (paddle1.y + (paddle1.h)) - (ball.y + (ball.h / 2));
			float normalizedRelativeY = relativeIntersectY / (paddle1.h / 2);

			ballVelocityX = std::abs(ballVelocityX);		// deflect the ball to the right
			ballVelocityY = -normalizedRelativeY * 300.0f;	// speed depending on the contact point
			ball.x = paddle1.x + paddle1.w;					// Place the ball outside the paddle
			Mix_PlayChannel(-1, bouceSound, 0);				// play bounce sound
		}

		// paddle2 collision
		if (SDL_HasIntersectionF(&ball, &paddle2))
		{
			float relativeIntersectY = (paddle2.y + (paddle2.h)) - (ball.y + (ball.h / 2));
			float normalizedRelativeY = relativeIntersectY / (paddle2.h / 2);

			ballVelocityX = -std::abs(ballVelocityX);		// deflect the ball to the right
			ballVelocityY = -normalizedRelativeY * 300.0f;	// speed depending on the contact point
			ball.x = paddle2.x - paddle2.w;					// Place the ball outside the paddle
			Mix_PlayChannel(-1, bouceSound, 0);				// play bounce sound
		}

		// update ball position
		ball.x = newBallX;
		ball.y = newBallY;

		// update score
		if (ball.x < 0)
		{
			score2++;
			resetBall();
			Mix_PlayChannel(-1, scoreSound, 0);	// play bounce sound
		}
		else if (ball.x > 800)
		{
			score1++;
			resetBall();
			Mix_PlayChannel(-1, scoreSound, 0);	// play bounce sound
		}
	}
}

void Game::render()
{
	window.clear();

	SDL_SetRenderDrawColor(window.GetRenderer(), 255, 255, 255, 255);
	SDL_RenderFillRectF(window.GetRenderer(), &ball);
	SDL_RenderFillRectF(window.GetRenderer(), &paddle1);
	SDL_RenderFillRectF(window.GetRenderer(), &paddle2);

	renderScore();
	if(GamePaused) { renderPause(); }
	if (ShowImGui) { renderGUI(); }

	window.present();
}

void Game::resetBall()
{
	ball = { 390.0f, 290.0f, 20.0f, 20.0f };
	ballVelocityX = -ballVelocityX;
}

void Game::renderGUI()
{
	// Start ImGui Frame
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// IMGUI
	ImGui::Begin("Game");
	if (ImGui::BeginTabBar("MainTabBar"))
	{
		if (ImGui::BeginTabItem("Game Info"))
		{
			ImGui::Text("Score 1: %d", score1);
			ImGui::Text("Score 2: %d", score2);

			ImGui::Checkbox("Pause Game", &GamePaused);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Game Var"))
		{
			ImGui::InputInt("Score 1", &score1, 1, 2);
			if(score1 < 0) score1 = 0;
			ImGui::InputInt("Score 2", &score2, 1, 2);
			if (score2 < 0) score2 = 0;
			ImGui::NewLine();
			ImGui::InputInt("BallVelocityX", &ballVelocityX);
			ImGui::InputInt("BallVelocityY", &ballVelocityY);
			ImGui::InputFloat("Paddle Speed", &paddleSpeed);
			if (paddleSpeed < 400) paddleSpeed = 400;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Game Settings"))
		{
			// Game Volume Adjustment
			ImGui::SliderInt("Audio", &GameVolume, 0, 128);

			// Adjust volumes of sounds
			Mix_VolumeChunk(bouceSound, GameVolume);
			Mix_VolumeChunk(scoreSound, GameVolume);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	
	ImGui::End();

	// ImGui Render
	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), window.GetRenderer());
}

void Game::renderScore()
{
	// score to text
	std::string scoreText = std::to_string(score1) + " : " + std::to_string(score2);

	// render text width TTF
	SDL_Color textColor = { 255, 255, 255, 255 };	// White
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
	if (!textSurface)
	{
		std::cerr << "[Game::renderScore] Failed to render text: " << TTF_GetError() << std::endl;
		throw std::runtime_error("Failed to render text: " + std::string(TTF_GetError()));
	}

	// convert text to texture
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(window.GetRenderer(), textSurface);
	SDL_FreeSurface(textSurface);
	if (!textTexture)
	{
		std::cerr << "[Game::renderScore] Failed to create texture: " << SDL_GetError() << std::endl;
		throw std::runtime_error("Failed to create texture: " + std::string(SDL_GetError()));
	}

	// score position
	int textWidth, textHeight;
	SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
	SDL_FRect dstRect = {
		(800 - textWidth) / 2,	// x centered
		550,
		textWidth,
		textHeight
	};

	// render text
	SDL_RenderCopyF(window.GetRenderer(), textTexture, nullptr, &dstRect);

	// destroy texture
	SDL_DestroyTexture(textTexture);
}

void Game::renderPause()
{
	// score to text
	std::string pauseText = "GAME PAUSED!";

	// render text width TTF
	SDL_Color textColor = { 36, 42, 209, 255 };	// White
	SDL_Surface* textSurface = TTF_RenderText_Solid(pauseFont, pauseText.c_str(), textColor);
	if (!textSurface)
	{
		std::cerr << "[Game::renderPause] Failed to render text: " << TTF_GetError() << std::endl;
		throw std::runtime_error("Failed to render text: " + std::string(TTF_GetError()));
	}

	// convert text to texture
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(window.GetRenderer(), textSurface);
	SDL_FreeSurface(textSurface);
	if (!textTexture)
	{
		std::cerr << "[Game::renderPause] Failed to create texture: " << SDL_GetError() << std::endl;
		throw std::runtime_error("Failed to create texture: " + std::string(SDL_GetError()));
	}

	// score position
	int textWidth, textHeight;
	SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
	SDL_FRect dstRect = {
		(800 - textWidth) / 2,	// x centered
		300 - textHeight,
		textWidth,
		textHeight
	};

	// render text
	SDL_RenderCopyF(window.GetRenderer(), textTexture, nullptr, &dstRect);

	// destroy texture
	SDL_DestroyTexture(textTexture);
}