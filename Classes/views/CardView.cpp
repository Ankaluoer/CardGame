#include "CardView.h"

USING_NS_CC;

CardView* CardView::create(const CardModel& model)
{
    CardView* ret = new (std::nothrow) CardView();
    if (ret && ret->init(model)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool CardView::init(const CardModel& model)
{
    if (!Sprite::init()) {
        return false;
    }
    
    _cardModel = model;
    _cardId = model.getId();
    
    setupCardTexture();
    setupTouchListener();
    
    this->setPosition(model.getPosition());
    
    return true;
}

void CardView::setupCardTexture()
{
    // 获取花色和牌面
    int suit = static_cast<int>(_cardModel.getSuit());
    int face = static_cast<int>(_cardModel.getFace());

    // 加载卡牌背景
    this->setTexture("res/card_general.png");

    Size cardSize = this->getContentSize();

    // 判断是红色还是黑色花色
    bool isRed = (suit == 1 || suit == 2);  // 方块或红桃是红色
    std::string colorPrefix = isRed ? "red" : "black";

    // 获取牌面字符串
    std::string faceStr;
    switch (face) {
    case 0:  faceStr = "A"; break;
    case 1:  faceStr = "2"; break;
    case 2:  faceStr = "3"; break;
    case 3:  faceStr = "4"; break;
    case 4:  faceStr = "5"; break;
    case 5:  faceStr = "6"; break;
    case 6:  faceStr = "7"; break;
    case 7:  faceStr = "8"; break;
    case 8:  faceStr = "9"; break;
    case 9:  faceStr = "10"; break;
    case 10: faceStr = "J"; break;
    case 11: faceStr = "Q"; break;
    case 12: faceStr = "K"; break;
    default: faceStr = "A"; break;
    }

    // 花色文件
    std::string suitFile;
    switch (suit) {
    case 0: suitFile = "res/suits/club.png"; break;
    case 1: suitFile = "res/suits/diamond.png"; break;
    case 2: suitFile = "res/suits/heart.png"; break;
    case 3: suitFile = "res/suits/spade.png"; break;
    default: suitFile = "res/suits/club.png"; break;
    }

    // 左上角数字（稍微往下移，不贴着上边沿）
    std::string smallNumFile = "res/number/small_" + colorPrefix + "_" + faceStr + ".png";
    auto leftNumSprite = Sprite::create(smallNumFile);
    if (leftNumSprite) {
        leftNumSprite->setPosition(Vec2(35, cardSize.height - 40));
        leftNumSprite->setScale(0.9f);
        this->addChild(leftNumSprite);
    }

    // 右上角花色（和左上角数字一样大，位置对称）
    auto rightSuitSprite = Sprite::create(suitFile);
    if (rightSuitSprite) {
        rightSuitSprite->setPosition(Vec2(cardSize.width - 35, cardSize.height - 40));
        rightSuitSprite->setScale(0.6f);
        this->addChild(rightSuitSprite);
    }

    // 中间大数字
    std::string bigNumFile = "res/number/big_" + colorPrefix + "_" + faceStr + ".png";
    auto bigNumSprite = Sprite::create(bigNumFile);
    if (bigNumSprite) {
        bigNumSprite->setPosition(Vec2(cardSize.width / 2, cardSize.height / 2 - 10));
        bigNumSprite->setScale(1.0f);
        this->addChild(bigNumSprite);
    }
}

void CardView::setupTouchListener()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        Vec2 locationInNode = this->convertToNodeSpace(touch->getLocation());
        Size size = this->getContentSize();
        Rect rect = Rect(0, 0, size.width, size.height);
        
        if (rect.containsPoint(locationInNode)) {
            return true;
        }
        return false;
    };
    
    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        Vec2 locationInNode = this->convertToNodeSpace(touch->getLocation());
        Size size = this->getContentSize();
        Rect rect = Rect(0, 0, size.width, size.height);
        
        if (rect.containsPoint(locationInNode)) {
            if (_clickCallback) {
                _clickCallback(_cardId);
            }
        }
    };
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CardView::playMoveAnimation(const Vec2& targetPos, float duration, const std::function<void()>& callback)
{
    auto moveTo = MoveTo::create(duration, targetPos);
    
    if (callback) {
        auto callFunc = CallFunc::create(callback);
        auto sequence = Sequence::create(moveTo, callFunc, nullptr);
        this->runAction(sequence);
    } else {
        this->runAction(moveTo);
    }
}

void CardView::updateCardModel(const CardModel& model)
{
    _cardModel = model;
    _cardId = model.getId();
}