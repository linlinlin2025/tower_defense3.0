#define _CRT_SECURE_NO_WARNINGS
#include "game_rule.h"
#include <stdio.h>
#include <string.h>
#include <graphics.h>

// 最高分全局变量定义
HighScore highScores[MAX_HIGH_SCORES];
int highScoreCount = 0;



// 检查网格是否可建造
int canBuildAt(int gridX, int gridY) {
    // 检查是否在地图范围内
    if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT) {
        return 0;
    }

    // 检查是否在路径上
    if (game.pathGrid[gridX][gridY] == 1) {
        return 0;
    }

    // 检查是否已经有炮台
    for (int i = 0; i < game.towerCount; i++) {
        if (game.towers[i].x == gridX && game.towers[i].y == gridY) {  // 修改这里：gridX → x, gridY → y
            return 0;
        }
    }

    return 1;
}

// 波次管理函数
void updateWave() {
    // 如果波次已开始且正在生成敌人
    if (game.waveStarted && game.waveSpawning) {
        // 更新敌人生成计时器
        game.enemySpawnTimer++;

        // 生成敌人（大约每秒一个）
        if (game.enemySpawnTimer >= 60 / game.gameSpeed) {
            if (game.enemiesSpawned < game.enemiesInWave) {
                // 由game_enemy模块处理
                game.enemySpawnTimer = 0;  // 重置计时器
            }
        }

        // 检查当前波次是否完成
        if (game.enemiesSpawned >= game.enemiesInWave &&
            game.enemyCount == 0) {
            // 当前波次完成
            game.waveSpawning = 0;  // 停止生成敌人
            game.waveDelayTimer = 0; // 开始波次间延迟计时

            // 波次完成奖励
            game.money += 50 + game.wave * 10;
            game.score += 100 * game.gameLevel;

            // 更新最高分显示
            updateHighScoreDisplay();
        }
    }

    // 波次间延迟处理
    if (!game.waveSpawning && game.waveStarted) {
        game.waveDelayTimer++;

        // 延迟约2秒后开始下一波
        if (game.waveDelayTimer > 120) {
            startNewWave();
            game.waveDelayTimer = 0;
        }
    }
}

// 开始新一波敌人
void startNewWave() {
    game.wave++;
    game.enemiesSpawned = 0;
    game.enemySpawnTimer = 0;
    game.enemiesInWave = 5 + game.wave * 2;

    // 每5波增加难度
    if (game.wave % 5 == 0) {
        game.enemiesInWave += 5;
    }

    // 开始生成敌人
    game.waveSpawning = 1;
}

// 更新最高分显示
void updateHighScoreDisplay() {
    if (game.score > game.highScore) {
        game.highScore = game.score;
    }
}

// 从文件读取最高分记录（从game_core.cpp移过来）
void loadHighScores() {
    FILE* file = fopen("highscores.dat", "rb");  // 以二进制只读方式打开文件
    if (file) {
        // 读取记录数量
        fread(&highScoreCount, sizeof(int), 1, file);
        // 确保不超过最大记录数
        if (highScoreCount > MAX_HIGH_SCORES) highScoreCount = MAX_HIGH_SCORES;
        // 读取记录数据
        fread(highScores, sizeof(HighScore), highScoreCount, file);
        fclose(file);

        // 设置当前最高分显示
        if (highScoreCount > 0) {
            game.highScore = highScores[0].score;  // 最高分是第一个记录
        }
    }
}

// 保存最高分记录到文件（从game_core.cpp移过来）
void saveHighScore() {
    FILE* file = fopen("highscores.dat", "wb");  // 以二进制写入方式打开文件
    if (file) {
        // 写入记录数量
        fwrite(&highScoreCount, sizeof(int), 1, file);
        // 写入记录数据
        fwrite(highScores, sizeof(HighScore), highScoreCount, file);
        fclose(file);
    }
}

// 更新最高分记录函数（从game_core.cpp移过来）
void updateHighScores() {
    // 如果当前得分超过最低记录或记录未满
    if (highScoreCount < MAX_HIGH_SCORES || game.score > highScores[MAX_HIGH_SCORES - 1].score) {
        // 创建新记录
        HighScore newScore;
        strcpy(newScore.name, "Player");  // 默认玩家名
        newScore.score = game.score;      // 当前得分
        newScore.level = game.gameLevel;  // 当前等级

        // 插入排序：找到合适的位置插入新记录
        int i;
        for (i = highScoreCount - 1; i >= 0; i--) {
            // 如果是第一个位置或得分低于前一个记录
            if (i == 0 || game.score <= highScores[i - 1].score) {
                if (i < MAX_HIGH_SCORES - 1) {
                    highScores[i] = newScore;  // 插入新记录
                }
                break;
            }
            else {
                if (i < MAX_HIGH_SCORES - 1) {
                    highScores[i] = highScores[i - 1];  // 后移记录
                }
            }
        }

        // 如果是在数组开始位置插入
        if (i < 0 && highScoreCount < MAX_HIGH_SCORES) {
            highScores[0] = newScore;
        }

        // 如果记录数未满，增加计数
        if (highScoreCount < MAX_HIGH_SCORES) {
            highScoreCount++;
        }

        // 保存到文件
        saveHighScore();

        // 更新游戏中的最高分显示
        game.highScore = highScores[0].score;
    }
}
