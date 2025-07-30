#pragma once

#include <string>
#include <deque>
#include "Player.h"
#include "Enemy.h"

extern std::deque<std::string> CombatLog;
extern const int MaxLogEntries;

void DrawStatsUI(const Player& player, const Enemy& enemy, int screenWidth, int screenHeight);
void DrawCombatLog(int screenWidth, int screenHeight);
void AddCombatLogEntry(const std::string& entry);
void DrawUIBackgrounds(int screenWidth, int screenHeight);
void DrawHealthBars(const Player& player, const Enemy& enemy, int screenWidth, int screenHeight);