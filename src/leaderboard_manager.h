#ifndef LEADERBOARD_MANAGER_H
#define LEADERBOARD_MANAGER_H

#include <vector>
#include <future>

struct LeaderboardPlayer {
  std::string username;
  int score;
  LeaderboardPlayer(const std::string &u, int s) : username(u), score(s) {}
};

class LeaderboardManager {
public:
  LeaderboardManager();
  void GetLeaderboardPlayers(std::promise<std::vector<LeaderboardPlayer>> prms);
  void AddPlayerToLeaderboard(std::string username, int score);

private:
  std::vector<LeaderboardPlayer> _leaderboardPlayers;
};

#endif