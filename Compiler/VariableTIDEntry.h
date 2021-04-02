#pragma once
#include "NamedTIDEntry.h"
#include "TypeAttribute.h"
class VariableTIDEntry : public NamedTIDEntry {
 public:
  VariableTIDEntry(TypeAttribute* type_attr);
  TypeAttribute* type;
  ~VariableTIDEntry();
};

