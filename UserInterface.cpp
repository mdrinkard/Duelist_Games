#include "UserInterface.h"
#include <raylib.h>

std::deque<std::string> CombatLog;
const int MaxLogEntries = 10;


void DrawUIBackgrounds(int screenWidth, int screenHeight)
{
    int uiHeight = 250;
    int leftWidth = screenWidth * 2 / 3;    // Left panel width (stats)
    int rightWidth = screenWidth - leftWidth; // Right panel width (combat log)

    // Left panel background (stats)
    DrawRectangle(0, screenHeight - uiHeight, leftWidth, uiHeight, DARKGRAY);

    // Right panel background (combat log)
    DrawRectangle(leftWidth, screenHeight - uiHeight, rightWidth, uiHeight, DARKBLUE);
}

// Draw Player and Enemy stats inside the left panel area
void DrawStatsUI(const Player& player, const Enemy& enemy, int screenWidth, int screenHeight)
{
    int uiHeight = 250;
    int leftWidth = screenWidth * 2 / 3;

    int padding = 20;
    int startX = padding;                      // Left panel left edge + padding
    int startY = screenHeight - uiHeight + padding;
    int lineHeight = 25;

    // Player Info
    DrawText("Player Info", startX, startY, 24, GREEN);
    DrawText(("Name: " + player.GetName()).c_str(), startX, startY + lineHeight * 1, 20, LIGHTGRAY);
    DrawText(("HP: " + std::to_string(player.GetHealth())).c_str(), startX, startY + lineHeight * 2, 20, LIGHTGRAY);
    DrawText(("Stamina: " + std::to_string(player.GetStamina())).c_str(), startX, startY + lineHeight * 3, 20, LIGHTGRAY);
    DrawText(("Armor: " + std::to_string(player.GetArmor())).c_str(), startX, startY + lineHeight * 4, 20, LIGHTGRAY);
    DrawText(("ATK: " + std::to_string(player.GetAtkPower())).c_str(), startX, startY + lineHeight * 5, 20, LIGHTGRAY);
    DrawText(("Equipped: " + player.GetEquippedItem().Name).c_str(), startX, startY + lineHeight * 6, 20, LIGHTGRAY);

    // Enemy Info (shifted right inside left panel)
    int enemyStartX = startX + leftWidth / 2;  // Approximately halfway in the left panel
    DrawText("Enemy Info", enemyStartX, startY, 24, RED);
    DrawText(("Name: " + enemy.GetName()).c_str(), enemyStartX, startY + lineHeight * 1, 20, LIGHTGRAY);
    DrawText(("HP: " + std::to_string(enemy.GetHealth())).c_str(), enemyStartX, startY + lineHeight * 2, 20, LIGHTGRAY);
    DrawText(("Armor: " + std::to_string(enemy.GetArmor())).c_str(), enemyStartX, startY + lineHeight * 3, 20, LIGHTGRAY);
    DrawText(("ATK: " + std::to_string(enemy.GetAtkPower())).c_str(), enemyStartX, startY + lineHeight * 4, 20, LIGHTGRAY);
}

// Draw Combat Log inside the right panel area
void DrawCombatLog(int screenWidth, int screenHeight)
{
    int uiHeight = 250;
    int leftWidth = screenWidth * 2 / 3;

    int padding = 20;
    int startX = leftWidth + padding;            // Right panel start + padding
    int startY = screenHeight - uiHeight + padding;
    int lineHeight = 20;

    DrawText("Combat Log", startX, startY, 24, ORANGE);

    for (int i = 0; i < CombatLog.size(); ++i)
    {
        DrawText(CombatLog[i].c_str(), startX, startY + lineHeight * (i + 1), 18, LIGHTGRAY);
    }
}

void DrawHealthBars(const Player& player, const Enemy& enemy, int screenWidth, int screenHeight)
{
    int barWidth = 300;
    int barHeight = 20;
    int padding = 20;

    // Positions
    int playerX = padding;
    int enemyX = screenWidth - barWidth - padding;
    int barY = screenHeight - 270; // Just above the UI rectangles

    // Player Health
    float playerHealthRatio = (float)player.GetHealth() / player.GetMaxHealth();
    DrawRectangle(playerX, barY, barWidth, barHeight, DARKGRAY); // Background
    DrawRectangle(playerX, barY, (int)(barWidth * playerHealthRatio), barHeight, GREEN); // Fill
    DrawText(player.GetName().c_str(), playerX, barY - 20, 18, WHITE);

    // Enemy Health
    float enemyHealthRatio = (float)enemy.GetHealth() / enemy.GetMaxHealth();
    DrawRectangle(enemyX, barY, barWidth, barHeight, DARKGRAY); // Background
    DrawRectangle(enemyX, barY, (int)(barWidth * enemyHealthRatio), barHeight, RED); // Fill
    DrawText(enemy.GetName().c_str(), enemyX, barY - 20, 18, WHITE);
}



void AddCombatLogEntry(const std::string& entry)
{
    if (CombatLog.size() >= MaxLogEntries)
        CombatLog.pop_front();

    CombatLog.push_back(entry);
}