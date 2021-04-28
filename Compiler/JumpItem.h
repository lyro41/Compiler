#pragma once
#include "Item.h"
class JumpItem :
    public Item
{
public:
    bool jump_offset = false;
    JumpItem(size_t jump_ind = -1);
    JumpItem(bool is_offset, size_t jump_ind = -1);
    size_t jump_ind_ = -1;
};

