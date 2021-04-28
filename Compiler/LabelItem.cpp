#include "LabelItem.h"

LabelItem::LabelItem()
{
    this->type_ = L"Label";
}

LabelItem::LabelItem(std::wstring name) {
    this->type_ = L"Label";
    this->name_ = name;
}
