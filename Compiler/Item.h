#pragma once
#include <stack>
#include <string>

class Generator;

class Item
{
public:
    
    virtual ~Item() {}
    virtual void Do(Generator* gen) = 0;
    std::wstring GetType() { return this->type_; }
    std::wstring SetType(std::wstring type) { this->type_ = type; }
private:
    std::wstring type_;


};

