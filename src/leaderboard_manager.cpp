#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <future>
#include <thread>
#include "leaderboard_manager.h"

LeaderboardManager::LeaderboardManager() {
    std::promise<std::vector<LeaderboardPlayer> > prms;
    std::future f = prms.get_future();
    std::thread t(&LeaderboardManager::GetLeaderboardPlayers, this, std::move(prms));
    t.join();
    _leaderboardPlayers = f.get();
}

void LeaderboardManager::GetLeaderboardPlayers(std::promise<std::vector<LeaderboardPlayer>> prms) {
  if (_leaderboardPlayers.size() > 0) {
    prms.set_value(_leaderboardPlayers);
    return;
  }
  
  std::filesystem::path source_directory = std::filesystem::canonical(std::filesystem::path(__FILE__)).parent_path();
  std::filesystem::path path = source_directory / "leaderboard" / "leaderboard.txt";
  std::vector<LeaderboardPlayer> players;

  std::filesystem::path dir = std::filesystem::path(path.c_str()).parent_path();
  if (!std::filesystem::exists(dir)) {
      if (!std::filesystem::create_directories(dir)) {
          std::cerr << "Failed to create directory: " << dir << std::endl;
          prms.set_value(players);
          return;
      }
  }

  if (!std::filesystem::exists(path)) {
      std::cout << "Leaderboard file not found, creating it ... :" << std::endl;
      std::ofstream ofs(path);
      if (!ofs) {
          std::cerr << "Failed to create the file: " << path << std::endl;
          prms.set_value(players);
          return;
      }
      ofs.close();
  }

  if (!std::filesystem::exists(path.c_str())) {
    std::cout << "Leaderboard file not found, creating it ... :" << std::endl;
    std::ofstream ofs(path.c_str());
    if (!ofs) {
      std::cerr << "Failed to create the file: " << path.c_str() << std::endl;
      prms.set_value(players);
      return;
    }
    ofs.close();
  }

  std::ifstream stream(path.c_str());

  if (!stream.is_open()) {
    std::cerr << "Failed to open the file: " << path.c_str() << std::endl;
    prms.set_value(players);
    return;
  }

  std::string line;
  std::string username;
  std::string score;
  while(std::getline(stream, line)) {
    std::istringstream lStream(line);
    while(lStream >> username >> score) {
      int iScore = std::stoi(score);
      players.emplace_back(username, iScore);
    }
  }

  std::sort(players.begin(), players.end(), [](LeaderboardPlayer aPlayer, LeaderboardPlayer bPlayer) {
    return aPlayer.score > bPlayer.score;
  });
  prms.set_value(players);
  return;
}

void LeaderboardManager::AddPlayerToLeaderboard(std::string username, int score) {
  _leaderboardPlayers.emplace_back(username, score);
  std::sort(_leaderboardPlayers.begin(), _leaderboardPlayers.end(), [](LeaderboardPlayer aPlayer, LeaderboardPlayer bPlayer) {
    return aPlayer.score > bPlayer.score;
  });

  if (_leaderboardPlayers.size() > 10) {
    _leaderboardPlayers.erase(_leaderboardPlayers.begin() + 10, _leaderboardPlayers.end());
  }
  std::filesystem::path source_directory = std::filesystem::canonical(std::filesystem::path(__FILE__)).parent_path();
  std::filesystem::path path = source_directory / "leaderboard" / "leaderboard.txt";
  std::ofstream ofs(path.c_str(), std::ios::trunc);
  if (!ofs.is_open()) {
    std::cerr << "Error opening file: " << path << std::endl;
    return;
  }

  for(auto player : _leaderboardPlayers) {
    ofs << player.username << " " << player.score << std::endl;
  }

  ofs.close();
}