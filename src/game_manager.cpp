#include <iostream>
#include <filesystem>
#include <future>
#include <thread>
#include <chrono>
#include "SDL.h"
#include <SDL_ttf.h>

#include "game_manager.h"
#include "controller.h"
#include "game.h"
#include "renderer.h"
#include "game_state.hpp"
#include "game_menu.h"
#include "leaderboard_manager.h"

GameManager::GameManager(
  const std::size_t width,
  const std::size_t height,
  const std::size_t grid_width,
  const std::size_t grid_height,
  Controller &controller
) : _width(width), _height(height), _grid_width(grid_width), _grid_height(grid_height), _game(grid_width, grid_height), _gameRenderer(width, height, grid_width, grid_height), _controller(controller) {
  _controller.SetCurrentState(std::make_shared<GameState>(GameState::ONBOARDING));
  _gameRenderer.Bootstrap();
  std::future<void> ftr = std::async(std::launch::async, &Renderer::LoadAssets, &_gameRenderer);

  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;

  constexpr std::size_t kFramesPerSecond{60};
  constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};

  ftr.get();

  while (!_controller.GetQuit()) {

    // std::cout << "Reset: " << (*_controller.GetReset() ? "true" : "false") << std::endl;
    if (*_controller.GetReset()) {
      _controller.SetReset(std::make_shared<bool>(false));
      _leaderboardManager.AddPlayerToLeaderboard(_controller.GetCurrentPlayerName(), _game.GetScore());
      _game.SetScore(0);
    }
    
    SDL_SetRenderDrawColor(_gameRenderer.GetRenderer(), 7, 15, 43, 255);
    SDL_RenderClear(_gameRenderer.GetRenderer());

    frame_start = SDL_GetTicks();

    switch(*_controller.GetCurrentState()) {
      case GameState::ONBOARDING: Onboarding(); break;
      case GameState::MENU: Menu(); break;
      case GameState::LEADERBOARD: Leaderboard(); break;
      case GameState::GAME: {
        RunGame();
        frame_end = SDL_GetTicks();

        // Keep track of how long each loop through the input/update/render cycle
        // takes.
        frame_count++;
        frame_duration = frame_end - frame_start;

        // After every second, update the window title.
        if (frame_end - title_timestamp >= 1000) {
          _gameRenderer.UpdateWindowTitle(_game.GetScore(), frame_count);
          frame_count = 0;
          title_timestamp = frame_end;
        }

        if (frame_duration < kMsPerFrame) {
          SDL_Delay(kMsPerFrame - frame_duration);
        }
        break;
      }
    }
  }
  return;
}

void GameManager::LoadAssets() {
  std::filesystem::path source_directory = std::filesystem::canonical(std::filesystem::path(__FILE__)).parent_path();
  std::filesystem::path font_path = source_directory / "assets" / "fonts" / "UbuntuSansMono-Medium.ttf";
  TTF_Font* font = TTF_OpenFont(font_path.c_str(), 24);
  if (!font) {
    std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
    SDL_DestroyRenderer(_gameRenderer.GetRenderer());
    SDL_DestroyWindow(_gameRenderer.GetWindow());
    TTF_Quit();
    SDL_Quit();
    return;
  }
  _font = font;
}

void GameManager::Onboarding() {
  SDL_SetRenderDrawColor(_gameRenderer.GetRenderer(), 7, 15, 43, 255);
  SDL_RenderClear(_gameRenderer.GetRenderer());
  _gameRenderer.RenderText("                   _             ", {100, 100, 0, 0});
  _gameRenderer.RenderText("   ___ _ __   __ _| | _____  ___ ", {100, 130, 0, 0});
  _gameRenderer.RenderText("  / __| '_ \\ / _` | |/ / _ \\/ __|", {100, 160, 0, 0});
  _gameRenderer.RenderText("  \\__ \\ | | | (_| |   <  __/\\__ \\", {100, 190, 0, 0});
  _gameRenderer.RenderText("  |___/_| |_|\\__,_|_|\\_\\___||___/", {100, 220, 0, 0});
  _gameRenderer.RenderText("Input your username", {200, 270, 0, 0});
  if (!_controller.GetCurrentPlayerName().empty()) {
    _gameRenderer.RenderText(_controller.GetCurrentPlayerName(), {150, 310, 0, 0});
  }

  SDL_RenderPresent(_gameRenderer.GetRenderer());
  _controller.HandleOnboardingInput();
}

void GameManager::Menu() {
  SDL_SetRenderDrawColor(_gameRenderer.GetRenderer(), 7, 15, 43, 255);
  SDL_RenderClear(_gameRenderer.GetRenderer());

  _gameRenderer.RenderText("Menu ( Move with arrow keys )", {130, 50, 0, 0});

  for(auto option : GetGameMenu().GetOptions()) {
    if (option.id == GetGameMenu().GetSelectedOption()) {
      _gameRenderer.RenderText("-> " + option.title, option.position);
      continue;
    }
    _gameRenderer.RenderText(option.title, option.position);
  }
  
  _gameRenderer.RenderText("Back ( Backspace )", {100, 250, 0, 0});

  SDL_RenderPresent(_gameRenderer.GetRenderer());
  _controller.HandleMenuInput(_gameMenu);  
}

void GameManager::Leaderboard() {
  SDL_SetRenderDrawColor(_gameRenderer.GetRenderer(), 7, 15, 43, 255);
  SDL_RenderClear(_gameRenderer.GetRenderer());

  _gameRenderer.RenderText("Leaderboard ( Top 10 )", {100, 50, 0, 0});

  std::promise<std::vector<LeaderboardPlayer> > prms;
  std::future f = prms.get_future();
  std::thread t(&LeaderboardManager::GetLeaderboardPlayers, _leaderboardManager, std::move(prms));
  t.join();
  auto players = f.get();
  for (std::size_t i = 0; i < players.size(); ++i) {
    auto player = players[i];
    int height = (i * 30) + 100;
    _gameRenderer.RenderText(player.username + " " + std::to_string(player.score), {100, height, 0, 0});
  }

  _gameRenderer.RenderText("Back ( Backspace )", {100, 500, 0, 0});

  SDL_RenderPresent(_gameRenderer.GetRenderer());
  _controller.HandleLeaderboardInput();
}

void GameManager::RunGame() {
  constexpr std::size_t kFramesPerSecond{60};
  constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};
  _game.Run(_controller, _gameRenderer, kMsPerFrame);
}