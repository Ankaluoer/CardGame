#ifndef __UNDO_MODEL_H__
#define __UNDO_MODEL_H__

#include "cocos2d.h"

/**
 * 操作类型枚举
 */
enum class UndoActionType {
    NONE = 0,
    MATCH_CARD,      // 匹配消除卡牌
    FLIP_TRAY_CARD   // 翻备用牌
};

/**
 * 撤销操作数据模型
 * 记录一次操作的信息，用于回退
 */
class UndoModel {
public:
    UndoModel();
    UndoModel(UndoActionType type, int cardId, const cocos2d::Vec2& fromPos, const cocos2d::Vec2& toPos);
    
    // Getters
    UndoActionType getActionType() const { return _actionType; }
    int getCardId() const { return _cardId; }
    cocos2d::Vec2 getFromPosition() const { return _fromPosition; }
    cocos2d::Vec2 getToPosition() const { return _toPosition; }
    
    // Setters
    void setActionType(UndoActionType type) { _actionType = type; }
    void setCardId(int id) { _cardId = id; }
    void setFromPosition(const cocos2d::Vec2& pos) { _fromPosition = pos; }
    void setToPosition(const cocos2d::Vec2& pos) { _toPosition = pos; }

private:
    UndoActionType _actionType;      // 操作类型
    int _cardId;                     // 操作的卡牌ID
    cocos2d::Vec2 _fromPosition;     // 原始位置
    cocos2d::Vec2 _toPosition;       // 目标位置
};

#endif // __UNDO_MODEL_H__