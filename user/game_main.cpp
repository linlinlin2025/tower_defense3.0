// main.cpp - 游戏主循环
#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <conio.h>
#include "game_core.h"
#include "game_defense.h"
#include "game_enemy.h"
#include "game_rule.h"

int main() {
    // 初始化图形窗口
    initgraph(MAP_WIDTH * GRID_SIZE + UI_PANEL_WIDTH,
        MAP_HEIGHT * GRID_SIZE + UI_PANEL_HEIGHT);

    // 初始化游戏
    initConfig();
    initGame();

    // 游戏主循环
    while (true) {
        // 处理鼠标输入
        ExMessage msg;
        while (peekmessage(&msg, EX_MOUSE)) {
            if (msg.message == WM_MOUSEMOVE) {
                updateButtonHover(msg.x, msg.y);
            }
            else if (msg.message == WM_LBUTTONDOWN) {
                handleMouseClick(msg.x, msg.y, 1);  // 左键
                handleButtonClick(msg.x, msg.y);    // 按钮点击
            }
            else if (msg.message == WM_RBUTTONDOWN) {
                handleMouseClick(msg.x, msg.y, 2);  // 右键
            }
        }

        // 处理键盘输入
        if (_kbhit()) {
            int key = _getch();
            if (key == ' ' || key == 'p' || key == 'P') {
                // 空格或P键暂停游戏
                game.gamePaused = !game.gamePaused;
                if (game.gamePaused) {
                    _tcscpy_s(game.buttons[BUTTON_PAUSE].text, 50, _T("继续"));
                }
                else {
                    _tcscpy_s(game.buttons[BUTTON_PAUSE].text, 50, _T("暂停"));
                }
            }
            else if (key == 27) {  // ESC键退出
                closegraph();
                return 0;
            }
        }

        // 计算时间增量
        static DWORD lastTime = GetTickCount();
        DWORD currentTime = GetTickCount();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // 更新游戏逻辑
        updateGame(deltaTime);

        // 绘制游戏画面
        drawGame();

        // 控制帧率
        Sleep(16);  // 约60FPS
    }

    closegraph();
    return 0;
}
