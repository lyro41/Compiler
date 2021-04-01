#pragma once
#include <string>

class ITIDEntry {
 public:
  virtual bool ShouldPush(ITIDEntry* entry) = 0;
};
