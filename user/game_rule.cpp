// game_rule.cpp - 修复后的版本
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
        if (game.towers[i].x == gridX && game.towers[i].y == gridY) {
            return 0;
        }
    }

    return 1;
}


// 更新最高分显示
void updateHighScoreDisplay() {
    if (game.score > game.highScore) {
        game.highScore = game.score;
    }
}

// 从文件读取最高分记录
void loadHighScores() {
    FILE* file = fopen("highscores.dat", "rb");
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
            game.highScore = highScores[0].score;
        }
    }
}

// 保存最高分记录到文件
void saveHighScore() {
    // 更新最高分记录
    updateHighScores();

    FILE* file = fopen("highscores.dat", "wb");
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
    // 如果当前得分超过最低记录或记录未满
    if (highScoreCount < MAX_HIGH_SCORES || game.score > highScores[MAX_HIGH_SCORES - 1].score) {
        // 创建新记录
        HighScore newScore;
        strcpy(newScore.name, "Player");
        newScore.score = game.score;
        newScore.level = game.gameLevel;

        // 插入排序
        int i;
        for (i = highScoreCount - 1; i >= 0; i--) {
            if (i == 0 || game.score <= highScores[i - 1].score) {
                if (i < MAX_HIGH_SCORES - 1) {
                    highScores[i] = newScore;
                }
                break;
            }
            else {
                if (i < MAX_HIGH_SCORES - 1) {
                    highScores[i] = highScores[i - 1];
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
    }
}


void initButtons() {
    int totalWidth = MAP_WIDTH * GRID_SIZE + UI_PANEL_WIDTH;
    int panelY = MAP_HEIGHT * GRID_SIZE + 10; // 这是UI面板内部的Y坐标

    // 开始波次按钮 - 修复坐标计算
    game.buttons[BUTTON_START_WAVE].x = totalWidth - 180;
    game.buttons[BUTTON_START_WAVE].y = panelY;  // 应该是在UI面板内部，不是地图下方
    game.buttons[BUTTON_START_WAVE].width = 80;
    game.buttons[BUTTON_START_WAVE].height = 30;
    _tcscpy_s(game.buttons[BUTTON_START_WAVE].text, 50, _T("start"));  // 使用_tcscpy_s更安全
    game.buttons[BUTTON_START_WAVE].active = 1;
    game.buttons[BUTTON_START_WAVE].hovered = 0;
    game.buttons[BUTTON_START_WAVE].clicked = 0;

    // 暂停/继续按钮
    game.buttons[BUTTON_PAUSE].x = totalWidth - 90;
    game.buttons[BUTTON_PAUSE].y = panelY;
    game.buttons[BUTTON_PAUSE].width = 80;
    game.buttons[BUTTON_PAUSE].height = 30;
    _tcscpy_s(game.buttons[BUTTON_PAUSE].text, 50, _T("暂停"));
    game.buttons[BUTTON_PAUSE].active = 1;
    game.buttons[BUTTON_PAUSE].hovered = 0;
    game.buttons[BUTTON_PAUSE].clicked = 0;

    // 升级按钮
    game.buttons[BUTTON_UPGRADE].x = totalWidth - 180;
    game.buttons[BUTTON_UPGRADE].y = panelY + 40;
    game.buttons[BUTTON_UPGRADE].width = 80;
    game.buttons[BUTTON_UPGRADE].height = 30;
    _tcscpy_s(game.buttons[BUTTON_UPGRADE].text, 50, _T("升级"));
    game.buttons[BUTTON_UPGRADE].active = 0;
    game.buttons[BUTTON_UPGRADE].hovered = 0;
    game.buttons[BUTTON_UPGRADE].clicked = 0;

    // 移除按钮
    game.buttons[BUTTON_REMOVE].x = totalWidth - 90;
    game.buttons[BUTTON_REMOVE].y = panelY + 40;
    game.buttons[BUTTON_REMOVE].width = 80;
    game.buttons[BUTTON_REMOVE].height = 30;
    _tcscpy_s(game.buttons[BUTTON_REMOVE].text, 50, _T("移除"));
    game.buttons[BUTTON_REMOVE].active = 0;
    game.buttons[BUTTON_REMOVE].hovered = 0;
    game.buttons[BUTTON_REMOVE].clicked = 0;

    // 退出按钮
    game.buttons[BUTTON_EXIT].x = totalWidth - 180;
    game.buttons[BUTTON_EXIT].y = panelY + 80;
    game.buttons[BUTTON_EXIT].width = 170;
    game.buttons[BUTTON_EXIT].height = 30;
    _tcscpy_s(game.buttons[BUTTON_EXIT].text, 50, _T("exit"));
    game.buttons[BUTTON_EXIT].active = 1;
    game.buttons[BUTTON_EXIT].hovered = 0;
    game.buttons[BUTTON_EXIT].clicked = 0;

    // 暂停界面的继续按钮（初始不激活）
    game.buttons[BUTTON_RESUME].x = 10;
    game.buttons[BUTTON_RESUME].y = 10;
    game.buttons[BUTTON_RESUME].width = 120;
    game.buttons[BUTTON_RESUME].height = 40;
    _tcscpy_s(game.buttons[BUTTON_RESUME].text, 50, _T("继续游戏"));
    game.buttons[BUTTON_RESUME].active = 0;
    game.buttons[BUTTON_RESUME].hovered = 0;
    game.buttons[BUTTON_RESUME].clicked = 0;
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