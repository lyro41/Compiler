#pragma once
#include "Attribute.h"
#include <string>
class FunctionAttribute : public Attribute {
  public:
  FunctionAttribute(std::wstring _name);
  std::wstring name;
};
