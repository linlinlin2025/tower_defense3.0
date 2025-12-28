#ifndef GAME_CORE_H
#define GAME_CORE_H

#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// 游戏常量定义
#define GRID_SIZE 40        // 网格大小（像素）
#define MAP_WIDTH 15        // 地图宽度（网格数）
#define MAP_HEIGHT 10       // 地图高度（网格数）
#define MAX_ENEMIES 50      // 最大敌人数
#define MAX_TOWERS 30       // 最大炮台数
#define MAX_BULLETS 100     // 最大子弹数
#define MAX_HIGH_SCORES 5   // 最高分记录数量
#define UI_PANEL_WIDTH 200  // UI面板额外宽度
#define UI_PANEL_HEIGHT 160 // UI面板高度（增加40像素）
#define NUM_PATHS 3         // 路径数量

// 按钮常量定义
#define BUTTON_START_WAVE 0
#define BUTTON_PAUSE 1
#define BUTTON_UPGRADE 2
#define BUTTON_REMOVE 3
#define BUTTON_EXIT 4
#define BUTTON_RESUME 5     // 新增：暂停界面的继续按钮

// 敌人类型枚举
typedef enum {
    ENEMY_BASIC = 0,  // 基础敌人：平衡型
    ENEMY_FAST,       // 快速敌人：速度快，生命低
    ENEMY_TANK        // 坦克敌人：速度慢，生命高
} EnemyType;

// 炮台类型枚举
typedef enum {
    TOWER_BASIC = 0,  // 基础炮塔：均衡型
    TOWER_ARROW,      // 初级箭塔：射速快，射程远
    TOWER_CANNON,     // 加农炮：高伤害，射程近
    TOWER_MAGIC       // 魔法塔：特殊攻击
} TowerType;

// 按钮结构体
typedef struct {
    int x, y;           // 按钮位置
    int width, height;  // 按钮尺寸
    const TCHAR* text;  // 按钮文本
    int active;         // 按钮是否可用
    int hovered;        // 鼠标是否悬停
    int clicked;        // 按钮是否被点击
} Button;

// 敌人结构体：定义敌人的所有属性
typedef struct {
    float x, y;           // 当前位置（像素坐标）
    int gridX, gridY;     // 当前网格坐标
    float speed;          // 移动速度（像素/帧）
    int maxSpeed;         // 最大速度
    int life;             // 当前生命值
    int maxLife;          // 最大生命值
    int damage;           // 对基地造成的伤害
    int money;            // 击杀奖励金钱
    int isAlive;          // 是否存活（1存活，0死亡）
    EnemyType type;       // 敌人类型
    COLORREF color;       // 绘制颜色
    int pathIndex;        // 当前路径点索引
    int pathId;           // 路径ID (0,1,2)
} Enemy;

// 炮台结构体：定义炮台的所有属性
typedef struct {
    float x, y;           // 位置（像素坐标）
    int gridX, gridY;     // 网格坐标
    TowerType type;       // 炮台类型
    int damage;           // 伤害值
    int range;            // 攻击范围（像素）
    float attackSpeed;    // 攻击速度（攻击间隔）
    float attackTimer;    // 攻击计时器
    int cost;             // 建造成本
    int upgradeCost;      // 升级成本
    int level;            // 等级（1-3）
    int selected;         // 是否被选中（1选中，0未选）
    int canBuild;         // 是否可以建造（1可建，0不可建）
    COLORREF color;       // 绘制颜色
} Tower;

// 子弹结构体：定义子弹的所有属性
typedef struct {
    float x, y;           // 当前位置
    float targetX, targetY; // 目标位置（敌人位置）
    float speed;          // 飞行速度
    int damage;           // 伤害值
    int fromTower;        // 发射炮台的索引
    int targetEnemy;      // 目标敌人的索引
    int isActive;         // 是否活跃（1活跃，0无效）
    COLORREF color;       // 子弹颜色
} Bullet;

// 游戏状态结构体：存储游戏的所有状态信息
typedef struct {
    int money;            // 玩家金钱
    int life;             // 玩家生命值（基地生命）
    int score;            // 当前得分
    int highScore;        // 最高分
    int wave;             // 当前波次
    int enemiesInWave;    // 当前波次敌人数量
    int enemiesSpawned;   // 已生成的敌人数量
    float gameSpeed;      // 游戏速度（1.0为正常）
    int gameLevel;        // 游戏等级（随得分提升）
    int gamePaused;       // 游戏是否暂停（1暂停，0进行）
    int gameOver;         // 游戏是否结束（1结束，0进行）

    Tower nextTower;      // 预览的下一个防御单位
    Tower* selectedTower; // 当前选中的炮台（用于升级/移除）

    Enemy enemies[MAX_ENEMIES]; // 敌人数组
    int enemyCount;             // 当前敌人数

    Tower towers[MAX_TOWERS];   // 炮台数组
    int towerCount;             // 当前炮台数

    Bullet bullets[MAX_BULLETS]; // 子弹数组
    int bulletCount;             // 当前子弹数

    // 多条路径数组
    POINT paths[NUM_PATHS][20];  // 三条路径，每条最多20个点
    int pathCounts[NUM_PATHS];   // 每条路径的点数

    int pathGrid[MAP_WIDTH][MAP_HEIGHT];  // 0=可建造，1=路径

    // 波次管理
    int waveStarted;         // 波次是否已开始
    int waveSpawning;        // 是否正在生成敌人
    int waveDelayTimer;      // 波次延迟计时器
    int enemySpawnTimer;     // 敌人生成计时器

    // 按钮数组
    Button buttons[6];  // 增加到6个按钮
} GameState;

// 游戏配置结构体：存储游戏平衡参数
typedef struct {
    int towerCosts[4];    // 各类型炮台成本
    int upgradeCosts[3];  // 各等级升级成本
    int enemyScores[3];   // 各类型敌人得分
    int baseLife;         // 基地初始生命
    int baseMoney;        // 初始金钱
} GameConfig;

// 最高分记录结构体
typedef struct {
    char name[20];  // 玩家名称
    int score;      // 得分
    int level;      // 达到的等级
} HighScore;

// 全局变量声明（在game_main.cpp中定义）
extern GameState game;
extern GameConfig config;
extern HighScore highScores[MAX_HIGH_SCORES];
extern int highScoreCount;

// 核心函数声明
void initConfig();
void initPathGrid();
void initPaths();
void initButtons();
void initGame();
void loadHighScores();
void saveHighScores();
void updateHighScores();
void drawGame();
void drawButton(Button* btn);
void handleButtonClick(int x, int y);
void updateButtonHover(int x, int y);
int isPointInButton(int x, int y, Button* btn);
void handleMouseClick(int x, int y, int button);

#endif
