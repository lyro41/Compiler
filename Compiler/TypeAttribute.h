#pragma once
#include "Attribute.h"
#include <string>

class TypeAttribute : public Attribute {
 public:
  TypeAttribute(std::wstring _type, bool _is_constant, bool is_lr);
  virtual TypeAttribute* Clone();
  std::wstring type;
  bool is_lrvalue = false;
  bool is_constant = false;
  bool operator==(const TypeAttribute& rhs) const {
    return rhs.type == type && rhs.is_constant == is_constant;
  }
};
