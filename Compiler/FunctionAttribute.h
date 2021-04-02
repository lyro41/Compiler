#pragma once
#include "Attribute.h"
#include "TypeAttribute.h"
#include <string>
class FunctionAttribute : public TypeAttribute {
  public:
  FunctionAttribute(std::wstring _name);
  std::wstring name;
};
