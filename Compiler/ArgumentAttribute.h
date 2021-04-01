#pragma once
#include <string>
#include "TypeAttribute.h"
#include "Attribute.h"
class ArgumentAttribute : public Attribute {
 public:
  ArgumentAttribute(std::wstring name, TypeAttribute* copy_from);
  std::wstring name;
  TypeAttribute type;
};
