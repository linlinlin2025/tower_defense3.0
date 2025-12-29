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

void startNewWave() {
    // 进入下一波
    game.wave++;
    game.enemiesSpawned = 0;
    game.enemySpawnTimer = 0;

    // 计算下一波的敌人数量
    game.enemiesInWave = config.initialEnemiesInWave +
        (game.wave - 1) * config.waveEnemyIncrease;

    // 开始生成敌人
    game.waveSpawning = 1;
    game.waveStarted = 1;

    // 波次开始提示（可选）
    printf("第 %d 波开始！敌人数量：%d\n", game.wave, game.enemiesInWave);
}

// 完成当前波次的逻辑
void completeCurrentWave() {
    // 波次完成奖励
    game.money += 50 + game.wave * 10;
    game.score += 100 * game.gameLevel;

    // 显示波次完成信息
    printf("第 %d 波完成！奖励：%d金钱，%d分数\n",
        game.wave - 1, 50 + (game.wave - 1) * 10, 100 * game.gameLevel);

    // 重新激活开始波次按钮
    game.buttons[BUTTON_START_WAVE].active = 1;
}

// 整合后的波次管理系统（替换原来的updateWaveSpawning）
void updateWaveSystem(float deltaTime) {
    // 如果游戏暂停或结束，不更新波次
    if (game.gamePaused || game.gameOver) return;

    // 如果波次未开始，不处理波次逻辑
    if (!game.waveStarted) return;

    // 情况1：正在生成敌人
    if (game.waveSpawning) {
        // 更新生成计时器
        game.enemySpawnTimer += deltaTime;

        // 每1秒生成一个敌人（根据游戏速度调整）
        float spawnInterval = 1.0f / game.gameSpeed;
        if (game.enemySpawnTimer >= spawnInterval &&
            game.enemiesSpawned < game.enemiesInWave) {

            // 随机选择敌人类型和路径
            EnemyType type = (EnemyType)(rand() % MAX_ENEMY_TYPES);
            int pathId = rand() % NUM_PATHS;

            spawnEnemy(type, pathId);
            game.enemySpawnTimer = 0;  // 重置计时器
        }

        // 检查是否所有敌人都已生成
        if (game.enemiesSpawned >= game.enemiesInWave) {
            // 所有敌人都已生成，停止生成状态
            game.waveSpawning = 0;
            game.waveDelayTimer = 0;  // 重置波次间延迟计时器

            // 如果当前没有敌人存活，直接完成波次
            if (game.enemyCount == 0) {
                completeCurrentWave();
                game.waveStarted = 0;  // 波次结束
            }
        }
    }
    // 情况2：波次间延迟或等待敌人全部死亡
    else {
        // 如果还有敌人存活，等待敌人全部死亡
        if (game.enemyCount > 0) {
            // 什么都不做，等待updateEnemies处理敌人移动和死亡
            return;
        }
        // 所有敌人都已死亡，波次完成
        else {
            // 延迟一小段时间再完成波次（可选，增加游戏节奏感）
            game.waveDelayTimer += deltaTime;
            if (game.waveDelayTimer >= 1.0f) {  // 1秒延迟
                completeCurrentWave();
                game.waveStarted = 0;  // 波次结束
                game.waveDelayTimer = 0;
            }
        }
    }
}

// 更新所有敌人
// 更新所有敌人
void updateEnemies(float deltaTime) {
    static int debugCounter = 0;
    
    for (int i = 0; i < game.enemyCount; i++) {
        Enemy* e = &game.enemies[i];

        // 每60帧输出一次调试信息
        if (debugCounter++ % 60 == 0) {
            printf("敌人 %d: 位置(%.2f, %.2f), 速度%.2f, 路径点%d/%d\n",
                i, e->x, e->y, e->speed,
                e->waypointIndex, game.pathCounts[e->pathId] - 1);
        }
    }
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

        // 计算到目标的向量（使用像素坐标计算）
        float targetX = target->x * GRID_SIZE + GRID_SIZE / 2.0f;
        float targetY = target->y * GRID_SIZE + GRID_SIZE / 2.0f;

        // 将当前网格坐标转换为像素坐标
        float currentX = e->x * GRID_SIZE + GRID_SIZE / 2.0f;
        float currentY = e->y * GRID_SIZE + GRID_SIZE / 2.0f;

        float dx = targetX - currentX;
        float dy = targetY - currentY;
        float distance = sqrt(dx * dx + dy * dy);

        // 如果已经非常接近目标点，直接到达
        if (distance < 1.0f) {
            e->x = target->x;
            e->y = target->y;
            e->waypointIndex++;
            continue;
        }

        // 计算移动步长（像素/秒）
        float moveStep = e->speed * deltaTime;

        // 确保至少移动最小距离
        if (moveStep < 1.0f) moveStep = 1.0f;

        // 如果移动步长大于到目标的距离，直接到达目标
        if (moveStep >= distance) {
            e->x = target->x;
            e->y = target->y;
            e->waypointIndex++;
        }
        else {
            // 按比例移动到目标
            float ratio = moveStep / distance;
            currentX += dx * ratio;
            currentY += dy * ratio;

            // 将像素坐标转换回网格坐标
            e->x = currentX / GRID_SIZE;
            e->y = currentY / GRID_SIZE;
        }
    }
}

// 绘制所有敌人
// 绘制所有敌人
void drawEnemies() {
    for (int i = 0; i < game.enemyCount; i++) {
        Enemy* e = &game.enemies[i];

        // 计算绘制位置（网格坐标转换为像素坐标）
        int drawX = (int)(e->x * GRID_SIZE + GRID_SIZE / 2.0f);
        int drawY = (int)(e->y * GRID_SIZE + GRID_SIZE / 2.0f);

        // 绘制敌人身体
        setfillcolor(config.enemyColors[e->type]);
        fillcircle(drawX, drawY, GRID_SIZE / 3);

        // 绘制生命条背景
        setfillcolor(RGB(100, 0, 0));
        fillrectangle(drawX - GRID_SIZE / 2 + 2,
            drawY - GRID_SIZE / 2 - 8,
            drawX + GRID_SIZE / 2 - 2,
            drawY - GRID_SIZE / 2 - 4);

        // 绘制当前生命条
        float healthRatio = (float)e->health / e->maxHealth;
        int barWidth = (int)((GRID_SIZE - 4) * healthRatio);

        setfillcolor(RGB(0, 200, 0));
        fillrectangle(drawX - GRID_SIZE / 2 + 2,
            drawY - GRID_SIZE / 2 - 8,
            drawX - GRID_SIZE / 2 + 2 + barWidth,
            drawY - GRID_SIZE / 2 - 4);

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
        outtextxy(drawX - textWidth / 2,
            drawY - textHeight / 2,
            &typeChar);
    }
}

// 初始化多条路径函数
void initPaths() {
    // 初始化第一条路径（原路径）
    game.pathCounts[0] = 0;
    // 起点（左下角）
    game.paths[0][game.pathCounts[0]].x = 0;
    game.paths[0][game.pathCounts[0]].y = MAP_HEIGHT - 1;
    game.pathCounts[0]++;

    // 向右移动
    game.paths[0][game.pathCounts[0]].x = 5;
    game.paths[0][game.pathCounts[0]].y = MAP_HEIGHT - 1;
    game.pathCounts[0]++;

    // 向上移动
    game.paths[0][game.pathCounts[0]].x = 5;
    game.paths[0][game.pathCounts[0]].y = MAP_HEIGHT - 4;
    game.pathCounts[0]++;

    // 向右移动
    game.paths[0][game.pathCounts[0]].x = 10;
    game.paths[0][game.pathCounts[0]].y = MAP_HEIGHT - 4;
    game.pathCounts[0]++;

    // 向下移动
    game.paths[0][game.pathCounts[0]].x = 10;
    game.paths[0][game.pathCounts[0]].y = MAP_HEIGHT - 1;
    game.pathCounts[0]++;

    // 终点（右下角）
    game.paths[0][game.pathCounts[0]].x = MAP_WIDTH - 1;
    game.paths[0][game.pathCounts[0]].y = MAP_HEIGHT - 1;
    game.pathCounts[0]++;

    // 初始化第二条路径（从左上到右下）
    game.pathCounts[1] = 0;
    // 起点（左上角）
    game.paths[1][game.pathCounts[1]].x = 0;
    game.paths[1][game.pathCounts[1]].y = 0;
    game.pathCounts[1]++;

    // 向右下移动
    game.paths[1][game.pathCounts[1]].x = 3;
    game.paths[1][game.pathCounts[1]].y = 3;
    game.pathCounts[1]++;

    // 向右移动
    game.paths[1][game.pathCounts[1]].x = 7;
    game.paths[1][game.pathCounts[1]].y = 3;
    game.pathCounts[1]++;

    // 向右下移动
    game.paths[1][game.pathCounts[1]].x = 10;
    game.paths[1][game.pathCounts[1]].y = 6;
    game.pathCounts[1]++;

    // 向右移动
    game.paths[1][game.pathCounts[1]].x = 14;
    game.paths[1][game.pathCounts[1]].y = 6;
    game.pathCounts[1]++;

    // 终点（右下角）
    game.paths[1][game.pathCounts[1]].x = MAP_WIDTH - 1;
    game.paths[1][game.pathCounts[1]].y = MAP_HEIGHT - 1;
    game.pathCounts[1]++;

    // 初始化第三条路径（从顶部中央到底部中央）
    game.pathCounts[2] = 0;
    // 起点（顶部中央）
    game.paths[2][game.pathCounts[2]].x = MAP_WIDTH / 2;
    game.paths[2][game.pathCounts[2]].y = 0;
    game.pathCounts[2]++;

    // 向下移动
    game.paths[2][game.pathCounts[2]].x = MAP_WIDTH / 2;
    game.paths[2][game.pathCounts[2]].y = 3;
    game.pathCounts[2]++;

    // 向右移动
    game.paths[2][game.pathCounts[2]].x = MAP_WIDTH / 2 + 3;
    game.paths[2][game.pathCounts[2]].y = 3;
    game.pathCounts[2]++;

    // 向下移动
    game.paths[2][game.pathCounts[2]].x = MAP_WIDTH / 2 + 3;
    game.paths[2][game.pathCounts[2]].y = 7;
    game.pathCounts[2]++;

    // 向左移动
    game.paths[2][game.pathCounts[2]].x = MAP_WIDTH / 2 - 2;
    game.paths[2][game.pathCounts[2]].y = 7;
    game.pathCounts[2]++;

    // 终点（底部中央）
    game.paths[2][game.pathCounts[2]].x = MAP_WIDTH / 2 - 2;
    game.paths[2][game.pathCounts[2]].y = MAP_HEIGHT - 1;
    game.pathCounts[2]++;

    // 初始化路径网格
    initPathGrid();
}
void drawPaths()
{
    // 绘制三条路径，使用不同颜色区分
    COLORREF pathColors[NUM_PATHS] = {
        RGB(150, 100, 50),   // 路径0：棕色
        RGB(50, 150, 100),   // 路径1：蓝绿色
        RGB(100, 50, 150)    // 路径2：紫色
    };

    // 绘制每条路径
    for (int pathId = 0; pathId < NUM_PATHS; pathId++) {
        setlinecolor(pathColors[pathId]);  // 设置路径颜色
        setlinestyle(PS_SOLID, 3);         // 设置线宽为3像素

        // 绘制路径线段
        for (int i = 0; i < game.pathCounts[pathId] - 1; i++) {
            int x1 = game.paths[pathId][i].x * GRID_SIZE + GRID_SIZE / 2;
            int y1 = game.paths[pathId][i].y * GRID_SIZE + GRID_SIZE / 2;
            int x2 = game.paths[pathId][i + 1].x * GRID_SIZE + GRID_SIZE / 2;
            int y2 = game.paths[pathId][i + 1].y * GRID_SIZE + GRID_SIZE / 2;
            line(x1, y1, x2, y2);
        }
    }
    setlinestyle(PS_SOLID, 1);  // 恢复默认线宽

    // 绘制每条路径的起点和终点
    for (int pathId = 0; pathId < NUM_PATHS; pathId++) {
        // 起点（绿色）
        setfillcolor(RGB(0, 150 + pathId * 30, 0));  // 不同深度的绿色
        fillrectangle(game.paths[pathId][0].x * GRID_SIZE,
            game.paths[pathId][0].y * GRID_SIZE,
            (game.paths[pathId][0].x + 1) * GRID_SIZE,
            (game.paths[pathId][0].y + 1) * GRID_SIZE);

        // 终点（红色）
        setfillcolor(RGB(150 + pathId * 30, 0, 0));  // 不同深度的红色
        fillrectangle(game.paths[pathId][game.pathCounts[pathId] - 1].x * GRID_SIZE,
        game.paths[pathId][game.pathCounts[pathId] - 1].y * GRID_SIZE,
        (game.paths[pathId][game.pathCounts[pathId] - 1].x + 1) * GRID_SIZE,
        (game.paths[pathId][game.pathCounts[pathId] - 1].y + 1) * GRID_SIZE);
    }
}