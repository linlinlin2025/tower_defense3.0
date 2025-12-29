// game_rule.h - 添加函数声明
#ifndef GAME_RULE_H
#define GAME_RULE_H

#include "game_core.h"

// 最高分记录数量
#define MAX_HIGH_SCORES 5

// 最高分记录结构体
typedef struct {
    char name[20];
    int score;
    int level;
} HighScore;

// 全局最高分变量声明
extern HighScore highScores[MAX_HIGH_SCORES];
extern int highScoreCount;

// 游戏规则函数声明
int canBuildAt(int gridX, int gridY);
void updateHighScoreDisplay();

// 最高分管理函数
void loadHighScores();
void saveHighScore();
void updateHighScores();

void initButtons();
void drawButton(Button* btn);
#endif