#pragma once
#include "Item.h"
class JumpItem :
    public Item
{
public:
    JumpItem(size_t jump_ind = -1);

    size_t jump_ind_ = -1;
};

