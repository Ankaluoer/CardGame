#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class GameController;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    ~HelloWorld();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
private:
    GameController* _gameController;
};

#endif // __HELLOWORLD_SCENE_H__