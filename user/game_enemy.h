// game_enemy.h
#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include "game_core.h"

// 敌人配置函数
void initEnemyConfig(GameConfig* config);

// 敌人生成函数
void spawnEnemy(EnemyType type, int pathId);
void updateWaveSystem(float deltaTime);  // 替换原来的updateWaveSpawning
void startNewWave();  // 从game_rule移动过来
void completeCurrentWave();  // 完成当前波次的逻辑

// 敌人更新函数
void updateEnemies(float deltaTime);
void drawEnemies();

void drawPaths();
void initPaths();

#endif // GAME_ENEMY_H