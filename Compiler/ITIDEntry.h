#pragma once
#include <string>
class TID;
class ITIDEntry {
 public:
  virtual bool ShouldPush(ITIDEntry* entry) = 0;
  virtual TID* GetTID();
};
