#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "SDL.h"
#include <SDL_ttf.h>

#include "game.h"
#include "game_state.hpp"
#include "game_menu.h"
#include "leaderboard_manager.h"
#include "controller.h"

class GameManager {
public:
  GameManager(
    const std::size_t width,
    const std::size_t height,
    const std::size_t grid_width,
    const std::size_t grid_height,
    Controller &controller
  );
  void Bootstrap();
  void Run();
  void LoadAssets();
  void Onboarding();
  void Menu();
  void HandleMenuInput();
  void Leaderboard();
  void HandleLeaderboardInput();
  void RunGame();
  void HandleGameInput();
  void RenderText(std::string text, SDL_Rect pos);

  void SetGameMenu(GameMenu v) { _gameMenu = v; }
  GameMenu GetGameMenu() { return _gameMenu; }

private:
  
  const std::size_t _width;
  const std::size_t _height;
  const std::size_t _grid_width;
  const std::size_t _grid_height;

  bool _quit;

  GameMenu _gameMenu;
  Game _game;
  Renderer _gameRenderer;
  LeaderboardManager _leaderboardManager;
  Controller &_controller;

  SDL_Event _e;
  SDL_Window *_window;
  SDL_Renderer *_renderer;
  TTF_Font* _font;

  void ClearScreen();
};

#endif