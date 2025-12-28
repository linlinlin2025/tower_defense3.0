#ifndef GAME_DEFENSE_H
#define GAME_DEFENSE_H

#include "game_core.h"

// 防御相关函数声明
void placeTower(int gridX, int gridY);
void selectTowerAt(int gridX, int gridY);
void upgradeSelectedTower();
void removeSelectedTower();
void updateTowers();
void updateBullets();
void drawTowers();
void drawBullets();

#endif