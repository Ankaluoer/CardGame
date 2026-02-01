#include "UndoModel.h"

UndoModel::UndoModel()
    : _actionType(UndoActionType::NONE)
    , _cardId(-1)
    , _fromPosition(cocos2d::Vec2::ZERO)
    , _toPosition(cocos2d::Vec2::ZERO)
{
}

UndoModel::UndoModel(UndoActionType type, int cardId, const cocos2d::Vec2& fromPos, const cocos2d::Vec2& toPos)
    : _actionType(type)
    , _cardId(cardId)
    , _fromPosition(fromPos)
    , _toPosition(toPos)
{
}