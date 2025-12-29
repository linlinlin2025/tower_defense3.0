// game_defense.cpp
#define _CRT_SECURE_NO_WARNINGS
#include "game_defense.h"
#include <math.h>
#include <graphics.h>

// 初始化炮台和升级配置
void initTowerConfig(GameConfig* config) {
    // 炮台成本配置
    config->towerCosts[TOWER_BASIC] = 100;   // 基础炮塔成本
    config->towerCosts[TOWER_ARROW] = 150;   // 初级箭塔成本
    config->towerCosts[TOWER_CANNON] = 200;  // 加农炮成本
    config->towerCosts[TOWER_MAGIC] = 300;   // 魔法塔成本

    // 炮台伤害配置
    config->towerDamage[TOWER_BASIC] = 10;
    config->towerDamage[TOWER_ARROW] = 15;
    config->towerDamage[TOWER_CANNON] = 25;
    config->towerDamage[TOWER_MAGIC] = 30;

    // 炮台攻击范围（以像素为单位）
    config->towerRange[TOWER_BASIC] = 3 * GRID_SIZE;   // 3格
    config->towerRange[TOWER_ARROW] = 4 * GRID_SIZE;   // 4格
    config->towerRange[TOWER_CANNON] = 3 * GRID_SIZE;  // 3格
    config->towerRange[TOWER_MAGIC] = 5 * GRID_SIZE;   // 5格

    // 炮台攻击速度（每秒攻击次数）
    config->towerAttackSpeed[TOWER_BASIC] = 1.0;   // 每秒1次
    config->towerAttackSpeed[TOWER_ARROW] = 1.5;   // 每秒1.5次
    config->towerAttackSpeed[TOWER_CANNON] = 0.5;  // 每秒0.5次
    config->towerAttackSpeed[TOWER_MAGIC] = 0.8;   // 每秒0.8次

    // 炮台颜色
    config->towerColors[TOWER_BASIC] = RGB(100, 100, 200);   // 蓝色
    config->towerColors[TOWER_ARROW] = RGB(100, 200, 100);   // 绿色
    config->towerColors[TOWER_CANNON] = RGB(200, 100, 100);  // 红色
    config->towerColors[TOWER_MAGIC] = RGB(200, 100, 200);   // 紫色

    // 升级成本配置（现在属于防御模块）
    config->upgradeCosts[0] = 50;   // 1级升2级
    config->upgradeCosts[1] = 100;  // 2级升3级
    config->upgradeCosts[2] = 200;  // 3级升4级

    // 升级伤害倍率
    config->upgradeDamageMultiplier[0] = 1.5;  // 1→2级：伤害增加50%
    config->upgradeDamageMultiplier[1] = 2.0;  // 2→3级：伤害增加100%
    config->upgradeDamageMultiplier[2] = 3.0;  // 3→4级：伤害增加200%

    // 升级范围倍率
    config->upgradeRangeMultiplier[0] = 1.2;   // 1→2级：范围增加20%
    config->upgradeRangeMultiplier[1] = 1.5;   // 2→3级：范围增加50%
    config->upgradeRangeMultiplier[2] = 2.0;   // 3→4级：范围增加100%

    // 升级攻击速度倍率
    config->upgradeSpeedMultiplier[0] = 1.2;   // 1→2级：攻速增加20%
    config->upgradeSpeedMultiplier[1] = 1.5;   // 2→3级：攻速增加50%
    config->upgradeSpeedMultiplier[2] = 2.0;   // 3→4级：攻速增加100%
}

// 生成下一个预览炮台
void generateNextTower() {
    // 随机选择炮台类型
    TowerType type = (TowerType)(rand() % MAX_TOWER_TYPES);

    game.nextTower.type = type;
    game.nextTower.cost = config.towerCosts[type];
    game.nextTower.color = config.towerColors[type];
    game.nextTower.damage = config.towerDamage[type];
    game.nextTower.range = config.towerRange[type];
    game.nextTower.attackSpeed = config.towerAttackSpeed[type];
}

// 计算升级成本
int calculateUpgradeCost(Tower* tower) {
    if (tower->level >= 4) return 0;
    return config.upgradeCosts[tower->level - 1];
}

// 计算出售价值
int calculateSellValue(Tower* tower) {
    // 出售返还总成本的一半
    int totalCost = tower->cost;

    // 加上升级成本
    for (int i = 1; i < tower->level; i++) {
        totalCost += config.upgradeCosts[i - 1];
    }

    return totalCost / 2;
}

// 放置炮台
void placeTower(int gridX, int gridY) {
    // 检查是否在路径上
    if (game.pathGrid[gridX][gridY] != 0) {
        return; // 不能放在路径上
    }

    // 检查是否已有炮台
    for (int i = 0; i < game.towerCount; i++) {
        if (game.towers[i].x == gridX && game.towers[i].y == gridY) {
            return; // 位置已有炮台
        }
    }

    // 检查金钱是否足够
    TowerType type = game.nextTower.type;
    int cost = game.nextTower.cost;

    if (game.money < cost) {
        return; // 金钱不足
    }

    // 创建新炮台
    if (game.towerCount < MAX_TOWERS) {
        Tower* t = &game.towers[game.towerCount];

        t->x = gridX;
        t->y = gridY;
        t->type = type;
        t->level = 1;
        t->damage = config.towerDamage[type];
        t->range = config.towerRange[type];
        t->attackSpeed = config.towerAttackSpeed[type];
        t->attackTimer = 0;
        t->cost = cost;
        t->upgradeCost = config.upgradeCosts[0]; // 升到2级的成本
        t->color = config.towerColors[type];

        game.towerCount++;
        game.money -= cost; // 扣除金钱

        // 生成下一个预览炮台
        generateNextTower();
    }
}

// 选择炮台
void selectTower(int gridX, int gridY) {
    game.selectedTower = NULL;

    for (int i = 0; i < game.towerCount; i++) {
        Tower* t = &game.towers[i];
        if (t->x == gridX && t->y == gridY) {
            game.selectedTower = t;

            // 激活升级和移除按钮
            game.buttons[BUTTON_UPGRADE].active = 1;
            game.buttons[BUTTON_REMOVE].active = 1;
            return;
        }
    }

    // 如果没有选中炮台，禁用升级和移除按钮
    game.buttons[BUTTON_UPGRADE].active = 0;
    game.buttons[BUTTON_REMOVE].active = 0;
}

// 升级选中的炮台
void upgradeSelectedTower() {
    if (game.selectedTower == NULL) return;

    Tower* t = game.selectedTower;

    // 检查是否达到最大等级
    if (t->level >= 4) return;

    // 检查金钱是否足够
    if (game.money < t->upgradeCost) return;

    // 升级炮台
    game.money -= t->upgradeCost;
    t->level++;

    // 应用升级效果
    int upgradeIndex = t->level - 2; // 0对应1→2级，1对应2→3级，2对应3→4级

    // 增加伤害
    t->damage = (int)(config.towerDamage[t->type] *
        config.upgradeDamageMultiplier[upgradeIndex]);

    // 增加范围
    t->range = (int)(config.towerRange[t->type] *
        config.upgradeRangeMultiplier[upgradeIndex]);

    // 增加攻击速度
    t->attackSpeed = config.towerAttackSpeed[t->type] *
        config.upgradeSpeedMultiplier[upgradeIndex];

    // 设置下一级升级成本
    if (t->level < 4) {
        t->upgradeCost = config.upgradeCosts[upgradeIndex + 1];
    }
    else {
        t->upgradeCost = 0; // 已满级
    }
}

// 移除选中的炮台
void removeSelectedTower() {
    if (game.selectedTower == NULL) return;

    Tower* t = game.selectedTower;

    // 返还一半成本
    game.money += calculateSellValue(t);

    // 找到炮台在数组中的索引
    int index = -1;
    for (int i = 0; i < game.towerCount; i++) {
        if (&game.towers[i] == t) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        // 将最后一个炮台移到这个位置
        game.towers[index] = game.towers[game.towerCount - 1];
        game.towerCount--;

        // 取消选择
        game.selectedTower = NULL;
        game.buttons[BUTTON_UPGRADE].active = 0;
        game.buttons[BUTTON_REMOVE].active = 0;
    }
}

// 更新所有炮台
void updateTowers(float deltaTime) {
    for (int i = 0; i < game.towerCount; i++) {
        Tower* t = &game.towers[i];

        // 更新攻击计时器
        t->attackTimer += deltaTime;

        if (t->attackTimer >= 1.0f / t->attackSpeed) {
            // 寻找攻击目标
            Enemy* target = NULL;
            float minDistance = t->range + 1; // 初始化为超出范围

            for (int j = 0; j < game.enemyCount; j++) {
                Enemy* e = &game.enemies[j];

                // 计算距离
                int dx = e->x * GRID_SIZE + GRID_SIZE / 2 -
                    (t->x * GRID_SIZE + GRID_SIZE / 2);
                int dy = e->y * GRID_SIZE + GRID_SIZE / 2 -
                    (t->y * GRID_SIZE + GRID_SIZE / 2);
                float distance = sqrt(dx * dx + dy * dy);

                if (distance <= t->range && distance < minDistance) {
                    target = e;
                    minDistance = distance;
                }
            }

            // 如果找到目标，发射子弹
            if (target != NULL) {
                createBullet(t, target);
                t->attackTimer = 0; // 重置计时器
            }
        }
    }
}

// 绘制所有炮台
void drawTowers() {
    for (int i = 0; i < game.towerCount; i++) {
        Tower* t = &game.towers[i];

        // 绘制炮台底座
        setfillcolor(t->color);
        fillrectangle(t->x * GRID_SIZE + 5,
            t->y * GRID_SIZE + 5,
            (t->x + 1) * GRID_SIZE - 5,
            (t->y + 1) * GRID_SIZE - 5);

        // 绘制炮台顶部（表示方向）
        setfillcolor(RGB(255, 255, 255));
        fillcircle(t->x * GRID_SIZE + GRID_SIZE / 2,
            t->y * GRID_SIZE + GRID_SIZE / 2,
            GRID_SIZE / 4);

        // 绘制等级标志
        settextcolor(RGB(255, 255, 255));
        setbkmode(TRANSPARENT);
        TCHAR levelText[10];
        _stprintf_s(levelText, _T("Lv.%d"), t->level);

        int textWidth = textwidth(levelText);
        int textHeight = textheight(levelText);
        outtextxy(t->x * GRID_SIZE + (GRID_SIZE - textWidth) / 2,
            t->y * GRID_SIZE + (GRID_SIZE - textHeight) / 2,
            levelText);

        // 如果被选中，绘制选中框
        if (&game.towers[i] == game.selectedTower) {
            setlinecolor(RGB(255, 255, 0));
            setlinestyle(PS_SOLID, 3);
            rectangle(t->x * GRID_SIZE,
                t->y * GRID_SIZE,
                (t->x + 1) * GRID_SIZE,
                (t->y + 1) * GRID_SIZE);
            setlinestyle(PS_SOLID, 1);
        }
    }
}

// 创建子弹
void createBullet(Tower* tower, Enemy* target) {
    if (game.bulletCount >= MAX_BULLETS) return;

    Bullet* b = &game.bullets[game.bulletCount];

    b->x = tower->x * GRID_SIZE + GRID_SIZE / 2;
    b->y = tower->y * GRID_SIZE + GRID_SIZE / 2;
    b->targetX = target->x * GRID_SIZE + GRID_SIZE / 2;
    b->targetY = target->y * GRID_SIZE + GRID_SIZE / 2;
    b->damage = tower->damage;
    b->speed = 300.0f; // 像素/秒

    // 找到目标敌人的索引
    for (int i = 0; i < game.enemyCount; i++) {
        if (&game.enemies[i] == target) {
            b->enemyIndex = i;
            break;
        }
    }

    // 根据炮台类型设置子弹颜色
    switch (tower->type) {
    case TOWER_BASIC: b->color = RGB(100, 100, 255); break;
    case TOWER_ARROW: b->color = RGB(100, 255, 100); break;
    case TOWER_CANNON: b->color = RGB(255, 100, 100); break;
    case TOWER_MAGIC: b->color = RGB(255, 100, 255); break;
    }

    game.bulletCount++;
}

// 更新子弹
void updateBullets(float deltaTime) {
    for (int i = 0; i < game.bulletCount; i++) {
        Bullet* b = &game.bullets[i];

        // 计算到目标的向量
        float dx = b->targetX - b->x;
        float dy = b->targetY - b->y;
        float distance = sqrt(dx * dx + dy * dy);

        // 如果子弹已到达目标位置
        if (distance < 5.0f) {
            // 造成伤害
            if (b->enemyIndex < game.enemyCount) {
                Enemy* e = &game.enemies[b->enemyIndex];
                e->health -= b->damage;

                // 检查敌人是否死亡
                if (e->health <= 0) {
                    // 奖励金钱和分数
                    game.money += e->moneyReward;
                    game.score += e->scoreReward;

                    // 移除敌人
                    game.enemies[b->enemyIndex] = game.enemies[game.enemyCount - 1];
                    game.enemyCount--;
                }
            }

            // 移除子弹
            game.bullets[i] = game.bullets[game.bulletCount - 1];
            game.bulletCount--;
            i--;
            continue;
        }

        // 移动子弹
        float moveDistance = b->speed * deltaTime;
        b->x += (int)(dx / distance * moveDistance);
        b->y += (int)(dy / distance * moveDistance);
    }
}

// 绘制子弹
void drawBullets() {
    for (int i = 0; i < game.bulletCount; i++) {
        Bullet* b = &game.bullets[i];

        setfillcolor(b->color);
        solidcircle(b->x, b->y, 5);
    }
}