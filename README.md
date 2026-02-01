# 纸牌消除游戏 - 程序设计文档

## 一、项目概述

### 1.1 项目简介
本项目是一个基于 Cocos2d-x 3.17 开发的纸牌消除游戏，实现了卡牌匹配消除、翻牌和回退功能。

### 1.2 开发环境
- **游戏引擎**: Cocos2d-x 3.17
- **开发工具**: Visual Studio 2022
- **编程语言**: C++
- **设计分辨率**: 1080 × 2080
- **目标平台**: Windows

### 1.3 核心功能
1. **手牌区翻牌替换**: 点击备用牌堆的牌，移动到底牌堆顶部
2. **桌面牌匹配消除**: 点击桌面牌，如果与底牌堆顶部牌点数差1，则匹配成功并移动
3. **回退功能**: 可以撤销之前的操作，牌回到原位

---

## 二、架构设计

### 2.1 整体架构
本项目采用 **MVC（Model-View-Controller）架构**，将数据、视图和逻辑分离，提高代码的可维护性和可扩展性。

```
┌─────────────────────────────────────────────────────────────┐
│                        GameController                       │
│                    （控制器：协调模型和视图）                 │
└─────────────────────────────────────────────────────────────┘
         │                      │                      │
         ▼                      ▼                      ▼
┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐
│    GameModel    │  │    GameView     │  │   UndoManager   │
│  （数据模型）    │  │   （视图层）     │  │  （回退管理）    │
└─────────────────┘  └─────────────────┘  └─────────────────┘
         │                      │
         ▼                      ▼
┌─────────────────┐  ┌─────────────────┐
│   CardModel     │  │    CardView     │
│ （卡牌数据）     │  │  （卡牌视图）    │
└─────────────────┘  └─────────────────┘
```

### 2.2 目录结构

```
Classes/
├── configs/           # 配置相关（预留）
├── models/            # 数据模型层
│   ├── CardModel.h/cpp      # 卡牌数据模型
│   ├── GameModel.h/cpp      # 游戏数据模型
│   └── UndoModel.h/cpp      # 撤销操作数据模型
├── views/             # 视图层
│   ├── CardView.h/cpp       # 卡牌视图
│   └── GameView.h/cpp       # 游戏主视图
├── controllers/       # 控制器层
│   └── GameController.h/cpp # 游戏控制器
├── managers/          # 管理器层
│   └── UndoManager.h/cpp    # 撤销管理器
├── services/          # 服务层（预留）
└── utils/             # 工具类（预留）
```

---

## 三、核心类设计

### 3.1 CardModel（卡牌数据模型）

**职责**: 存储单张卡牌的数据信息

```cpp
class CardModel {
private:
    int _id;                    // 卡牌唯一ID
    CardFaceType _face;         // 牌面（A-K）
    CardSuitType _suit;         // 花色（梅花/方块/红桃/黑桃）
    Vec2 _position;             // 当前位置
    Vec2 _originalPosition;     // 原始位置（用于回退）

public:
    bool canMatch(const CardModel& other) const;  // 判断是否可匹配
    int getFaceValue() const;                      // 获取牌面数值
};
```

**匹配规则**: 两张牌的点数相差1即可匹配（K和A也可匹配）

### 3.2 GameModel（游戏数据模型）

**职责**: 存储整个游戏的状态数据

```cpp
class GameModel {
private:
    vector<CardModel> _playfieldCards;  // 主牌区（桌面上的牌）
    vector<CardModel> _stackCards;       // 底牌堆（手牌区顶部的牌）
    vector<CardModel> _trayCards;        // 备用牌堆

public:
    CardModel* getTopStackCard();        // 获取底牌堆顶部的牌
    void addPlayfieldCard(const CardModel& card);
    bool removePlayfieldCard(int cardId);
    CardModel* popTrayCard();
};
```

### 3.3 UndoModel（撤销操作数据模型）

**职责**: 记录一次操作的信息，用于回退

```cpp
enum class UndoActionType {
    MATCH_CARD,       // 匹配消除卡牌
    FLIP_TRAY_CARD    // 翻备用牌
};

class UndoModel {
private:
    UndoActionType _actionType;  // 操作类型
    int _cardId;                 // 操作的卡牌ID
    Vec2 _fromPosition;          // 原始位置
    Vec2 _toPosition;            // 目标位置
};
```

### 3.4 CardView（卡牌视图）

**职责**: 负责单张卡牌的显示和交互

```cpp
class CardView : public Sprite {
public:
    static CardView* create(const CardModel& model);
    
    void setClickCallback(const function<void(int)>& callback);
    void playMoveAnimation(const Vec2& targetPos, float duration, 
                           const function<void()>& callback);
private:
    void setupCardTexture();     // 创建卡牌纹理
    void setupTouchListener();   // 设置触摸事件
};
```

### 3.5 GameView（游戏主视图）

**职责**: 负责整个游戏界面的显示

```cpp
class GameView : public Layer {
public:
    void initWithModel(GameModel* model);
    
    void playMatchAnimation(int cardId, const Vec2& targetPos, ...);
    void playFlipTrayAnimation(const CardModel& card, ...);
    void playUndoAnimation(int cardId, const Vec2& targetPos, ...);
    
private:
    map<int, CardView*> _cardViews;  // 卡牌ID到视图的映射
    
    void setupBackground();
    void setupUI();
    void setupPlayfieldCards(GameModel* model);
    void setupStackCards(GameModel* model);
    void setupTrayCards(GameModel* model);
};
```

### 3.6 GameController（游戏控制器）

**职责**: 协调模型和视图，处理游戏逻辑

```cpp
class GameController {
public:
    bool init(Scene* scene);
    bool loadLevel(const string& levelFile);
    
    void onCardClicked(int cardId);    // 处理卡牌点击
    void onTrayClicked();              // 处理备用牌点击
    void onUndoClicked();              // 处理回退按钮点击

private:
    bool tryMatchCard(int cardId);     // 尝试匹配
    void executeMatch(int cardId);     // 执行匹配
    void executeFlipTray();            // 执行翻牌
    void executeUndo();                // 执行回退
    
    GameModel* _gameModel;
    GameView* _gameView;
    UndoManager* _undoManager;
};
```

### 3.7 UndoManager（撤销管理器）

**职责**: 管理所有的撤销操作记录

```cpp
class UndoManager {
private:
    vector<UndoModel> _undoStack;  // 撤销栈

public:
    void recordAction(const UndoModel& action);  // 记录操作
    bool popLastAction(UndoModel& outAction);    // 获取并移除最后操作
    bool canUndo() const;                        // 是否可撤销
    void clear();                                // 清空记录
};
```

---

## 四、核心流程

### 4.1 游戏初始化流程

```
AppDelegate::applicationDidFinishLaunching()
    │
    ▼
HelloWorld::init()
    │
    ▼
GameController::init(scene)
    ├── 创建 GameModel
    ├── 创建 UndoManager
    ├── 创建 GameView 并添加到场景
    └── 设置回调函数
    │
    ▼
GameController::loadLevel("level1.json")
    ├── 读取JSON配置文件
    ├── 解析主牌区卡牌
    ├── 解析底牌堆卡牌
    ├── 解析备用牌堆卡牌
    └── GameView::initWithModel()
```

### 4.2 卡牌点击流程

```
用户点击卡牌
    │
    ▼
CardView::onTouchEnded()
    │
    ▼
_clickCallback(cardId)  [回调]
    │
    ▼
GameController::onCardClicked(cardId)
    │
    ▼
GameController::tryMatchCard(cardId)
    ├── 查找点击的卡牌
    ├── 获取底牌堆顶部牌
    └── 检查是否可匹配 (canMatch)
    │
    ▼ [如果可匹配]
GameController::executeMatch(cardId)
    ├── 记录 UndoModel 到 UndoManager
    └── GameView::playMatchAnimation()
            │
            ▼ [动画完成]
        更新 GameModel 数据
```

### 4.3 回退操作流程

```
用户点击"回退"按钮
    │
    ▼
GameView::_undoClickCallback()
    │
    ▼
GameController::onUndoClicked()
    │
    ▼
GameController::executeUndo()
    ├── UndoManager::popLastAction()
    ├── 根据 actionType 判断操作类型
    └── GameView::playUndoAnimation()
            │
            ▼ [动画完成]
        恢复 GameModel 数据
```

---

## 五、扩展指南

### 5.1 如何新增一种卡牌类型

假设要添加一种新的卡牌类型（如"万能牌"），步骤如下：

**步骤 1**: 修改 `CardModel.h`，添加新的属性

```cpp
// 在 CardModel 类中添加
enum class CardSpecialType {
    NONE = 0,
    WILD = 1,    // 万能牌，可以和任何牌匹配
};

class CardModel {
private:
    CardSpecialType _specialType;  // 新增：特殊类型
    
public:
    CardSpecialType getSpecialType() const { return _specialType; }
    void setSpecialType(CardSpecialType type) { _specialType = type; }
};
```

**步骤 2**: 修改 `CardModel::canMatch()` 方法

```cpp
bool CardModel::canMatch(const CardModel& other) const
{
    // 万能牌可以和任何牌匹配
    if (_specialType == CardSpecialType::WILD || 
        other.getSpecialType() == CardSpecialType::WILD) {
        return true;
    }
    
    // 原有匹配逻辑
    int diff = std::abs(this->getFaceValue() - other.getFaceValue());
    return (diff == 1) || (diff == 12);
}
```

**步骤 3**: 修改 `CardView::setupCardTexture()` 添加新的显示效果

```cpp
void CardView::setupCardTexture()
{
    // ... 原有代码 ...
    
    // 如果是万能牌，添加特殊标识
    if (_cardModel.getSpecialType() == CardSpecialType::WILD) {
        auto wildIcon = Sprite::create("res/wild_icon.png");
        wildIcon->setPosition(Vec2(cardSize.width/2, cardSize.height - 20));
        this->addChild(wildIcon);
    }
}
```

**步骤 4**: 修改 `GameController::parseLevelConfig()` 解析新字段

```cpp
// 在解析卡牌数据时
if (cardData.HasMember("SpecialType")) {
    int specialType = cardData["SpecialType"].GetInt();
    card.setSpecialType(static_cast<CardSpecialType>(specialType));
}
```

### 5.2 如何新增一种回退功能

假设要添加"洗牌"功能的回退，步骤如下：

**步骤 1**: 在 `UndoModel.h` 中添加新的操作类型

```cpp
enum class UndoActionType {
    NONE = 0,
    MATCH_CARD,       // 匹配消除卡牌
    FLIP_TRAY_CARD,   // 翻备用牌
    SHUFFLE_CARDS     // 新增：洗牌操作
};
```

**步骤 2**: 扩展 `UndoModel` 存储更多数据

```cpp
class UndoModel {
private:
    // ... 原有成员 ...
    
    // 新增：存储洗牌前的所有卡牌位置
    std::vector<std::pair<int, Vec2>> _cardPositions;
    
public:
    void setCardPositions(const std::vector<std::pair<int, Vec2>>& positions) {
        _cardPositions = positions;
    }
    const std::vector<std::pair<int, Vec2>>& getCardPositions() const {
        return _cardPositions;
    }
};
```

**步骤 3**: 在 `GameController` 中实现洗牌功能

```cpp
void GameController::executeShuffle()
{
    auto& cards = _gameModel->getPlayfieldCards();
    
    // 记录当前所有卡牌位置
    UndoModel undoAction;
    undoAction.setActionType(UndoActionType::SHUFFLE_CARDS);
    std::vector<std::pair<int, Vec2>> positions;
    for (auto& card : cards) {
        positions.push_back({card.getId(), card.getPosition()});
    }
    undoAction.setCardPositions(positions);
    _undoManager->recordAction(undoAction);
    
    // 执行洗牌逻辑
    // ... 随机打乱卡牌位置 ...
}
```

**步骤 4**: 在 `GameController::executeUndo()` 中处理新的回退类型

```cpp
void GameController::executeUndo()
{
    // ... 原有代码 ...
    
    if (actionType == UndoActionType::SHUFFLE_CARDS) {
        // 回退洗牌：恢复所有卡牌到原位置
        auto& positions = lastAction.getCardPositions();
        for (auto& pair : positions) {
            int cardId = pair.first;
            Vec2 originalPos = pair.second;
            
            // 播放动画，移动回原位置
            if (_gameView) {
                _gameView->playUndoAnimation(cardId, originalPos, nullptr);
            }
            
            // 更新模型数据
            // ...
        }
    }
}
```

---

## 六、关卡配置说明

### 6.1 配置文件格式

关卡配置使用 JSON 格式，存放在 `Resources/level1.json`：

```json
{
    "Playfield": [
        {
            "CardFace": 12,    // 牌面：0=A, 1=2, ..., 12=K
            "CardSuit": 0,     // 花色：0=梅花, 1=方块, 2=红桃, 3=黑桃
            "Position": {"x": 250, "y": 1000}  // 相对于主牌区的位置
        }
    ],
    "Stack": [
        // 最后一张是底牌堆顶牌，其他是备用牌堆
    ]
}
```

### 6.2 枚举定义

```cpp
// 花色类型
enum class CardSuitType {
    CLUBS = 0,      // 梅花
    DIAMONDS = 1,   // 方块
    HEARTS = 2,     // 红桃
    SPADES = 3      // 黑桃
};

// 牌面类型
enum class CardFaceType {
    ACE = 0,   TWO = 1,   THREE = 2, FOUR = 3,
    FIVE = 4,  SIX = 5,   SEVEN = 6, EIGHT = 7,
    NINE = 8,  TEN = 9,   JACK = 10, QUEEN = 11,
    KING = 12
};
```

---

## 七、编译与运行

### 7.1 环境要求
- Visual Studio 2022（含 C++ 桌面开发组件）
- Python 2.7
- Cocos2d-x 3.17

### 7.2 编译步骤
1. 打开 `proj.win32/CardGame2.sln`
2. 右键解决方案 → 重定解决方案目标 → 选择最新 SDK 版本
3. 按 F5 编译运行

---

## 八、总结

本项目采用 MVC 架构，实现了清晰的代码分层：
- **Model层** 负责数据存储，不包含业务逻辑
- **View层** 负责界面显示和用户交互
- **Controller层** 协调 Model 和 View，处理业务逻辑
- **Manager层** 提供特定功能的管理（如回退管理）

这种架构使得代码易于维护和扩展，添加新功能时只需按照对应的层级进行修改。