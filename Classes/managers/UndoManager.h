#ifndef __UNDO_MANAGER_H__
#define __UNDO_MANAGER_H__

#include "models/UndoModel.h"
#include <vector>
#include <functional>

/**
 * 撤销管理器类
 * 管理所有的撤销操作记录
 */
class UndoManager {
public:
    UndoManager();
    ~UndoManager();
    
    // 记录一次操作
    void recordAction(const UndoModel& action);
    
    // 获取并移除最后一次操作（用于撤销）
    bool popLastAction(UndoModel& outAction);
    
    // 检查是否有可撤销的操作
    bool canUndo() const { return !_undoStack.empty(); }
    
    // 获取撤销栈大小
    size_t getUndoCount() const { return _undoStack.size(); }
    
    // 清空所有记录
    void clear();

private:
    std::vector<UndoModel> _undoStack;  // 撤销栈
};

#endif // __UNDO_MANAGER_H__