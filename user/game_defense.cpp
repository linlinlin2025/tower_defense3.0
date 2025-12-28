#include "game_defense.h"
#include "game_rule.h"

// 放置炮台函数
void placeTower(int gridX, int gridY) {
    // 检查炮台数量限制
    if (game.towerCount >= MAX_TOWERS) return;
    // 检查是否可建造
    if (!canBuildAt(gridX, gridY)) return;

    // 检查金钱是否足够
    if (game.money < game.nextTower.cost) return;

    // 扣钱
    game.money -= game.nextTower.cost;

    // 创建新炮台（从预览炮台复制属性）
    Tower* t = &game.towers[game.towerCount];
    *t = game.nextTower;  // 复制所有属性

    // 设置位置相关属性
    t->gridX = gridX;
    t->gridY = gridY;
    t->x = gridX * GRID_SIZE + GRID_SIZE / 2;  // 计算像素坐标（网格中心）
    t->y = gridY * GRID_SIZE + GRID_SIZE / 2;
    t->selected = 0;      // 未选中状态
    t->canBuild = 1;      // 可建造状态
    t->attackTimer = 0;   // 攻击计时器清零

    // 增加炮台计数
    game.towerCount++;

    // 生成新的预览防御单位
    generateNextTower();
}

// 升级选中的炮台
void upgradeSelectedTower() {
    // 检查是否有选中的炮台
    if (game.selectedTower == NULL) return;

    Tower* t = game.selectedTower;

    // 检查是否已达到最高等级
    if (t->level >= 3) return;

    // 获取升级成本
    int cost = config.upgradeCosts[t->level - 1];
    // 检查金钱是否足够
    if (game.money < cost) return;

    // 扣钱
    game.money -= cost;

    // 升级属性
    t->level++;                                // 等级提升
    t->damage = (int)(t->damage * 1.5);        // 伤害增加50%
    t->range = (int)(t->range * 1.2);          // 范围增加20%
    t->attackSpeed = t->attackSpeed * 0.9;     // 攻击间隔减少10%（攻击速度加快）

    // 设置下一级升级成本（如果不是最高级）
    if (t->level < 3) {
        t->upgradeCost = config.upgradeCosts[t->level - 1];
    }

    // 颜色变亮表示升级
    int r = GetRValue(t->color) + 30;
    int g = GetGValue(t->color) + 30;
    int b = GetBValue(t->color) + 30;
    // 限制颜色值在0-255范围内
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    t->color = RGB(r, g, b);
}

// 移除选中的炮台
void removeSelectedTower() {
    // 检查是否有选中的炮台
    if (game.selectedTower == NULL) return;

    Tower* t = game.selectedTower;

    // 返还部分金钱（50%）
    int refund = t->cost / 2;
    game.money += refund;

    // 找到炮台在数组中的索引位置
    int index = -1;
    for (int i = 0; i < game.towerCount; i++) {
        if (&game.towers[i] == t) {
            index = i;
            break;
        }
    }

    // 如果找到，移除该炮台
    if (index != -1) {
        // 将后面的炮台向前移动，覆盖要移除的炮台
        for (int i = index; i < game.towerCount - 1; i++) {
            game.towers[i] = game.towers[i + 1];
        }
        // 减少炮台计数
        game.towerCount--;
    }

    // 清除选中状态
    game.selectedTower = NULL;

    // 禁用升级和移除按钮
    game.buttons[BUTTON_UPGRADE].active = 0;
    game.buttons[BUTTON_REMOVE].active = 0;
}

// 选择炮台（通过鼠标点击选择）
void selectTowerAt(int gridX, int gridY) {
    // 取消之前选择的炮台
    if (game.selectedTower != NULL) {
        game.selectedTower->selected = 0;  // 清除选中状态
        game.selectedTower = NULL;         // 清空选中指针
    }

    // 遍历所有炮台，查找点击位置的炮台
    for (int i = 0; i < game.towerCount; i++) {
        Tower* t = &game.towers[i];
        // 如果点击位置有炮台
        if (t->gridX == gridX && t->gridY == gridY) {
            t->selected = 1;          // 设置为选中状态
            game.selectedTower = t;   // 设置全局选中指针

            // 启用升级和移除按钮
            game.buttons[BUTTON_UPGRADE].active = 1;
            game.buttons[BUTTON_REMOVE].active = 1;
            break;
        }
    }
}

// 炮台寻找目标并攻击
void updateTowers() {
    // 遍历所有炮台
    for (int i = 0; i < game.towerCount; i++) {
        Tower* t = &game.towers[i];

        // 更新攻击计时器（减少计时器值）
        t->attackTimer -= 0.1 * game.gameSpeed;
        // 如果计时器大于0，还不能攻击
        if (t->attackTimer > 0) continue;

        // 寻找最近的敌人
        int targetIdx = -1;            // 目标敌人索引
        float minDist = t->range + 1;  // 最小距离初始化为大于攻击范围的值

        // 遍历所有敌人
        for (int j = 0; j < game.enemyCount; j++) {
            Enemy* e = &game.enemies[j];
            if (!e->isAlive) continue;  // 跳过死亡的敌人

            // 计算炮台到敌人的距离
            float dx = e->x - t->x;
            float dy = e->y - t->y;
            float dist = sqrt(dx * dx + dy * dy);

            // 如果在攻击范围内且距离更近
            if (dist < t->range && dist < minDist) {
                minDist = dist;   // 更新最小距离
                targetIdx = j;    // 记录敌人索引
            }
        }

        // 如果找到目标且子弹数量未达上限
        if (targetIdx != -1 && game.bulletCount < MAX_BULLETS) {
            Bullet* b = &game.bullets[game.bulletCount];

            // 设置子弹属性
            b->x = t->x;                      // 起始位置（炮台位置）
            b->y = t->y;
            b->targetX = game.enemies[targetIdx].x;  // 目标位置（敌人当前位置）
            b->targetY = game.enemies[targetIdx].y;
            b->speed = 5.0 * game.gameSpeed;  // 子弹速度
            b->damage = t->damage;            // 子弹伤害
            b->fromTower = i;                 // 发射炮台索引
            b->targetEnemy = targetIdx;       // 目标敌人索引
            b->isActive = 1;                  // 激活子弹
            b->color = t->color;              // 子弹颜色与炮台相同

            // 更新计数和计时器
            game.bulletCount++;               // 子弹计数增加
            t->attackTimer = t->attackSpeed;  // 重置攻击计时器
        }
    }
}

// 更新子弹
void updateBullets() {
    // 遍历所有子弹
    for (int i = 0; i < game.bulletCount; i++) {
        Bullet* b = &game.bullets[i];
        // 跳过非活跃的子弹
        if (!b->isActive) continue;

        // 计算子弹到目标的向量
        float dx = b->targetX - b->x;
        float dy = b->targetY - b->y;
        float dist = sqrt(dx * dx + dy * dy);  // 计算距离

        // 如果子弹到达目标位置（距离小于速度值）
        if (dist < b->speed) {
            // 击中目标
            b->isActive = 0;  // 标记子弹为非活跃

            // 检查目标敌人是否有效
            if (b->targetEnemy < game.enemyCount) {
                Enemy* e = &game.enemies[b->targetEnemy];
                e->life -= b->damage;  // 敌人受到伤害

                // 如果敌人死亡
                if (e->life <= 0) {
                    e->isAlive = 0;  // 标记死亡
                    // 奖励金钱
                    game.money += e->money;
                    // 增加得分（得分受游戏等级影响）
                    game.score += config.enemyScores[e->type] * game.gameLevel;

                    // 每1000分提升一级
                    int newLevel = game.score / 1000 + 1;
                    if (newLevel > game.gameLevel) {
                        game.gameLevel = newLevel;
                        // 每级游戏速度增加20%
                        game.gameSpeed = 1.0 + (game.gameLevel - 1) * 0.2;
                    }
                }
            }
        }
        else {
            // 子弹向目标移动
            b->x += (dx / dist) * b->speed;  // 归一化方向向量乘以速度
            b->y += (dy / dist) * b->speed;
        }
    }

    // 清理无效的子弹（从数组中移除）
    for (int i = 0; i < game.bulletCount; i++) {
        if (!game.bullets[i].isActive) {
            // 将后面的子弹向前移动，覆盖无效的子弹
            for (int j = i; j < game.bulletCount - 1; j++) {
                game.bullets[j] = game.bullets[j + 1];
            }
            game.bulletCount--;  // 子弹计数减少
            i--;  // 重新检查当前位置
        }
    }
}

// 绘制炮台
void drawTowers() {
    for (int i = 0; i < game.towerCount; i++) {
        Tower* t = &game.towers[i];

        // 绘制炮台底座（灰色矩形）
        setfillcolor(RGB(100, 100, 100));
        solidrectangle(t->gridX * GRID_SIZE, t->gridY * GRID_SIZE,
            (t->gridX + 1) * GRID_SIZE, (t->gridY + 1) * GRID_SIZE);

        // 绘制炮台主体（圆形）
        setfillcolor(t->color);
        solidcircle(t->x, t->y, GRID_SIZE / 3);

        // 绘制等级标志（白色数字）
        TCHAR levelStr[10];
        _stprintf_s(levelStr, _T("%d"), t->level);  // 将等级转换为字符串
        settextcolor(RGB(255, 255, 255));
        setbkmode(TRANSPARENT);  // 透明背景
        outtextxy(t->x - 5, t->y - 8, levelStr);  // 居中显示等级

        // 如果炮台被选中，绘制攻击范围和高亮边框
        if (t->selected) {
            setlinecolor(RGB(255, 255, 0));  // 黄色
            circle(t->x, t->y, t->range);    // 绘制攻击范围圆
            circle(t->x, t->y, GRID_SIZE / 3 + 2);  // 绘制高亮边框
        }
    }
}

// 绘制子弹
void drawBullets() {
    for (int i = 0; i < game.bulletCount; i++) {
        Bullet* b = &game.bullets[i];

        setfillcolor(b->color);
        solidcircle(b->x, b->y, 4);  // 绘制小圆点表示子弹
    }
}