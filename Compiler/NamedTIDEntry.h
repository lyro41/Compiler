#pragma once
#include "ITIDEntry.h"
#include "SemanticException.h"
class NamedTIDEntry : public ITIDEntry {
 public:
  NamedTIDEntry() = default;
  NamedTIDEntry(std::wstring name_);
  virtual bool ShouldPush(ITIDEntry* entry) override;
  std::wstring name;
};
