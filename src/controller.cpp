#include <iostream>
#include <string>
#include "SDL.h"
#include <SDL_ttf.h>
#include "snake.h"
#include "controller.h"
#include "game_menu.h"
#include "game_state.hpp"

#define MAX_USERNAME_CHARACTERS 10

Controller::Controller() {
  SDL_Event _e;
  _currentPlayerName = "";
}

void Controller::ChangeDirection(Snake &snake, Snake::Direction input,
                                 Snake::Direction opposite) {
  if (snake.direction != opposite || snake.size == 1) snake.direction = input;
  return;
}

void Controller::HandleOnboardingInput() {
  while (SDL_PollEvent(&_e) != 0) {
    // ESC to quit the game
    if (_e.type == SDL_QUIT || _e.key.keysym.sym == SDLK_ESCAPE) {
      _quit = true;
      break;
    }

    if (_e.type == SDL_KEYDOWN) {

      if (_e.key.keysym.sym == SDLK_RETURN || _e.key.keysym.sym == SDLK_RETURN2) {
        if (!_currentPlayerName.empty()) {
          SetCurrentState(std::make_shared<GameState>(GameState::MENU));
        }
      }

      if (_e.key.keysym.sym == SDLK_BACKSPACE && !_currentPlayerName.empty()) {
        _currentPlayerName.pop_back();
      } else if(_e.key.keysym.sym >= SDLK_a && _e.key.keysym.sym <= SDLK_z) {
        if (_currentPlayerName.length() < MAX_USERNAME_CHARACTERS) {
          _currentPlayerName += static_cast<char>(_e.key.keysym.sym);
        }
      }
    }
  }
}

void Controller::HandleMenuInput(GameMenu &gameMenu) {
  while (SDL_PollEvent(&_e) != 0) {
    // ESC to quit the game
    if (_e.type == SDL_QUIT || _e.key.keysym.sym == SDLK_ESCAPE) {
      _quit = true;
      break;
    }

    if (_e.type == SDL_KEYDOWN) {
      if (_e.key.keysym.sym == SDLK_BACKSPACE) {
        SetCurrentState(std::make_shared<GameState>(GameState::ONBOARDING));
        break;
      }

      if (_e.key.keysym.sym == SDLK_RETURN || _e.key.keysym.sym == SDLK_RETURN2) {
        if (gameMenu.GetSelectedOption() == 1) {
          SetCurrentState(std::make_shared<GameState>(GameState::GAME));
          break;
        } else if (gameMenu.GetSelectedOption() == 2) {
          SetCurrentState(std::make_shared<GameState>(GameState::LEADERBOARD));
          break;
        }
      }

      if (_e.key.keysym.sym == SDLK_UP) {
        gameMenu.SetSelectedOption(1);
      } else if (_e.key.keysym.sym == SDLK_DOWN) {
        gameMenu.SetSelectedOption(2);
      }
    }
  }
}

void Controller::HandleLeaderboardInput() {
    while (SDL_PollEvent(&_e) != 0) {
    // ESC to quit the game
    if (_e.type == SDL_QUIT || _e.key.keysym.sym == SDLK_ESCAPE) {
      _quit = true;
      break;
    }

    if (_e.type == SDL_KEYDOWN && _e.key.keysym.sym == SDLK_BACKSPACE) {
      SetCurrentState(std::make_shared<GameState>(GameState::MENU));
      break;
    }
  }
}

void Controller::HandleInput(bool &running, Snake &snake) {
  while (SDL_PollEvent(&_e) != 0) {
    if (_e.type == SDL_QUIT || _e.key.keysym.sym == SDLK_ESCAPE) {
      _quit = true;
      break;
    }

    if (_e.type == SDL_KEYDOWN) {
      switch (_e.key.keysym.sym) {
        case SDLK_UP:
          ChangeDirection(snake, Snake::Direction::kUp,
                          Snake::Direction::kDown);
          break;

        case SDLK_DOWN:
          ChangeDirection(snake, Snake::Direction::kDown,
                          Snake::Direction::kUp);
          break;

        case SDLK_LEFT:
          ChangeDirection(snake, Snake::Direction::kLeft,
                          Snake::Direction::kRight);
          break;

        case SDLK_RIGHT:
          ChangeDirection(snake, Snake::Direction::kRight,
                          Snake::Direction::kLeft);
          break;
        case SDLK_RETURN:
        case SDLK_RETURN2:
          if (!snake.alive) {
            SetReset(std::make_shared<bool>(true));
            snake.Reborn();
          }
          break;
        case SDLK_BACKSPACE:
          if (!snake.alive) {
            SetCurrentState(std::make_shared<GameState>(GameState::MENU));
            SetReset(std::make_shared<bool>(true));
            snake.Reborn();
          }
      }
    }
  }
}