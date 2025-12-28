#include "game_core.h"
#include "game_rule.h"
#include "game_enemy.h"
#include "game_defense.h"

// 绘制所有游戏元素（整合各模块的绘制函数）
void drawAllGameElements() {
    cleardevice();  // 清空屏幕

    // 绘制背景
    setfillcolor(RGB(40, 40, 40));  // 深灰色背景
    solidrectangle(0, 0, MAP_WIDTH * GRID_SIZE, MAP_HEIGHT * GRID_SIZE);

    // 绘制网格线
    setlinecolor(RGB(80, 80, 80));  // 浅灰色网格线
    // 绘制垂直线
    for (int i = 0; i <= MAP_WIDTH; i++) {
        line(i * GRID_SIZE, 0, i * GRID_SIZE, MAP_HEIGHT * GRID_SIZE);
    }
    // 绘制水平线
    for (int i = 0; i <= MAP_HEIGHT; i++) {
        line(0, i * GRID_SIZE, MAP_WIDTH * GRID_SIZE, i * GRID_SIZE);
    }

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

    // 绘制敌人
    for (int i = 0; i < game.enemyCount; i++) {
        Enemy* e = &game.enemies[i];

        // 绘制敌人身体（圆形）
        setfillcolor(e->color);
        solidcircle(e->x, e->y, GRID_SIZE / 3);

        // 绘制白色边框
        setlinecolor(RGB(255, 255, 255));
        circle(e->x, e->y, GRID_SIZE / 3);

        // 绘制生命条（红色）
        float lifeRatio = (float)e->life / e->maxLife;  // 计算生命比例
        setfillcolor(RGB(255, 0, 0));
        fillrectangle(e->x - GRID_SIZE / 2, e->y - GRID_SIZE / 2 - 5,
            e->x - GRID_SIZE / 2 + GRID_SIZE * lifeRatio,
            e->y - GRID_SIZE / 2 - 2);
    }

    // 绘制炮台
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

    // 绘制子弹
    for (int i = 0; i < game.bulletCount; i++) {
        Bullet* b = &game.bullets[i];

        setfillcolor(b->color);
        solidcircle(b->x, b->y, 4);  // 绘制小圆点表示子弹
    }
}

// 主函数：程序入口点
int main() {
    // 初始化图形窗口 - 增加宽度和高度以容纳UI
    int totalWidth = MAP_WIDTH * GRID_SIZE + UI_PANEL_WIDTH;
    int totalHeight = MAP_HEIGHT * GRID_SIZE + UI_PANEL_HEIGHT;

    initgraph(totalWidth, totalHeight);
    BeginBatchDraw();  // 开启批量绘制（双缓冲模式）
    setbkcolor(RGB(30, 30, 30));  // 设置背景色

    // 初始化游戏配置
    initConfig();

    // 初始化游戏状态
    initGame();

    // 设置随机种子
    srand(time(NULL));

    // 消息变量（用于接收鼠标和键盘事件）
    ExMessage msg;

    // 游戏主循环
    while (1) {
        // 处理输入事件（鼠标和键盘）
        while (peekmessage(&msg, EX_MOUSE | EX_KEY)) {
            if (msg.message == WM_MOUSEMOVE) {
                // 更新按钮悬停状态（包括暂停界面的继续按钮）
                updateButtonHover(msg.x, msg.y);
            }
            else if (msg.message == WM_LBUTTONDOWN) {
                // 鼠标左键按下
                if (!game.gamePaused && !game.gameOver) {
                    // 先检查是否点击了按钮
                    handleButtonClick(msg.x, msg.y);

                    // 如果不是按钮，处理地图点击
                    if (msg.x < MAP_WIDTH * GRID_SIZE) {
                        // 将像素坐标转换为网格坐标
                        int gridX = msg.x / GRID_SIZE;
                        int gridY = msg.y / GRID_SIZE;

                        // 检查是否在地图范围内
                        if (gridX >= 0 && gridX < MAP_WIDTH &&
                            gridY >= 0 && gridY < MAP_HEIGHT) {
                            placeTower(gridX, gridY);
                        }
                    }
                }
                else if (game.gameOver || game.gamePaused) {
                    // 游戏结束或暂停时，只响应按钮点击
                    handleButtonClick(msg.x, msg.y);
                }
            }
            else if (msg.message == WM_RBUTTONDOWN) {
                // 鼠标右键按下（选择炮台）
                if (!game.gamePaused && !game.gameOver) {
                    if (msg.x < MAP_WIDTH * GRID_SIZE) {
                        // 将像素坐标转换为网格坐标
                        int gridX = msg.x / GRID_SIZE;
                        int gridY = msg.y / GRID_SIZE;

                        // 检查是否在地图范围内
                        if (gridX >= 0 && gridX < MAP_WIDTH &&
                            gridY >= 0 && gridY < MAP_HEIGHT) {
                            selectTowerAt(gridX, gridY);
                        }
                    }
                }
            }
            // 处理升级和移除按钮点击
            else if (msg.message == WM_LBUTTONDOWN) {
                // 检查是否点击了升级或移除按钮
                for (int i = 0; i < 6; i++) {
                    if (game.buttons[i].active && isPointInButton(msg.x, msg.y, &game.buttons[i])) {
                        if (i == BUTTON_UPGRADE && !game.gamePaused && !game.gameOver) {
                            upgradeSelectedTower();
                        }
                        else if (i == BUTTON_REMOVE && !game.gamePaused && !game.gameOver) {
                            removeSelectedTower();
                        }
                    }
                }
            }
            // 保留ESC键退出功能
            else if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                closegraph();
                return 0;
            }
        }

        // 如果游戏没有暂停且没有结束，更新游戏状态
        if (!game.gamePaused && !game.gameOver) {
            // 更新波次管理（包括敌人生成）
            updateWave();  // 调用波次管理函数

            // 生成敌人（在updateWave中调用）
            if (game.waveStarted && game.waveSpawning &&
                game.enemySpawnTimer >= 60 / game.gameSpeed &&
                game.enemiesSpawned < game.enemiesInWave) {
                spawnEnemy();
                game.enemySpawnTimer = 0;
            }

            // 更新敌人移动
            updateEnemies();

            // 更新炮台攻击
            updateTowers();

            // 更新子弹
            updateBullets();

            // 更新最高分显示
            updateHighScoreDisplay();
        }

        // 绘制游戏画面
        drawAllGameElements();

        // 绘制UI面板
        int panelY = MAP_HEIGHT * GRID_SIZE;
        int panelHeight = UI_PANEL_HEIGHT;
        int totalWidth = MAP_WIDTH * GRID_SIZE + UI_PANEL_WIDTH;

        // 面板背景 - 扩展到总宽度
        setfillcolor(RGB(50, 50, 80));
        solidrectangle(0, panelY, totalWidth, panelY + panelHeight);

        // 游戏信息文本
        settextcolor(RGB(255, 255, 255));
        TCHAR info[100];

        // 显示金钱、生命、得分
        _stprintf_s(info, _T("金钱: %d  生命: %d  得分: %d"), game.money, game.life, game.score);
        outtextxy(10, panelY + 10, info);

        // 显示波次、等级、游戏速度
        _stprintf_s(info, _T("波次: %d (%d/%d)  等级: %d  速度: %.1fx"),
            game.wave, game.enemiesSpawned, game.enemiesInWave,
            game.gameLevel, game.gameSpeed);
        outtextxy(10, panelY + 30, info);

        // 显示最高分
        _stprintf_s(info, _T("最高分: %d"), game.highScore);
        outtextxy(10, panelY + 50, info);

        // 显示路径信息
        _stprintf_s(info, _T("路径数量: %d"), NUM_PATHS);
        outtextxy(10, panelY + 70, info);

        // 绘制右侧面板的5个按钮
        for (int i = 0; i < 5; i++) {
            drawButton(&game.buttons[i]);
        }

        // 游戏结束显示
        if (game.gameOver) {
            // 半透明黑色覆盖层
            setfillcolor(RGB(0, 0, 0));
            solidrectangle(0, 0, totalWidth, MAP_HEIGHT * GRID_SIZE + panelHeight);

            // 再绘制一个稍浅的矩形显示内容
            setfillcolor(RGB(50, 50, 50));
            solidrectangle(totalWidth / 2 - 150, MAP_HEIGHT * GRID_SIZE / 2 - 80,
                totalWidth / 2 + 150, MAP_HEIGHT * GRID_SIZE / 2 + 80);

            // 游戏结束大字
            settextcolor(RGB(255, 0, 0));
            settextstyle(48, 0, _T("黑体"));
            outtextxy(totalWidth / 2 - 100,
                MAP_HEIGHT * GRID_SIZE / 2 - 50, _T("游戏结束"));

            // 最终成绩
            settextstyle(16, 0, _T("宋体"));
            settextcolor(RGB(255, 255, 255));
            _stprintf_s(info, _T("最终得分: %d  等级: %d"), game.score, game.gameLevel);
            outtextxy(totalWidth / 2 - 80,
                MAP_HEIGHT * GRID_SIZE / 2 + 10, info);

            // 退出提示
            outtextxy(totalWidth / 2 - 100,
                MAP_HEIGHT * GRID_SIZE / 2 + 40, _T("点击退出按钮退出"));
        }

        // 暂停显示
        if (game.gamePaused && !game.gameOver) {
            // 半透明覆盖层
            setfillcolor(RGB(0, 0, 0));
            solidrectangle(0, 0, totalWidth, MAP_HEIGHT * GRID_SIZE + panelHeight);

            // 再绘制一个稍浅的矩形显示内容
            setfillcolor(RGB(50, 50, 50));
            solidrectangle(totalWidth / 2 - 150, MAP_HEIGHT * GRID_SIZE / 2 - 80,
                totalWidth / 2 + 150, MAP_HEIGHT * GRID_SIZE / 2 + 80);

            // 暂停提示
            settextcolor(RGB(255, 255, 0));
            settextstyle(36, 0, _T("黑体"));
            outtextxy(totalWidth / 2 - 70,
                MAP_HEIGHT * GRID_SIZE / 2 - 30, _T("游戏暂停"));

            // 继续提示
            settextstyle(16, 0, _T("宋体"));
            settextcolor(RGB(200, 200, 255));
            outtextxy(totalWidth / 2 - 100,
                MAP_HEIGHT * GRID_SIZE / 2 + 20, _T("点击左上角继续按钮继续游戏"));

            // 绘制暂停界面的继续按钮
            game.buttons[BUTTON_RESUME].x = 10;
            game.buttons[BUTTON_RESUME].y = 10;
            game.buttons[BUTTON_RESUME].width = 120;
            game.buttons[BUTTON_RESUME].height = 40;
            game.buttons[BUTTON_RESUME].active = 1;
            game.buttons[BUTTON_RESUME].text = _T("继续游戏");
            drawButton(&game.buttons[BUTTON_RESUME]);
        }
        else {
            // 游戏没有暂停时，暂停界面的继续按钮不激活
            game.buttons[BUTTON_RESUME].active = 0;
        }

        FlushBatchDraw();
        // 控制帧率（约60FPS）
        Sleep(16);  // 休眠16毫秒
    }
    EndBatchDraw();
    // 关闭图形窗口（实际不会执行到这里，因为程序在ESC键退出）
    closegraph();
    return 0;
}
