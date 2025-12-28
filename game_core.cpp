#include "game_core.h"

// 全局变量定义
GameState game;
GameConfig config;
HighScore highScores[MAX_HIGH_SCORES];
int highScoreCount = 0;

// 初始化游戏配置函数
void initConfig() {
    // 炮台成本配置
    config.towerCosts[TOWER_BASIC] = 100;   // 基础炮塔成本
    config.towerCosts[TOWER_ARROW] = 150;   // 初级箭塔成本
    config.towerCosts[TOWER_CANNON] = 200;  // 加农炮成本
    config.towerCosts[TOWER_MAGIC] = 300;   // 魔法塔成本

    // 升级成本配置
    config.upgradeCosts[0] = 50;   // 1级升2级
    config.upgradeCosts[1] = 100;  // 2级升3级
    config.upgradeCosts[2] = 200;  // 3级升4级（保留）

    // 敌人得分配置
    config.enemyScores[ENEMY_BASIC] = 10;  // 基础敌人得分
    config.enemyScores[ENEMY_FAST] = 20;   // 快速敌人得分
    config.enemyScores[ENEMY_TANK] = 30;   // 坦克敌人得分

    // 基础配置
    config.baseLife = 100;   // 基地初始生命
    config.baseMoney = 500;  // 初始金钱
}

void initPathGrid() {
    // 先清空所有网格
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            game.pathGrid[x][y] = 0;
        }
    }

    // 标记所有路径点和路径线段（遍历所有路径）
    for (int pathId = 0; pathId < NUM_PATHS; pathId++) {
        for (int i = 0; i < game.pathCounts[pathId]; i++) {
            // 标记路径点
            int x = game.paths[pathId][i].x;
            int y = game.paths[pathId][i].y;
            game.pathGrid[x][y] = 1;

            // 标记路径线段
            if (i < game.pathCounts[pathId] - 1) {
                int x1 = game.paths[pathId][i].x;
                int y1 = game.paths[pathId][i].y;
                int x2 = game.paths[pathId][i + 1].x;
                int y2 = game.paths[pathId][i + 1].y;

                // 标记线段上的所有点
                if (x1 == x2) {  // 垂直线
                    int minY = (y1 < y2) ? y1 : y2;
                    int maxY = (y1 > y2) ? y1 : y2;
                    for (int y = minY; y <= maxY; y++) {
                        game.pathGrid[x1][y] = 1;
                    }
                }
                else if (y1 == y2) {  // 水平线
                    int minX = (x1 < x2) ? x1 : x2;
                    int maxX = (x1 > x2) ? x1 : x2;
                    for (int x = minX; x <= maxX; x++) {
                        game.pathGrid[x][y1] = 1;
                    }
                }
                // 如果需要斜线路径，可以添加斜线处理
            }
        }
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

// 初始化按钮
void initButtons() {
    int totalWidth = MAP_WIDTH * GRID_SIZE + UI_PANEL_WIDTH;
    int panelY = MAP_HEIGHT * GRID_SIZE + 10; // 按钮放在上部

    // 开始波次按钮
    game.buttons[BUTTON_START_WAVE].x = totalWidth - 180;
    game.buttons[BUTTON_START_WAVE].y = panelY;
    game.buttons[BUTTON_START_WAVE].width = 80;
    game.buttons[BUTTON_START_WAVE].height = 30;
    game.buttons[BUTTON_START_WAVE].text = _T("开始波次");
    game.buttons[BUTTON_START_WAVE].active = 1;
    game.buttons[BUTTON_START_WAVE].hovered = 0;
    game.buttons[BUTTON_START_WAVE].clicked = 0;

    // 暂停/继续按钮
    game.buttons[BUTTON_PAUSE].x = totalWidth - 90;
    game.buttons[BUTTON_PAUSE].y = panelY;
    game.buttons[BUTTON_PAUSE].width = 80;
    game.buttons[BUTTON_PAUSE].height = 30;
    game.buttons[BUTTON_PAUSE].text = _T("暂停");
    game.buttons[BUTTON_PAUSE].active = 1;
    game.buttons[BUTTON_PAUSE].hovered = 0;
    game.buttons[BUTTON_PAUSE].clicked = 0;

    // 升级按钮
    game.buttons[BUTTON_UPGRADE].x = totalWidth - 180;
    game.buttons[BUTTON_UPGRADE].y = panelY + 40;
    game.buttons[BUTTON_UPGRADE].width = 80;
    game.buttons[BUTTON_UPGRADE].height = 30;
    game.buttons[BUTTON_UPGRADE].text = _T("升级");
    game.buttons[BUTTON_UPGRADE].active = 0;
    game.buttons[BUTTON_UPGRADE].hovered = 0;
    game.buttons[BUTTON_UPGRADE].clicked = 0;

    // 移除按钮
    game.buttons[BUTTON_REMOVE].x = totalWidth - 90;
    game.buttons[BUTTON_REMOVE].y = panelY + 40;
    game.buttons[BUTTON_REMOVE].width = 80;
    game.buttons[BUTTON_REMOVE].height = 30;
    game.buttons[BUTTON_REMOVE].text = _T("移除");
    game.buttons[BUTTON_REMOVE].active = 0;
    game.buttons[BUTTON_REMOVE].hovered = 0;
    game.buttons[BUTTON_REMOVE].clicked = 0;

    // 退出按钮
    game.buttons[BUTTON_EXIT].x = totalWidth - 180;
    game.buttons[BUTTON_EXIT].y = panelY + 80;
    game.buttons[BUTTON_EXIT].width = 170;
    game.buttons[BUTTON_EXIT].height = 30;
    game.buttons[BUTTON_EXIT].text = _T("退出游戏");
    game.buttons[BUTTON_EXIT].active = 1;
    game.buttons[BUTTON_EXIT].hovered = 0;
    game.buttons[BUTTON_EXIT].clicked = 0;

    // 暂停界面的继续按钮（初始不激活，位置设为左上角）
    game.buttons[BUTTON_RESUME].x = 10;
    game.buttons[BUTTON_RESUME].y = 10;
    game.buttons[BUTTON_RESUME].width = 120;
    game.buttons[BUTTON_RESUME].height = 40;
    game.buttons[BUTTON_RESUME].text = _T("继续游戏");
    game.buttons[BUTTON_RESUME].active = 0;
    game.buttons[BUTTON_RESUME].hovered = 0;
    game.buttons[BUTTON_RESUME].clicked = 0;
}

// 从文件读取最高分记录
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
    }
}

// 保存最高分记录到文件
void saveHighScores() {
    FILE* file = fopen("highscores.dat", "wb");  // 以二进制写入方式打开文件
    if (file) {
        // 写入记录数量
        fwrite(&highScoreCount, sizeof(int), 1, file);
        // 写入记录数据
        fwrite(highScores, sizeof(HighScore), highScoreCount, file);
        fclose(file);
    }
}

// 更新最高分记录函数
void updateHighScores() {
    // 如果当前得分超过最低记录
    if (game.score > highScores[MAX_HIGH_SCORES - 1].score) {
        // 创建新记录
        HighScore newScore;
        strcpy(newScore.name, "Player");  // 默认玩家名
        newScore.score = game.score;      // 当前得分
        newScore.level = game.gameLevel;  // 当前等级

        // 插入排序：找到合适的位置插入新记录
        int i;
        for (i = MAX_HIGH_SCORES - 1; i >= 0; i--) {
            // 如果是第一个位置或得分低于前一个记录
            if (i == 0 || game.score <= highScores[i - 1].score) {
                highScores[i] = newScore;  // 插入新记录
                break;
            }
            else {
                highScores[i] = highScores[i - 1];  // 后移记录
            }
        }

        // 如果记录数未满，增加计数
        if (highScoreCount < MAX_HIGH_SCORES) {
            highScoreCount++;
        }

        // 保存到文件
        saveHighScores();
    }
}

// 初始化游戏函数：设置游戏初始状态
void initGame() {
    srand(time(NULL));

    // 初始化游戏状态
    game.money = config.baseMoney;
    game.life = config.baseLife;
    game.score = 0;
    game.highScore = (highScoreCount > 0) ? highScores[0].score : 0;
    game.wave = 1;
    game.enemiesInWave = 5;
    game.enemiesSpawned = 0;
    game.gameSpeed = 1.0;
    game.gameLevel = 1;
    game.gamePaused = 0;
    game.gameOver = 0;

    game.enemyCount = 0;
    game.towerCount = 0;
    game.bulletCount = 0;
    game.selectedTower = NULL;

    // 初始化波次状态
    game.waveStarted = 0;        // 初始未开始
    game.waveSpawning = 0;       // 未在生成
    game.waveDelayTimer = 0;     // 延迟计时器清零
    game.enemySpawnTimer = 0;    // 生成计时器清零

    // 初始化路径（改为多条路径）
    initPaths();

    // 初始化按钮
    initButtons();

    // 加载最高分
    loadHighScores();
}

// 检查点是否在按钮内
int isPointInButton(int x, int y, Button* btn) {
    return (x >= btn->x && x <= btn->x + btn->width &&
        y >= btn->y && y <= btn->y + btn->height);
}

// 处理按钮点击
void handleButtonClick(int x, int y) {
    for (int i = 0; i < 6; i++) {  // 现在有6个按钮
        if (game.buttons[i].active && isPointInButton(x, y, &game.buttons[i])) {
            switch (i) {
            case BUTTON_START_WAVE:
                if (!game.gamePaused && !game.gameOver && !game.waveStarted) {
                    game.waveStarted = 1;    // 标记波次开始
                    game.waveSpawning = 1;   // 开始生成敌人
                    game.enemySpawnTimer = 0;
                    game.waveDelayTimer = 0;
                }
                break;

            case BUTTON_PAUSE:
                if (!game.gameOver) {
                    game.gamePaused = !game.gamePaused;  // 切换暂停状态
                    if (game.gamePaused) {
                        game.buttons[BUTTON_PAUSE].text = _T("继续");
                    }
                    else {
                        game.buttons[BUTTON_PAUSE].text = _T("暂停");
                    }
                }
                break;

            case BUTTON_EXIT:
                closegraph();
                exit(0);
                break;

            case BUTTON_RESUME:  // 暂停界面的继续按钮
                if (game.gamePaused && !game.gameOver) {
                    game.gamePaused = 0;  // 继续游戏
                    game.buttons[BUTTON_PAUSE].text = _T("暂停");  // 更新右侧按钮文本
                }
                break;
            }
            break;
        }
    }
}

// 更新按钮悬停状态
void updateButtonHover(int x, int y) {
    for (int i = 0; i < 6; i++) {  // 现在有6个按钮
        if (game.buttons[i].active) {
            game.buttons[i].hovered = isPointInButton(x, y, &game.buttons[i]);
        }
        else {
            game.buttons[i].hovered = 0;
        }
    }
}

// 鼠标点击处理函数
void handleMouseClick(int x, int y, int button) {
    // 将像素坐标转换为网格坐标
    int gridX = x / GRID_SIZE;
    int gridY = y / GRID_SIZE;

    // 检查是否在地图范围内
    if (gridX >= 0 && gridX < MAP_WIDTH &&
        gridY >= 0 && gridY < MAP_HEIGHT) {

        if (button == 1) {  // 左键：放置炮台
            // 由game_defense模块处理
        }
        else if (button == 2) {  // 右键：选择炮台
            // 由game_defense模块处理
        }
    }
}

// 绘制按钮
void drawButton(Button* btn) {
    if (!btn->active) {
        // 禁用状态的按钮
        setfillcolor(RGB(100, 100, 100));
        setlinecolor(RGB(120, 120, 120));
    }
    else if (btn->hovered) {
        // 鼠标悬停状态的按钮
        setfillcolor(RGB(80, 120, 200));
        setlinecolor(RGB(100, 140, 220));
    }
    else {
        // 正常状态的按钮
        setfillcolor(RGB(60, 100, 180));
        setlinecolor(RGB(80, 120, 200));
    }

    solidrectangle(btn->x, btn->y, btn->x + btn->width, btn->y + btn->height);
    rectangle(btn->x, btn->y, btn->x + btn->width, btn->y + btn->height);

    // 绘制按钮文字
    settextcolor(RGB(255, 255, 255));
    setbkmode(TRANSPARENT);

    // 获取文本宽度和高度
    int textWidth = textwidth(btn->text);
    int textHeight = textheight(btn->text);

    // 居中显示文本
    outtextxy(btn->x + (btn->width - textWidth) / 2,
        btn->y + (btn->height - textHeight) / 2,
        btn->text);
}

// 绘制游戏画面，绘制三条路径
void drawGame() {
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

    // 绘制UI面板（游戏信息显示区域）
    int panelY = MAP_HEIGHT * GRID_SIZE;  // 面板Y坐标（地图下方）
    int panelHeight = UI_PANEL_HEIGHT;    // 面板高度（增加40像素）
    int totalWidth = MAP_WIDTH * GRID_SIZE + UI_PANEL_WIDTH; // 总宽度

    // 面板背景 - 扩展到总宽度
    setfillcolor(RGB(50, 50, 80));  // 深蓝色背景
    solidrectangle(0, panelY, totalWidth, panelY + panelHeight);

    // 游戏信息文本
    settextcolor(RGB(255, 255, 255));  // 白色文字
    TCHAR info[100];  // 字符串缓冲区

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

    // 预览区域：显示下一个防御单位
    int previewX = 250;
    // 预览区域背景
    setfillcolor(RGB(70, 70, 100));
    solidrectangle(previewX, panelY + 10, previewX + 120, panelY + 90);
    setlinecolor(RGB(255, 255, 255));
    rectangle(previewX, panelY + 10, previewX + 120, panelY + 90);

    // 预览区域标题
    settextcolor(RGB(255, 255, 255));
    outtextxy(previewX + 5, panelY + 15, _T("下一防御单位:"));

    // 绘制预览图标
    int centerX = previewX + 60;
    int centerY = panelY + 60;
    setfillcolor(game.nextTower.color);
    solidcircle(centerX, centerY, 20);

    // 显示类型和成本
    const TCHAR* typeNames[] = { _T("基础炮塔"), _T("初级箭塔"), _T("加农炮"), _T("魔法塔") };
    _stprintf_s(info, _T("%s: %d金"), typeNames[game.nextTower.type], game.nextTower.cost);
    outtextxy(previewX + 5, panelY + 75, info);

    // 控制说明区域 - 在下方
    settextcolor(RGB(200, 200, 255));  // 淡紫色文字
    outtextxy(400, panelY + 10, _T("控制说明:"));
    outtextxy(400, panelY + 30, _T("鼠标左键: 放置炮台"));
    outtextxy(400, panelY + 50, _T("鼠标右键: 选择炮台"));
    outtextxy(400, panelY + 70, _T("点击右侧按钮操作"));

    // 如果炮台被选中，显示详细信息 - 在右侧面板显示
    if (game.selectedTower != NULL) {
        Tower* t = game.selectedTower;
        int infoX = MAP_WIDTH * GRID_SIZE + 10;  // 在地图右侧显示

        // 选中信息区域背景
        setfillcolor(RGB(80, 80, 120));
        solidrectangle(infoX, panelY + 10, infoX + 180, panelY + 90);

        // 显示选中炮台信息
        _stprintf_s(info, _T("选中: %s Lv.%d"), typeNames[t->type], t->level);
        outtextxy(infoX + 10, panelY + 15, info);

        _stprintf_s(info, _T("伤害: %d  范围: %d格"), t->damage, t->range / GRID_SIZE);
        outtextxy(infoX + 10, panelY + 35, info);

        // 升级信息
        if (t->level < 3) {
            _stprintf_s(info, _T("升级到Lv.%d需要: %d金"), t->level + 1, t->upgradeCost);
            outtextxy(infoX + 10, panelY + 55, info);
        }
        else {
            outtextxy(infoX + 10, panelY + 55, _T("已达到最高等级"));
        }

        // 移除返还信息
        _stprintf_s(info, _T("移除返还: %d金"), t->cost / 2);
        outtextxy(infoX + 10, panelY + 75, info);
    }

    // 绘制右侧面板的5个按钮（不包括暂停界面的继续按钮）
    for (int i = 0; i < 5; i++) {
        drawButton(&game.buttons[i]);
    }

    // 游戏结束显示 - 扩展到总宽度
    if (game.gameOver) {
        // 半透明黑色覆盖层（使用深灰色模拟半透明）
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

    // 暂停显示 - 扩展到总宽度
    if (game.gamePaused && !game.gameOver) {
        // 半透明覆盖层（使用深灰色模拟半透明）
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

        // 绘制暂停界面的继续按钮（在画布左上角）
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
}