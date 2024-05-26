#include <iostream>
#include "controller.h"
#include "game_manager.h"

int main(int argc, char* argv[]) {
  constexpr std::size_t kFramesPerSecond{60};
  constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};
  constexpr std::size_t kScreenWidth{640};
  constexpr std::size_t kScreenHeight{640};
  constexpr std::size_t kGridWidth{32};
  constexpr std::size_t kGridHeight{32};

  Controller controller;
  GameManager gameManager(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight, controller);
  std::cout << "Game has terminated successfully!\n";
  return 0;
}