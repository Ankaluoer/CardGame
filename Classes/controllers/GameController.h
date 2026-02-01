#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__

#include "cocos2d.h"
#include "models/GameModel.h"
#include "views/GameView.h"
#include "managers/UndoManager.h"

/**
 * 游戏控制器类
 * 协调模型和视图，处理游戏逻辑
 */
class GameController {
public:
    GameController();
    ~GameController();
    
    // 初始化控制器
    bool init(cocos2d::Scene* scene);
    
    // 加载关卡
    bool loadLevel(const std::string& levelFile);
    
    // 处理卡牌点击
    void onCardClicked(int cardId);
    
    // 处理备用牌堆点击
    void onTrayClicked();
    
    // 处理回退按钮点击
    void onUndoClicked();
    
    // 获取游戏模型
    GameModel* getGameModel() { return _gameModel; }
    
    // 获取游戏视图
    GameView* getGameView() { return _gameView; }

private:
    // 尝试匹配卡牌
    bool tryMatchCard(int cardId);
    
    // 执行匹配操作
    void executeMatch(int cardId);
    
    // 执行翻牌操作
    void executeFlipTray();
    
    // 执行回退操作
    void executeUndo();
    
    // 解析关卡配置
    bool parseLevelConfig(const std::string& jsonStr);
    
    GameModel* _gameModel;
    GameView* _gameView;
    UndoManager* _undoManager;
    
    int _nextCardId;  // 用于生成唯一卡牌ID
};

#endif // __GAME_CONTROLLER_H__