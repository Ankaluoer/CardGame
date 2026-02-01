#ifndef __CARD_VIEW_H__
#define __CARD_VIEW_H__

#include "cocos2d.h"
#include "models/CardModel.h"
#include <functional>

/**
 * 卡牌视图类
 * 负责单张卡牌的显示和交互
 */
class CardView : public cocos2d::Sprite {
public:
    static CardView* create(const CardModel& model);
    
    bool init(const CardModel& model);
    
    // 获取卡牌ID
    int getCardId() const { return _cardId; }
    
    // 获取卡牌数据
    const CardModel& getCardModel() const { return _cardModel; }
    
    // 设置点击回调
    void setClickCallback(const std::function<void(int)>& callback) { _clickCallback = callback; }
    
    // 播放移动动画
    void playMoveAnimation(const cocos2d::Vec2& targetPos, float duration, const std::function<void()>& callback = nullptr);
    
    // 更新卡牌数据
    void updateCardModel(const CardModel& model);

private:
    // 创建卡牌纹理
    void setupCardTexture();
    
    // 设置触摸事件
    void setupTouchListener();
    
    int _cardId;
    CardModel _cardModel;
    std::function<void(int)> _clickCallback;
};

#endif // __CARD_VIEW_H__