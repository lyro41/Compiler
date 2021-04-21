#pragma once
#include <stack>
#include <iostream>
#include <map>
#include <vector>
#include "Item.h"
#include "FuncReturnItem.h"
#include "JumpElseItem.h"
#include "JumpItem.h"
#include "LabelItem.h"
#include "ScopeEndItem.h"
#include "ScopeStackClearItem.h"
#include "VarItem.h"
#include "SeparatorItem.h"
#include "ScopeBeginItem.h"

class Parser;

struct LoopJumpable {
    size_t inc_ind_ = -1;
    size_t else_ind_ = -1;
    size_t end_ind_ = -1;
    enum class Type {
        BREAK,
        CONTINUE,
        NONE
    };
    Type type = Type::NONE;
    std::vector<std::pair<int, Type>> loop_jumps;
    LoopJumpable(size_t inc_ind = -1, size_t else_ind = -1, size_t end_ind = -1, Type tp=Type::NONE) :
        inc_ind_(inc_ind), else_ind_(else_ind), end_ind_(end_ind), type(Type::NONE){}
};


class Generator
{
public:
    void PushItemToRpn(Item* item);
    void PushItemToProgram(Item* item);
    size_t GetCurrentCursor();
    void ClrBuffer();
    void CompleteRPN();
    void CompleteRPN(int begin, int end);
    void PushSeparator();
    void PushFor();
    void PushIf();
    //void PushWhile();
    //void PushDoWhile();
    //void PushFunc();
    //void PushProto();
    friend Item;
    friend Parser;
private:
    std::vector<LoopJumpable> incompleteLoopJumps;
    std::vector<Item*> buffer_;
    std::vector<Item*> program;
    std::vector<size_t> incompleteJumps;


};

