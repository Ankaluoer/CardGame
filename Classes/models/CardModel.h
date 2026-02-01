#ifndef __CARD_MODEL_H__
#define __CARD_MODEL_H__

#include "cocos2d.h"

/**
 * 花色类型枚举
 */
enum class CardSuitType {
    NONE = -1,
    CLUBS = 0,      // 梅花
    DIAMONDS = 1,   // 方块
    HEARTS = 2,     // 红桃
    SPADES = 3      // 黑桃
};

/**
 * 牌面类型枚举
 */
enum class CardFaceType {
    NONE = -1,
    ACE = 0,
    TWO = 1,
    THREE = 2,
    FOUR = 3,
    FIVE = 4,
    SIX = 5,
    SEVEN = 6,
    EIGHT = 7,
    NINE = 8,
    TEN = 9,
    JACK = 10,
    QUEEN = 11,
    KING = 12
};

/**
 * 卡牌数据模型类
 * 存储单张卡牌的数据信息
 */
class CardModel {
public:
    CardModel();
    CardModel(int id, CardFaceType face, CardSuitType suit, const cocos2d::Vec2& pos);
    
    // Getters
    int getId() const { return _id; }
    CardFaceType getFace() const { return _face; }
    CardSuitType getSuit() const { return _suit; }
    cocos2d::Vec2 getPosition() const { return _position; }
    cocos2d::Vec2 getOriginalPosition() const { return _originalPosition; }
    
    // Setters
    void setId(int id) { _id = id; }
    void setFace(CardFaceType face) { _face = face; }
    void setSuit(CardSuitType suit) { _suit = suit; }
    void setPosition(const cocos2d::Vec2& pos) { _position = pos; }
    void setOriginalPosition(const cocos2d::Vec2& pos) { _originalPosition = pos; }
    
    /**
     * 判断两张牌是否可以匹配（点数相差1）
     */
    bool canMatch(const CardModel& other) const;
    
    /**
     * 获取牌面数值（用于匹配判断）
     */
    int getFaceValue() const { return static_cast<int>(_face); }

private:
    int _id;                           // 卡牌唯一ID
    CardFaceType _face;                // 牌面
    CardSuitType _suit;                // 花色
    cocos2d::Vec2 _position;           // 当前位置
    cocos2d::Vec2 _originalPosition;   // 原始位置（用于回退）
};

#endif // __CARD_MODEL_H__