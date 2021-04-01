#pragma once
#include "Attribute.h"
#include "TypeAttribute.h"
class JumpAttribute : public Attribute {
public:
  JumpAttribute(TypeAttribute* return_type);
  TypeAttribute* return_type_ptr;
};
