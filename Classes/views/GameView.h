#ifndef __GAME_VIEW_H__
#define __GAME_VIEW_H__

#include "cocos2d.h"
#include "CardView.h"
#include "models/GameModel.h"
#include <functional>
#include <map>

/**
 * 游戏主视图类
 * 负责整个游戏界面的显示
 */
class GameView : public cocos2d::Layer {
public:
    static GameView* create();
    
    virtual bool init() override;
    
    // 初始化游戏视图
    void initWithModel(GameModel* model);
    
    // 设置卡牌点击回调
    void setCardClickCallback(const std::function<void(int)>& callback) { _cardClickCallback = callback; }
    
    // 设置备用牌点击回调
    void setTrayClickCallback(const std::function<void()>& callback) { _trayClickCallback = callback; }
    
    // 设置回退按钮点击回调
    void setUndoClickCallback(const std::function<void()>& callback) { _undoClickCallback = callback; }
    
    // 播放卡牌匹配动画
    void playMatchAnimation(int cardId, const cocos2d::Vec2& targetPos, const std::function<void()>& callback = nullptr);
    
    // 播放翻牌动画
    void playFlipTrayAnimation(const CardModel& card, const cocos2d::Vec2& targetPos, const std::function<void()>& callback = nullptr);
    
    // 播放回退动画
    void playUndoAnimation(int cardId, const cocos2d::Vec2& targetPos, const std::function<void()>& callback = nullptr);
    
    // 移除卡牌视图
    void removeCardView(int cardId);
    
    // 添加卡牌视图
    void addCardView(const CardModel& model);
    
    // 获取卡牌视图
    CardView* getCardView(int cardId);
    
    // 更新底牌堆显示
    void updateStackDisplay(const CardModel& topCard);

private:
    void setupBackground();
    void setupUI();
    void setupPlayfieldCards(GameModel* model);
    void setupStackCards(GameModel* model);
    void setupTrayCards(GameModel* model);
    
    std::map<int, CardView*> _cardViews;  // 卡牌ID到视图的映射
    cocos2d::Sprite* _traySprite;          // 备用牌堆精灵
    cocos2d::Sprite* _stackSprite;         // 底牌堆精灵
    
    std::function<void(int)> _cardClickCallback;
    std::function<void()> _trayClickCallback;
    std::function<void()> _undoClickCallback;
};

#endif // __GAME_VIEW_H__