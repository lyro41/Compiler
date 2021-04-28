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
    std::vector<std::pair<size_t, Type>> loop_jumps;
    LoopJumpable(size_t inc_ind = -1, size_t else_ind = -1, size_t end_ind = -1, Type tp=Type::NONE) :
        inc_ind_(inc_ind), else_ind_(else_ind), end_ind_(end_ind), type(Type::NONE){}

};

struct NameTable {
    enum class TableType {
        VAR,
        STRUCT,
        FUNC,
        NAMESPACE,
        NONE
    };
    std::wstring name = L"";
    TableType type = TableType::NONE;
    NameTable* parrent = nullptr;
    size_t size = 0;
    size_t jump_in = 0;
    size_t offset = 0;
    std::map<std::wstring, int> mapping;
    std::vector<NameTable*> sons = {};
    void add_son(std::wstring name) {
        NameTable* n_Table = new NameTable(name);
        sons.push_back(n_Table);
        n_Table->parrent = this;
        mapping[n_Table->name] = sons.size() - 1;
    }
    void add_son(NameTable* son) {
        sons.push_back(son);
        son->parrent = this;
        mapping[son->name] = sons.size() - 1;
    }
    size_t update_size() {
        if (sons.size() == 0) {
            return size;
        }
        size_t sm = 0;
        for (auto& elem : sons) {
            sm += elem->update_size();
        }
        this->size = sm;
        return size;
    }
    NameTable(std::wstring nm) {
        this->name = nm;

    }   
    NameTable(std::wstring nm, TableType n_type) {
        this->name = nm;
        this->type = n_type;
    }
};

//struct FuncTable {
//    std::wstring name = L"";
//    FuncTable* parrent = nullptr;
//    size_t jmp_in = 0;
//    std::map<std::wstring, int> mapping;
//    std::vector<FuncTable*> sons;
//    void add_son(std::wstring name) {
//        FuncTable* n_Table = new FuncTable();
//        n_Table->name = name;
//        sons.push_back(n_Table);
//        mapping[n_Table->name] = sons.size() - 1;
//    }
//    void add_son(std::wstring name, std::wstring type) {
//        FuncTable* n_Table = new FuncTable();
//        n_Table->name = name;
//        n_Table->type = type
//        sons.push_back(n_Table);
//        mapping[n_Table->name] = sons.size() - 1;
//    }
//    
//};

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
    
    //void PushFor();
    //void PushIf();

    bool should_skip = false;
    NameTable* global_table = new NameTable(L"");
    NameTable* current_table = global_table;
    size_t max_frame_size = 0;
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
    std::vector <std::pair<size_t, std::wstring>> incompleteJumps;
    std::map<std::wstring, size_t> labels;
    std::stack<size_t> frame_offset;    

};

