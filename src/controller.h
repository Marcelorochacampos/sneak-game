#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>
#include "SDL.h"
#include <SDL_ttf.h>
#include "snake.h"
#include "game_state.hpp"
#include "game_menu.h"

class Controller {
 public:
  Controller();
  void HandleOnboardingInput();
  void HandleMenuInput(GameMenu &gameMenu);
  void HandleInput(bool &running, Snake &snake);
  void HandleLeaderboardInput();

  bool GetQuit() { return _quit; }
  std::shared_ptr<bool> GetReset() { return _reset; }
  std::string GetCurrentPlayerName() { return _currentPlayerName; }
  std::shared_ptr<GameState> GetCurrentState() { return _currentState; }

  void SetQuit(bool v) { _quit = v; }
  void SetReset(std::shared_ptr<bool> v) { _reset = v; }
  void SetCurrentState(std::shared_ptr<GameState> state) { _currentState = state; }

 private:
  void ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite);
  SDL_Event _e;
  bool _quit;
  std::shared_ptr<bool> _reset{std::make_shared<bool>(false)};
  std::string _currentPlayerName;
  std::shared_ptr<GameState> _currentState;

};

#endif