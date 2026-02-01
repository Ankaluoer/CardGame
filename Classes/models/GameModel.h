#ifndef __GAME_MODEL_H__
#define __GAME_MODEL_H__

#include "CardModel.h"
#include <vector>

/**
 * 游戏数据模型类
 * 存储整个游戏的状态数据
 */
class GameModel {
public:
    GameModel();
    ~GameModel();
    
    // 主牌区的牌（桌面上的牌）
    std::vector<CardModel>& getPlayfieldCards() { return _playfieldCards; }
    const std::vector<CardModel>& getPlayfieldCards() const { return _playfieldCards; }
    
    // 底牌堆（手牌区顶部的牌）
    std::vector<CardModel>& getStackCards() { return _stackCards; }
    const std::vector<CardModel>& getStackCards() const { return _stackCards; }
    
    // 备用牌堆（手牌区可以翻的牌）
    std::vector<CardModel>& getTrayCards() { return _trayCards; }
    const std::vector<CardModel>& getTrayCards() const { return _trayCards; }
    
    // 获取底牌堆顶部的牌
    CardModel* getTopStackCard();
    
    // 添加牌到各个区域
    void addPlayfieldCard(const CardModel& card);
    void addStackCard(const CardModel& card);
    void addTrayCard(const CardModel& card);
    
    // 从主牌区移除牌
    bool removePlayfieldCard(int cardId);
    
    // 从备用牌堆移除顶部牌
    CardModel* popTrayCard();
    
    // 清空所有数据
    void clear();

private:
    std::vector<CardModel> _playfieldCards;  // 主牌区
    std::vector<CardModel> _stackCards;       // 底牌堆
    std::vector<CardModel> _trayCards;        // 备用牌堆
};

#endif // __GAME_MODEL_H__