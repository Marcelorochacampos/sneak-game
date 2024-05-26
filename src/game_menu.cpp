#include <vector>

#include "game_menu.h"

GameMenu::GameMenu() {
  std::vector<MenuOption> options = {
    {1, "Play", {100, 130, 0, 0}},
    {2, "Leaderboard", {100, 160, 0, 0}},
  };

  for(auto option : options) {
    _options.emplace_back(option);
  }

  _selectedOption = 1;
}