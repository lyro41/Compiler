#include "StructAttribute.h"

StructAttribute::StructAttribute(std::wstring name, bool _is_constant,
                                 bool _is_lr, TID* struct_tid)
    : TypeAttribute(name, _is_constant, _is_lr), struct_tid_(struct_tid) {}

TypeAttribute* StructAttribute::Clone() { return new StructAttribute(*this); }
