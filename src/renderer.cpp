#include "renderer.h"
#include <iostream>
#include <filesystem>
#include <string>
#include "SDL.h"
#include <SDL_ttf.h>

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height)
    : screen_width(screen_width),
      screen_height(screen_height),
      grid_width(grid_width),
      grid_height(grid_height) {

}

Renderer::~Renderer() {
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

void Renderer::Bootstrap() {
  std::cout << "Bootstraping rendering ... :" << std::endl;
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return;
  }

  if (TTF_Init() != 0) {
    std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    SDL_Quit();
    return;
  }
  SDL_Init(SDL_INIT_EVERYTHING);
  _window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);

  if (_window == nullptr) {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return;
  }

  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (_renderer == nullptr) {
    SDL_DestroyWindow(_window);
    std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return;
  }
}

void Renderer::LoadAssets() {
  std::cout << "Loading assets ... :" << std::endl;
  std::filesystem::path source_directory = std::filesystem::canonical(std::filesystem::path(__FILE__)).parent_path();
  std::filesystem::path font_path = source_directory / "assets" / "fonts" / "UbuntuSansMono-Medium.ttf";
  TTF_Font* font = TTF_OpenFont(font_path.c_str(), 24);
  if (!font) {
    std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    TTF_Quit();
    SDL_Quit();
    return;
  }
  _font = font;
}

void Renderer::RenderGame(Snake const snake, SDL_Point const &food) {
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;

  // Clear screen
  SDL_SetRenderDrawColor(_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(_renderer);

  // Render food
  SDL_SetRenderDrawColor(_renderer, 0xFF, 0xCC, 0x00, 0xFF);
  block.x = food.x * block.w;
  block.y = food.y * block.h;
  SDL_RenderFillRect(_renderer, &block);

  // Render snake's body
  SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (SDL_Point const &point : snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(_renderer, &block);
  }

  // Render snake's head
  block.x = static_cast<int>(snake.head_x) * block.w;
  block.y = static_cast<int>(snake.head_y) * block.h;
  if (snake.alive) {
    SDL_SetRenderDrawColor(_renderer, 0x00, 0x7A, 0xCC, 0xFF);
  } else {
    SDL_SetRenderDrawColor(_renderer, 0xFF, 0x00, 0x00, 0xFF);
  }
  SDL_RenderFillRect(_renderer, &block);

  if (!snake.alive) {
    RenderText("Game Over", {250, 200, 0, 0});
    RenderText("Restart ( Enter )", {220, 245, 0, 0});
    RenderText("Go back to menu ( Backspace )", {140, 275, 0, 0});
  }

  // Update Screen
  SDL_RenderPresent(_renderer);
}

void Renderer::UpdateWindowTitle(int score, int fps) {
  std::string title{"Snake Score: " + std::to_string(score) + " FPS: " + std::to_string(fps)};
  SDL_SetWindowTitle(_window, title.c_str());
}

void Renderer::RenderText(std::string text, SDL_Rect pos) {
  SDL_Color color = {255, 255, 255, 255};
  SDL_Surface* surface = TTF_RenderText_Solid(_font, text.c_str(), color);
  if (!surface) {
    std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
    TTF_CloseFont(_font);
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    TTF_Quit();
    SDL_Quit();
    return;
  }

  SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
  if (!texture) {
      std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
      SDL_FreeSurface(surface);
      TTF_CloseFont(_font);
      SDL_DestroyRenderer(_renderer);
      SDL_DestroyWindow(_window);
      TTF_Quit();
      SDL_Quit();
      return;
  }

  SDL_FreeSurface(surface);
  SDL_Rect dstRect = pos;
  SDL_QueryTexture(texture, NULL, NULL, &dstRect.w, &dstRect.h);

  // Render the texture onto the renderer
  SDL_RenderCopy(_renderer, texture, NULL, &dstRect);

  // Cleanup the texture
  SDL_DestroyTexture(texture);
}
