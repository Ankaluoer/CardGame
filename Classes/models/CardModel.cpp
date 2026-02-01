#include "CardModel.h"
#include <cmath>

CardModel::CardModel()
    : _id(-1)
    , _face(CardFaceType::NONE)
    , _suit(CardSuitType::NONE)
    , _position(cocos2d::Vec2::ZERO)
    , _originalPosition(cocos2d::Vec2::ZERO)
{
}

CardModel::CardModel(int id, CardFaceType face, CardSuitType suit, const cocos2d::Vec2& pos)
    : _id(id)
    , _face(face)
    , _suit(suit)
    , _position(pos)
    , _originalPosition(pos)
{
}

bool CardModel::canMatch(const CardModel& other) const
{
    int diff = std::abs(this->getFaceValue() - other.getFaceValue());
    // 点数相差1可以匹配，或者 K(12) 和 A(0) 也可以匹配
    return (diff == 1) || (diff == 12);
}