// game_enemy.h
#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include "game_core.h"

// 敌人配置函数
void initEnemyConfig(GameConfig* config);

// 敌人生成函数
void spawnEnemy(EnemyType type, int pathId);
void updateWaveSpawning(float deltaTime);

// 敌人更新函数
void updateEnemies(float deltaTime);
void drawEnemies();

#endif // GAME_ENEMY_H