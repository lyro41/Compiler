#pragma once
#include "Item.h"
class JumpElseItem :
    public Item
{
public:
    JumpElseItem(size_t jump_ind = -1);
    size_t jump_ind_ = -1;
};

