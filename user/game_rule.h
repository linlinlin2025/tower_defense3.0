#ifndef GAME_RULE_H
#define GAME_RULE_H

#include "game_core.h"

//@brief 生成下一个预览的防御塔（炮台）
  //此函数会随机生成一个将在预览区域显示的防御塔类型和属性
// 可以使用这个预览来规划下一步的建造策略

void generateNextTower();


 // @brief 检查指定网格位置是否可以建造防御塔
 //
 // @param gridX X轴网格坐标
 // @param gridY Y轴网格坐标
 // @return int 返回1表示可以建造，返回0表示不能建造
 //
 // 此函数检查建造位置的合法性，包括：
 // 1. 是否在地图边界内
 // 2. 是否在敌人路径上（避免阻挡敌人移动）
 // 3. 是否已经有其他防御塔占据该位置
int canBuildAt(int gridX, int gridY);

 // @brief 更新游戏波次状态
 //
 // 此函数在每一帧被调用，负责管理：
 // 1. 敌人生成计时和它的控制
 // 2. 波次完成检测
 // 3. 波次间延迟处理
 // 4. 奖励分配（金币和分数）

void updateWave();


 // @brief 开始新的敌人波次
 //
 // 此函数初始化新波次的参数
 // 1. 增加波次数
 // 2. 根据当前难度设置敌人数量
 // 3. 重置相关的计时器和计数器
 // 4. 开始生成敌人

void startNewWave();


 // @brief 更新最高分显示
 //
 // 比较当前得分和历史最高分，
 // 如果当前得分更高则更新最高分记录。

void updateHighScoreDisplay();

#endif 
