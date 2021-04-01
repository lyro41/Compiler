#pragma once
#include "ITIDEntry.h"

class ImportedFileTIDEntry : public ITIDEntry {
 public:
  virtual bool ShouldPush(ITIDEntry* entry) override;
  std::wstring file_name;
};
