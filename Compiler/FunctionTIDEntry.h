#pragma once
#include "ITIDEntry.h"
#include "NamedTIDEntry.h"
#include <TypeAttribute.h>
#include "SemanticException.h"
#include <vector>
#include <algorithm>
class FunctionTIDEntry : public NamedTIDEntry {
 public:
  virtual bool ShouldPush(ITIDEntry* entry) override;
  std::vector<TypeAttribute> type_arguments;
  std::wstring return_type;
  bool is_proto;
};
