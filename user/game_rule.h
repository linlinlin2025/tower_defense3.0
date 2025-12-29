#define _CRT_SECURE_NO_WARNINGS
#ifndef GAME_RULE_H
#define GAME_RULE_H

#include "game_core.h"

// 最高分记录数量
#define MAX_HIGH_SCORES 5

// 最高分记录结构体（从game_core.h移过来）
typedef struct {
    char name[20];  // 玩家名称
    int score;      // 得分
    int level;      // 达到的等级
} HighScore;

// 全局最高分变量声明（在game_rule.cpp中定义）
extern HighScore highScores[MAX_HIGH_SCORES];
extern int highScoreCount;

// 游戏规则函数声明
void generateNextTower();
int canBuildAt(int gridX, int gridY);
void updateWave();
void startNewWave();
void updateHighScoreDisplay();

// 最高分管理函数（从game_core.cpp移过来）
void loadHighScores();
void saveHighScores();
void updateHighScores();

#endif
