#include "GameView.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

GameView* GameView::create()
{
    GameView* ret = new (std::nothrow) GameView();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool GameView::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    _traySprite = nullptr;
    _stackSprite = nullptr;
    
    setupBackground();
    setupUI();
    
    return true;
}

void GameView::setupBackground()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建上方主牌区背景（棕色）
    auto topBg = LayerColor::create(Color4B(139, 90, 43, 255), 1080, 1500);
    topBg->setPosition(Vec2(0, 580));
    this->addChild(topBg, -1);

    // 创建下方手牌区背景（紫色）
    auto bottomBg = LayerColor::create(Color4B(156, 89, 182, 255), 1080, 580);
    bottomBg->setPosition(Vec2(0, 0));
    this->addChild(bottomBg, -1);
}

void GameView::setupUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建回退按钮（在手牌区右侧）
    auto undoLabel = Label::createWithSystemFont("回退", "Arial", 40);
    undoLabel->setPosition(Vec2(900, 290));
    undoLabel->setTextColor(Color4B::WHITE);
    this->addChild(undoLabel, 100);

    // 为文字添加点击事件
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [undoLabel](Touch* touch, Event* event) {
        Vec2 locationInNode = undoLabel->convertToNodeSpace(touch->getLocation());
        Size size = undoLabel->getContentSize();
        Rect rect = Rect(0, 0, size.width, size.height);
        return rect.containsPoint(locationInNode);
        };
    listener->onTouchEnded = [this, undoLabel](Touch* touch, Event* event) {
        Vec2 locationInNode = undoLabel->convertToNodeSpace(touch->getLocation());
        Size size = undoLabel->getContentSize();
        Rect rect = Rect(0, 0, size.width, size.height);
        if (rect.containsPoint(locationInNode)) {
            if (_undoClickCallback) {
                _undoClickCallback();
            }
        }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, undoLabel);
}

void GameView::initWithModel(GameModel* model)
{
    if (!model) return;
    
    setupPlayfieldCards(model);
    setupStackCards(model);
    setupTrayCards(model);
}

void GameView::setupPlayfieldCards(GameModel* model)
{
    auto& cards = model->getPlayfieldCards();
    for (auto& cardModel : cards) {
        auto cardView = CardView::create(cardModel);
        if (cardView) {
            cardView->setClickCallback([this](int cardId) {
                if (_cardClickCallback) {
                    _cardClickCallback(cardId);
                }
            });
            this->addChild(cardView, 10);
            _cardViews[cardModel.getId()] = cardView;
        }
    }
}

void GameView::setupStackCards(GameModel* model)
{
    // 底牌堆位置（右侧）
    Vec2 stackPos = Vec2(700, 290);

    auto& cards = model->getStackCards();
    if (!cards.empty()) {
        auto& topCard = cards.back();
        // 更新卡牌位置
        CardModel updatedCard = topCard;
        updatedCard.setPosition(stackPos);

        auto cardView = CardView::create(updatedCard);
        if (cardView) {
            cardView->setPosition(stackPos);
            this->addChild(cardView, 5);
            _cardViews[topCard.getId()] = cardView;
        }
    }
}

void GameView::setupTrayCards(GameModel* model)
{
    // 备用牌堆位置（左侧）
    Vec2 trayBasePos = Vec2(300, 290);

    auto& cards = model->getTrayCards();

    CCLOG("Tray cards count: %d", (int)cards.size());

    // 显示所有备用牌堆的牌（左右错开，上下不错开）
    for (size_t i = 0; i < cards.size(); i++) {
        auto& card = cards[i];

        // 每张牌左右错开50像素，和主牌区一致
        Vec2 cardPos = Vec2(trayBasePos.x + i * 50, trayBasePos.y);

        // 更新卡牌模型的位置
        CardModel displayCard(card.getId(), card.getFace(), card.getSuit(), cardPos);
        displayCard.setOriginalPosition(cardPos);

        auto cardView = CardView::create(displayCard);
        if (cardView) {
            cardView->setPosition(cardPos);
            // 所有备用牌都可以点击（点击后移动到底牌堆）
            cardView->setClickCallback([this](int cardId) {
                if (_trayClickCallback) {
                    _trayClickCallback();
                }
                });
            this->addChild(cardView, 5 + (int)i);
            _cardViews[card.getId()] = cardView;
        }

        CCLOG("Added tray card %d at position (%f, %f)", card.getId(), cardPos.x, cardPos.y);
    }
}

void GameView::playMatchAnimation(int cardId, const Vec2& targetPos, const std::function<void()>& callback)
{
    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end()) {
        CardView* cardView = it->second;
        // 提升层级，确保移动的牌显示在最上面
        cardView->setLocalZOrder(100);
        it->second->playMoveAnimation(targetPos, 0.3f, [this, cardId, callback]() {
            // 动画完成后，设置为底牌堆的层级
            auto it = _cardViews.find(cardId);
            if (it != _cardViews.end()) {
                it->second->setLocalZOrder(50);
            }
            if (callback) {
                callback();
            }
            });
    }
}

void GameView::playFlipTrayAnimation(const CardModel& card, const Vec2& targetPos, const std::function<void()>& callback)
{
    auto it = _cardViews.find(card.getId());
    if (it != _cardViews.end()) {
        CardView* cardView = it->second;
        // 提升层级，确保移动的牌显示在最上面
        cardView->setLocalZOrder(100);
        cardView->playMoveAnimation(targetPos, 0.3f, [this, card, callback]() {
            // 动画完成后，设置为底牌堆的层级
            auto it = _cardViews.find(card.getId());
            if (it != _cardViews.end()) {
                it->second->setLocalZOrder(50);
            }
            if (callback) {
                callback();
            }
            });
    }
}

void GameView::playUndoAnimation(int cardId, const Vec2& targetPos, const std::function<void()>& callback)
{
    CCLOG("playUndoAnimation: cardId=%d, targetPos=(%f, %f)", cardId, targetPos.x, targetPos.y);

    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end()) {
        CardView* cardView = it->second;
        // 移动到原始位置，动画完成后调用回调
        cardView->playMoveAnimation(targetPos, 0.3f, callback);
    }
    else {
        CCLOG("Card view not found for id: %d", cardId);
        // 如果找不到视图，直接调用回调
        if (callback) {
            callback();
        }
    }
}

void GameView::removeCardView(int cardId)
{
    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end()) {
        it->second->removeFromParent();
        _cardViews.erase(it);
    }
}

void GameView::addCardView(const CardModel& model)
{
    auto cardView = CardView::create(model);
    if (cardView) {
        cardView->setClickCallback([this](int cardId) {
            if (_cardClickCallback) {
                _cardClickCallback(cardId);
            }
        });
        this->addChild(cardView, 10);
        _cardViews[model.getId()] = cardView;
    }
}

CardView* GameView::getCardView(int cardId)
{
    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end()) {
        return it->second;
    }
    return nullptr;
}

void GameView::updateStackDisplay(const CardModel& topCard)
{
    // 更新底牌堆顶部显示
    auto it = _cardViews.find(topCard.getId());
    if (it == _cardViews.end()) {
        auto cardView = CardView::create(topCard);
        if (cardView) {
            this->addChild(cardView, 5);
            _cardViews[topCard.getId()] = cardView;
        }
    }
}