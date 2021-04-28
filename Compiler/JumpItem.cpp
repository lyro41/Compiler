#include "JumpItem.h"


JumpItem::JumpItem(size_t jump_ind) : jump_ind_(jump_ind)
{
    this->type_ = L"Jump";
}

JumpItem::JumpItem(bool is_offset, size_t jump_ind): jump_offset(is_offset), jump_ind_(jump_ind){}
