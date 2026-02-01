#include "HelloWorldScene.h"
#include "controllers/GameController.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

bool HelloWorld::init()
{
    if (!Scene::init())
    {
        return false;
    }

    _gameController = nullptr;  // 先初始化为空

    // 创建游戏控制器
    _gameController = new GameController();
    if (_gameController && _gameController->init(this)) {
        // 加载关卡
        _gameController->loadLevel("level1.json");
    }

    return true;
}

HelloWorld::~HelloWorld()
{
    if (_gameController) {
        delete _gameController;
        _gameController = nullptr;
    }
}