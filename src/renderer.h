#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "snake.h"
#include "SDL.h"
#include <SDL_ttf.h>

class Renderer {
 public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
           const std::size_t grid_width, const std::size_t grid_height);
  ~Renderer();

  void Bootstrap();
  void LoadAssets();
  void RenderGame(Snake const snake, SDL_Point const &food);
  void UpdateWindowTitle(int score, int fps);
  void RenderText(std::string text, SDL_Rect pos);

  SDL_Window* GetWindow() { return _window; }
  SDL_Renderer* GetRenderer() { return _renderer; }

 private:
  SDL_Window *_window;
  SDL_Renderer *_renderer;
  TTF_Font* _font;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_width;
  const std::size_t grid_height;
};

#endif