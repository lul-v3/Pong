#include "Window.h"
#include <stdexcept>
#include <iostream>

Window::Window(const std::string& title, int width, int height)
{
	// initialize SDL2
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "[Window::Window] Failed to initialize SDL2: " << SDL_GetError() << std::endl;
		throw std::runtime_error("Failed to Initialize SDL2: " + std::string(SDL_GetError()));
	}

	window = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width, 
		height,
		SDL_WINDOW_SHOWN
	);
	if (!window)
	{
		std::cerr << "[Window::Window] Failed to create Window " << SDL_GetError() << std::endl;
		throw std::runtime_error("Failed to create Window: " + std::string(SDL_GetError()));
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		std::cerr << "[Window::Window] Failed to create Renderer: " << SDL_GetError() << std::endl;
		throw std::runtime_error("Failed to create Renderer: " + std::string(SDL_GetError()));
	}
}

Window::~Window() 
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Window::clear()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);	// Black
	SDL_RenderClear(renderer);
}

void Window::present()
{
	SDL_RenderPresent(renderer);
}