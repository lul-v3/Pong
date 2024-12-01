#ifndef WINDOW_H
#define WINDOW_H
#include <SDL2/SDL.h>
#include <string>

class Window
{
public:
	Window(const std::string& title, int width, int height);
	~Window();

	void clear();
	void present();

	SDL_Renderer* GetRenderer() { return renderer; }
	SDL_Window* GetWindow() { return window; }
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
};

#endif

