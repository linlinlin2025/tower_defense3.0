#define _CRT_SECURE_NO_WARNINGS
#include "game_enemy.h"
#include <graphics.h>
#include <math.h>

// 初始化敌人配置
void initEnemyConfig(GameConfig* config) {
    // 敌人得分配置
    config->enemyScores[ENEMY_BASIC] = 10;  // 基础敌人得分
    config->enemyScores[ENEMY_FAST] = 20;   // 快速敌人得分
    config->enemyScores[ENEMY_TANK] = 30;   // 坦克敌人得分

    // 敌人生命值
    config->enemyHealth[ENEMY_BASIC] = 50;
    config->enemyHealth[ENEMY_FAST] = 30;
    config->enemyHealth[ENEMY_TANK] = 150;

    // 敌人速度（像素/秒）
    config->enemySpeed[ENEMY_BASIC] = 60.0f;   // 1.5格/秒
    config->enemySpeed[ENEMY_FAST] = 120.0f;   // 3.0格/秒
    config->enemySpeed[ENEMY_TANK] = 32.0f;    // 0.8格/秒

    // 敌人金钱奖励
    config->enemyGoldReward[ENEMY_BASIC] = 10;
    config->enemyGoldReward[ENEMY_FAST] = 15;
    config->enemyGoldReward[ENEMY_TANK] = 25;

    // 敌人对基地的伤害
    config->enemyDamage[ENEMY_BASIC] = 5;
    config->enemyDamage[ENEMY_FAST] = 8;
    config->enemyDamage[ENEMY_TANK] = 15;

    // 敌人颜色
    config->enemyColors[ENEMY_BASIC] = RGB(150, 150, 150);  // 灰色
    config->enemyColors[ENEMY_FAST] = RGB(100, 200, 100);   // 绿色
    config->enemyColors[ENEMY_TANK] = RGB(200, 100, 100);   // 红色
}

// 生成敌人
void spawnEnemy(EnemyType type, int pathId) {
    if (game.enemyCount >= MAX_ENEMIES) return;

    Enemy* e = &game.enemies[game.enemyCount];

    e->type = type;
    e->health = config.enemyHealth[type];
    e->maxHealth = config.enemyHealth[type];
    e->speed = config.enemySpeed[type];
    e->pathId = pathId;
    e->waypointIndex = 0;
    e->moneyReward = config.enemyGoldReward[type];
    e->scoreReward = config.enemyScores[type];
    e->baseDamage = config.enemyDamage[type];

    // 应用波次强化
    if (game.wave > 1) {
        float waveMultiplier = 1.0f + (game.wave - 1) * 0.2f; // 每波增加20%
        e->health = (int)(e->health * waveMultiplier);
        e->maxHealth = e->health;
        e->speed *= waveMultiplier;
        e->moneyReward = (int)(e->moneyReward * waveMultiplier);
        e->scoreReward = (int)(e->scoreReward * waveMultiplier);
    }

    // 设置初始位置为路径起点
    if (pathId < NUM_PATHS && game.pathCounts[pathId] > 0) {
        e->x = game.paths[pathId][0].x;
        e->y = game.paths[pathId][0].y;
    }

    game.enemyCount++;
    game.enemiesSpawned++;
}

// 更新波次生成
void updateWaveSpawning(float deltaTime) {
    if (!game.waveSpawning || game.enemiesSpawned >= game.enemiesInWave) {
        // 如果所有敌人都已生成，检查是否还有敌人存活
        if (game.enemyCount == 0 && game.enemiesSpawned >= game.enemiesInWave) {
            game.waveSpawning = 0;
            game.waveStarted = 0;

            // 进入下一波
            game.wave++;
            game.enemiesSpawned = 0;

            // 增加下一波的敌人数量
            game.enemiesInWave = config.initialEnemiesInWave +
                (game.wave - 1) * config.waveEnemyIncrease;

            // 重新激活开始波次按钮
            game.buttons[BUTTON_START_WAVE].active = 1;
        }
        return;
    }

    // 更新生成计时器
    game.enemySpawnTimer += deltaTime;

    // 每1秒生成一个敌人
    if (game.enemySpawnTimer >= 1.0f) {
        // 随机选择敌人类型和路径
        EnemyType type = (EnemyType)(rand() % MAX_ENEMY_TYPES);
        int pathId = rand() % NUM_PATHS;

        spawnEnemy(type, pathId);
        game.enemySpawnTimer = 0;
    }
}

// 更新所有敌人
void updateEnemies(float deltaTime) {
    for (int i = 0; i < game.enemyCount; i++) {
        Enemy* e = &game.enemies[i];

        // 检查是否到达终点
        if (e->waypointIndex >= game.pathCounts[e->pathId] - 1) {
            // 对基地造成伤害
            game.life -= e->baseDamage;

            // 移除敌人
            game.enemies[i] = game.enemies[game.enemyCount - 1];
            game.enemyCount--;
            i--;
            continue;
        }

        // 获取当前目标路径点
        Point* target = &game.paths[e->pathId][e->waypointIndex + 1];

        // 计算到目标的向量
        float targetX = target->x * GRID_SIZE + GRID_SIZE / 2;
        float targetY = target->y * GRID_SIZE + GRID_SIZE / 2;
        float currentX = e->x * GRID_SIZE + GRID_SIZE / 2;
        float currentY = e->y * GRID_SIZE + GRID_SIZE / 2;

        float dx = targetX - currentX;
        float dy = targetY - currentY;
        float distance = sqrt(dx * dx + dy * dy);

        // 移动到目标路径点
        float moveDistance = e->speed * deltaTime;

        if (moveDistance >= distance) {
            // 到达路径点
            e->x = target->x;
            e->y = target->y;
            e->waypointIndex++;
        }
        else {
            // 向目标移动
            e->x += dx / distance * moveDistance / GRID_SIZE;
            e->y += dy / distance * moveDistance / GRID_SIZE;
        }
    }
}

// 绘制所有敌人
void drawEnemies() {
    for (int i = 0; i < game.enemyCount; i++) {
        Enemy* e = &game.enemies[i];

        // 绘制敌人身体
        setfillcolor(config.enemyColors[e->type]);
        fillcircle(e->x * GRID_SIZE + GRID_SIZE / 2,
            e->y * GRID_SIZE + GRID_SIZE / 2,
            GRID_SIZE / 3);

        // 绘制生命条背景
        setfillcolor(RGB(100, 0, 0));
        fillrectangle(e->x * GRID_SIZE + 2,
            e->y * GRID_SIZE - 8,
            (e->x + 1) * GRID_SIZE - 2,
            e->y * GRID_SIZE - 4);

        // 绘制当前生命条
        float healthRatio = (float)e->health / e->maxHealth;
        int barWidth = (int)((GRID_SIZE - 4) * healthRatio);

        setfillcolor(RGB(0, 200, 0));
        fillrectangle(e->x * GRID_SIZE + 2,
            e->y * GRID_SIZE - 8,
            e->x * GRID_SIZE + 2 + barWidth,
            e->y * GRID_SIZE - 4);

        // 绘制敌人类型标识
        settextcolor(RGB(255, 255, 255));
        setbkmode(TRANSPARENT);
        TCHAR typeChar;
        switch (e->type) {
        case ENEMY_BASIC: typeChar = 'B'; break;
        case ENEMY_FAST: typeChar = 'F'; break;
        case ENEMY_TANK: typeChar = 'T'; break;
        default: typeChar = '?';
        }

        int textWidth = textwidth(&typeChar);
        int textHeight = textheight(&typeChar);
        outtextxy(e->x * GRID_SIZE + (GRID_SIZE - textWidth) / 2,
            e->y * GRID_SIZE + (GRID_SIZE - textHeight) / 2,
            &typeChar);
    }
}
