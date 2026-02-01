#include "UndoManager.h"

UndoManager::UndoManager()
{
}

UndoManager::~UndoManager()
{
    clear();
}

void UndoManager::recordAction(const UndoModel& action)
{
    _undoStack.push_back(action);
}

bool UndoManager::popLastAction(UndoModel& outAction)
{
    if (_undoStack.empty()) {
        return false;
    }
    
    outAction = _undoStack.back();
    _undoStack.pop_back();
    return true;
}

void UndoManager::clear()
{
    _undoStack.clear();
}