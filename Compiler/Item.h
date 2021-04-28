#pragma once
#include <stack>
#include <string>

class Generator;

class Item
{
public:
    
    virtual ~Item() {}
    virtual void Do(Generator* gen) {};
    std::wstring GetType() { return this->type_; }
    void SetType(std::wstring type) { this->type_ = type; }
protected:
    std::wstring type_;


};
