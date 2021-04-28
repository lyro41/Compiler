#pragma once
#include "Item.h"
class LabelItem :
    public Item
{
public:
    LabelItem();
    LabelItem(std::wstring name);
    
    std::wstring name_;
};

