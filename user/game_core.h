// game_core.h
#ifndef GAME_CORE_H
#define GAME_CORE_H

#include <windows.h>
#include <stdio.h>
#include <time.h>

// 定义常量
#define MAP_WIDTH 20
#define MAP_HEIGHT 15
#define GRID_SIZE 40
#define UI_PANEL_WIDTH 200
#define UI_PANEL_HEIGHT 120
#define NUM_PATHS 3
#define MAX_PATH_POINTS 20
#define MAX_TOWERS 50
#define MAX_ENEMIES 100
#define MAX_BULLETS 200

// 塔类型枚举
enum TowerType {
    TOWER_BASIC,   // 基础炮塔
    TOWER_ARROW,   // 初级箭塔
    TOWER_CANNON,  // 加农炮
    TOWER_MAGIC,   // 魔法塔
    MAX_TOWER_TYPES
};

// 敌人类型枚举
enum EnemyType {
    ENEMY_BASIC,   // 基础敌人
    ENEMY_FAST,    // 快速敌人
    ENEMY_TANK,    // 坦克敌人
    MAX_ENEMY_TYPES
};

// 按钮枚举
enum ButtonType {
    BUTTON_START_WAVE,
    BUTTON_PAUSE,
    BUTTON_UPGRADE,
    BUTTON_REMOVE,
    BUTTON_EXIT,
    BUTTON_RESUME,
    MAX_BUTTONS
};

// 坐标结构
struct Point {
    int x, y;
};

// 炮塔结构
struct Tower {
    int x, y;           // 位置
    TowerType type;     // 类型
    int level;          // 等级
    int damage;         // 伤害
    int range;          // 攻击范围
    float attackSpeed;  // 攻击速度
    float attackTimer;  // 攻击计时器
    int cost;           // 成本
    int upgradeCost;    // 升级成本
    COLORREF color;     // 颜色
};

// 敌人结构
struct Enemy {
    int x, y;           // 位置
    EnemyType type;     // 类型
    int health;         // 生命值
    int maxHealth;      // 最大生命值
    float speed;        // 移动速度
    int pathId;         // 路径ID
    int waypointIndex;  // 当前路径点索引
    int moneyReward;    // 金钱奖励
    int scoreReward;    // 得分奖励
    int baseDamage;     // 对基地的伤害
};

// 子弹结构
struct Bullet {
    int x, y;           // 位置
    int targetX, targetY; // 目标位置
    int damage;         // 伤害
    float speed;        // 速度
    int enemyIndex;     // 目标敌人索引
    COLORREF color;     // 颜色
};

// 按钮结构
struct Button {
    int x, y;
    int width, height;
    TCHAR text[50];
    int active;
    int hovered;
    int clicked;
};

// 游戏配置结构体 - 扩展版
struct GameConfig {
    // 基础配置
    int baseLife;
    int baseMoney;
    int baseScore;

    // 炮台配置（全部放在防御模块）
    int towerCosts[MAX_TOWER_TYPES];
    int towerDamage[MAX_TOWER_TYPES];
    int towerRange[MAX_TOWER_TYPES];
    float towerAttackSpeed[MAX_TOWER_TYPES];
    COLORREF towerColors[MAX_TOWER_TYPES];

    // 升级配置（放在防御模块）
    int upgradeCosts[3];  // 1-2, 2-3, 3-4级升级成本
    float upgradeDamageMultiplier[3];
    float upgradeRangeMultiplier[3];
    float upgradeSpeedMultiplier[3];

    // 敌人配置（全部放在敌人模块）
    int enemyScores[MAX_ENEMY_TYPES];
    int enemyHealth[MAX_ENEMY_TYPES];
    float enemySpeed[MAX_ENEMY_TYPES];
    int enemyGoldReward[MAX_ENEMY_TYPES];
    int enemyDamage[MAX_ENEMY_TYPES];
    COLORREF enemyColors[MAX_ENEMY_TYPES];

    // 波次配置
    int waveEnemyIncrease;
    float waveHealthMultiplier;
    float waveSpeedMultiplier;
    int initialEnemiesInWave;
};

// 游戏状态结构体
struct GameState {
    // 核心状态
    int money;
    int life;
    int score;
    int highScore;

    // 波次状态
    int wave;
    int enemiesInWave;
    int enemiesSpawned;
    int waveStarted;
    int waveSpawning;
    int waveDelayTimer;
    int enemySpawnTimer;

    // 游戏控制
    float gameSpeed;
    int gameLevel;
    int gamePaused;
    int gameOver;

    // 对象计数
    int towerCount;
    int enemyCount;
    int bulletCount;

    // 对象数组
    Tower towers[MAX_TOWERS];
    Enemy enemies[MAX_ENEMIES];
    Bullet bullets[MAX_BULLETS];

    // 路径系统
    Point paths[NUM_PATHS][MAX_PATH_POINTS];
    int pathCounts[NUM_PATHS];
    int pathGrid[MAP_WIDTH][MAP_HEIGHT];

    // 选择与预览
    Tower* selectedTower;
    Tower nextTower;  // 预览炮台

    // UI系统
    Button buttons[MAX_BUTTONS];
};

// 全局变量声明
extern GameState game;
extern GameConfig config;

// 函数声明
void initConfig();
void initGame();
void initPathGrid();

void handleMouseClick(int x, int y, int button);
void handleButtonClick(int x, int y);
void updateButtonHover(int x, int y);
void drawGame();

int isPointInButton(int x, int y, Button* btn);

void updateGame(float deltaTime);

// 分数系统函数（由game_rule模块提供）
void loadHighScores();
void saveHighScore();

#endif // GAME_CORE_H
