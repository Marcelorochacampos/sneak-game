#ifndef GAME_MENU_H
#define GAME_MENU_H

#include <string>
#include <vector>
#include "SDL.h"

struct MenuOption {
  int id;
  std::string title;
  SDL_Rect position;
};

class GameMenu {
public:
  GameMenu();
  void SetSelectedOption(int v) { _selectedOption = v; }
  int GetSelectedOption() { return _selectedOption; }
  std::vector<MenuOption> GetOptions() { return _options; }

private:
  std::vector<MenuOption> _options;
  int _selectedOption;
};

#endif