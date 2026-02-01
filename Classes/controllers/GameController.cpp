#include "GameController.h"
#include "json/rapidjson.h"
#include "json/document.h"

USING_NS_CC;

GameController::GameController()
    : _gameModel(nullptr)
    , _gameView(nullptr)
    , _undoManager(nullptr)
    , _nextCardId(0)
{
}

GameController::~GameController()
{
    CC_SAFE_DELETE(_gameModel);
    CC_SAFE_DELETE(_undoManager);
    // _gameView 由 cocos2d 管理，不需要手动删除
}

bool GameController::init(Scene* scene)
{
    if (!scene) return false;
    
    _gameModel = new GameModel();
    _undoManager = new UndoManager();
    
    _gameView = GameView::create();
    if (_gameView) {
        scene->addChild(_gameView);
        
        // 设置回调
        _gameView->setCardClickCallback([this](int cardId) {
            this->onCardClicked(cardId);
        });
        
        _gameView->setTrayClickCallback([this]() {
            this->onTrayClicked();
        });
        
        _gameView->setUndoClickCallback([this]() {
            this->onUndoClicked();
        });
    }
    
    return true;
}

bool GameController::loadLevel(const std::string& levelFile)
{
    // 读取关卡配置文件
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(levelFile);
    std::string jsonStr = FileUtils::getInstance()->getStringFromFile(fullPath);
    
    if (jsonStr.empty()) {
        CCLOG("Failed to load level file: %s", levelFile.c_str());
        return false;
    }
    
    if (!parseLevelConfig(jsonStr)) {
        return false;
    }
    
    // 初始化视图
    if (_gameView && _gameModel) {
        _gameView->initWithModel(_gameModel);
    }
    
    return true;
}

bool GameController::parseLevelConfig(const std::string& jsonStr)
{
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());

    if (doc.HasParseError()) {
        CCLOG("JSON parse error!");
        return false;
    }

    const float STACK_AREA_HEIGHT = 580.0f;  // 堆牌区高度

    // 解析主牌区
    if (doc.HasMember("Playfield") && doc["Playfield"].IsArray()) {
        const rapidjson::Value& playfield = doc["Playfield"];
        for (rapidjson::SizeType i = 0; i < playfield.Size(); i++) {
            const rapidjson::Value& cardData = playfield[i];

            int face = cardData["CardFace"].GetInt();
            int suit = cardData["CardSuit"].GetInt();
            float x = cardData["Position"]["x"].GetFloat();
            float y = cardData["Position"]["y"].GetFloat();

            // 主牌区的y坐标需要加上堆牌区高度
            Vec2 pos(x, y + STACK_AREA_HEIGHT);

            CardModel card(_nextCardId++,
                static_cast<CardFaceType>(face),
                static_cast<CardSuitType>(suit),
                pos);
            _gameModel->addPlayfieldCard(card);
        }
    }

    // 解析底牌堆（手牌区右侧的牌）
    // Stack中的牌：最后一张是顶牌（显示的牌），前面的是备用牌
    if (doc.HasMember("Stack") && doc["Stack"].IsArray()) {
        const rapidjson::Value& stack = doc["Stack"];
        int stackSize = stack.Size();

        // 底牌堆位置（右侧）
        Vec2 stackPos(700, 290);
        // 备用牌堆位置（左侧）
        Vec2 trayPos(380, 290);

        for (rapidjson::SizeType i = 0; i < stack.Size(); i++) {
            const rapidjson::Value& cardData = stack[i];

            int face = cardData["CardFace"].GetInt();
            int suit = cardData["CardSuit"].GetInt();

            if (i == stackSize - 1) {
                // 最后一张是底牌堆顶牌
                CardModel card(_nextCardId++,
                    static_cast<CardFaceType>(face),
                    static_cast<CardSuitType>(suit),
                    stackPos);
                _gameModel->addStackCard(card);
            }
            else {
                // 其他是备用牌堆
                CardModel card(_nextCardId++,
                    static_cast<CardFaceType>(face),
                    static_cast<CardSuitType>(suit),
                    trayPos);
                _gameModel->addTrayCard(card);
            }
        }
    }

    return true;
}

void GameController::onCardClicked(int cardId)
{
    CCLOG("Card clicked: %d", cardId);
    tryMatchCard(cardId);
}

void GameController::onTrayClicked()
{
    CCLOG("Tray clicked");
    executeFlipTray();
}

void GameController::onUndoClicked()
{
    CCLOG("Undo clicked");
    executeUndo();
}

bool GameController::tryMatchCard(int cardId)
{
    // 查找点击的卡牌
    auto& playfieldCards = _gameModel->getPlayfieldCards();
    CardModel* clickedCard = nullptr;
    
    for (auto& card : playfieldCards) {
        if (card.getId() == cardId) {
            clickedCard = &card;
            break;
        }
    }
    
    if (!clickedCard) {
        return false;
    }
    
    // 获取底牌堆顶部的牌
    CardModel* topStackCard = _gameModel->getTopStackCard();
    if (!topStackCard) {
        return false;
    }
    
    // 检查是否可以匹配
    if (clickedCard->canMatch(*topStackCard)) {
        executeMatch(cardId);
        return true;
    }
    
    CCLOG("Cards cannot match!");
    return false;
}

void GameController::executeMatch(int cardId)
{
    // 查找卡牌
    auto& playfieldCards = _gameModel->getPlayfieldCards();
    CardModel matchedCard;
    
    for (auto& card : playfieldCards) {
        if (card.getId() == cardId) {
            matchedCard = card;
            break;
        }
    }
    
    // 获取底牌堆顶部位置
    CardModel* topStackCard = _gameModel->getTopStackCard();
    Vec2 targetPos = topStackCard ? topStackCard->getPosition() : Vec2(540, 200);
    
    // 记录撤销操作
    UndoModel undoAction(UndoActionType::MATCH_CARD, 
                         cardId,
                         matchedCard.getPosition(),
                         targetPos);
    _undoManager->recordAction(undoAction);
    
    // 播放动画
    if (_gameView) {
        _gameView->playMatchAnimation(cardId, targetPos, [this, cardId, matchedCard]() {
            // 动画完成后更新数据
            _gameModel->removePlayfieldCard(cardId);
            
            // 将匹配的牌加入底牌堆
            CardModel newStackCard = matchedCard;
            newStackCard.setPosition(_gameModel->getTopStackCard()->getPosition());
            _gameModel->addStackCard(newStackCard);
        });
    }
}

void GameController::executeFlipTray()
{
    auto& trayCards = _gameModel->getTrayCards();
    if (trayCards.empty()) {
        CCLOG("No more tray cards!");
        return;
    }

    // 获取底牌堆顶部位置
    CardModel* topStackCard = _gameModel->getTopStackCard();
    Vec2 targetPos = topStackCard ? topStackCard->getPosition() : Vec2(700, 290);

    // 获取备用牌堆顶部的牌（最后一张）
    CardModel trayCard = trayCards.back();
    Vec2 fromPos = trayCard.getPosition();

    // 记录撤销操作
    UndoModel undoAction(UndoActionType::FLIP_TRAY_CARD,
        trayCard.getId(),
        fromPos,
        targetPos);
    _undoManager->recordAction(undoAction);

    // 播放移动动画
    if (_gameView) {
        _gameView->playFlipTrayAnimation(trayCard, targetPos, [this, trayCard]() {
            // 动画完成后，从备用牌堆移除，加入底牌堆
            auto& trayCards = _gameModel->getTrayCards();
            if (!trayCards.empty()) {
                CardModel card = trayCards.back();
                trayCards.pop_back();

                // 更新位置并加入底牌堆
                CardModel* topStackCard = _gameModel->getTopStackCard();
                Vec2 stackPos = topStackCard ? topStackCard->getPosition() : Vec2(700, 290);
                card.setPosition(stackPos);
                _gameModel->addStackCard(card);
            }
            });
    }
}

void GameController::executeUndo()
{
    if (!_undoManager->canUndo()) {
        CCLOG("Nothing to undo!");
        return;
    }

    UndoModel lastAction;
    if (!_undoManager->popLastAction(lastAction)) {
        return;
    }

    int cardId = lastAction.getCardId();
    Vec2 originalPos = lastAction.getFromPosition();
    UndoActionType actionType = lastAction.getActionType();

    CCLOG("Undo action type: %d, cardId: %d, originalPos: (%f, %f)",
        (int)actionType, cardId, originalPos.x, originalPos.y);

    if (actionType == UndoActionType::MATCH_CARD) {
        // 回退匹配操作：将牌从底牌堆移回主牌区
        auto& stackCards = _gameModel->getStackCards();
        if (stackCards.size() > 1) {  // 确保底牌堆至少有2张牌
            // 获取要回退的牌（底牌堆最上面的牌）
            CardModel cardToUndo = stackCards.back();

            if (_gameView) {
                _gameView->playUndoAnimation(cardToUndo.getId(), originalPos, [this, cardToUndo, originalPos]() {
                    // 动画完成后更新数据
                    auto& stackCards = _gameModel->getStackCards();
                    if (!stackCards.empty()) {
                        stackCards.pop_back();

                        // 恢复到主牌区
                        CardModel restoredCard = cardToUndo;
                        restoredCard.setPosition(originalPos);
                        _gameModel->addPlayfieldCard(restoredCard);
                    }
                    });
            }
        }
    }
    else if (actionType == UndoActionType::FLIP_TRAY_CARD) {
        // 回退翻牌操作：将牌从底牌堆移回备用牌堆
        auto& stackCards = _gameModel->getStackCards();
        if (!stackCards.empty()) {
            CardModel cardToUndo = stackCards.back();

            if (_gameView) {
                _gameView->playUndoAnimation(cardToUndo.getId(), originalPos, [this, cardToUndo, originalPos]() {
                    // 动画完成后更新数据
                    auto& stackCards = _gameModel->getStackCards();
                    if (!stackCards.empty()) {
                        stackCards.pop_back();

                        // 恢复到备用牌堆
                        CardModel restoredCard = cardToUndo;
                        restoredCard.setPosition(originalPos);
                        _gameModel->addTrayCard(restoredCard);
                    }
                    });
            }
        }
    }
}