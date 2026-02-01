#include "GameModel.h"

GameModel::GameModel()
{
}

GameModel::~GameModel()
{
    clear();
}

CardModel* GameModel::getTopStackCard()
{
    if (_stackCards.empty()) {
        return nullptr;
    }
    return &_stackCards.back();
}

void GameModel::addPlayfieldCard(const CardModel& card)
{
    _playfieldCards.push_back(card);
}

void GameModel::addStackCard(const CardModel& card)
{
    _stackCards.push_back(card);
}

void GameModel::addTrayCard(const CardModel& card)
{
    _trayCards.push_back(card);
}

bool GameModel::removePlayfieldCard(int cardId)
{
    for (auto it = _playfieldCards.begin(); it != _playfieldCards.end(); ++it) {
        if (it->getId() == cardId) {
            _playfieldCards.erase(it);
            return true;
        }
    }
    return false;
}

CardModel* GameModel::popTrayCard()
{
    if (_trayCards.empty()) {
        return nullptr;
    }
    // 将最后一张牌移动到返回值，然后从列表中移除
    _stackCards.push_back(_trayCards.back());
    _trayCards.pop_back();
    return &_stackCards.back();
}

void GameModel::clear()
{
    _playfieldCards.clear();
    _stackCards.clear();
    _trayCards.clear();
}