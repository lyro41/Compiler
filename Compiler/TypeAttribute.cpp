#include "TypeAttribute.h"

TypeAttribute::TypeAttribute(std::wstring _type, bool _is_constant, bool is_lr)
    : type(_type), is_constant(_is_constant), is_lrvalue(is_lr) {}

TypeAttribute* TypeAttribute::Clone() { return new TypeAttribute(*this); }
