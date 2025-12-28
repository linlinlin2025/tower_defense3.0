#ifndef GAME_RULE_H
#define GAME_RULE_H

#include "game_core.h"

// 游戏规则相关函数声明
void generateNextTower();
int canBuildAt(int gridX, int gridY);
void updateWave();
void startNewWave();
void updateHighScoreDisplay();

#endif
