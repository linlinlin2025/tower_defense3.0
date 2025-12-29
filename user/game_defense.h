#ifndef GAME_DEFENSE_H
#define GAME_DEFENSE_H

#include "game_core.h"

// 炮台配置函数
void initTowerConfig(GameConfig* config);

// 炮台操作函数
void placeTower(int gridX, int gridY);
void selectTower(int gridX, int gridY);
void upgradeSelectedTower();
void removeSelectedTower();
void updateTowers(float deltaTime);
void drawTowers();

// 子弹函数
void createBullet(Tower* tower, Enemy* target);
void updateBullets(float deltaTime);
void drawBullets();

// 预览炮台函数
void generateNextTower();

// 炮台升级计算函数
int calculateUpgradeCost(Tower* tower);
int calculateSellValue(Tower* tower);

#endif // GAME_DEFENSE_H