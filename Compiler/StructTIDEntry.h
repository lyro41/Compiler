#pragma once
#include "ITIDEntry.h"
#include "NamedTIDEntry.h"
#include "TID.h"
class StructTIDEntry : public NamedTIDEntry {
  public:
  StructTIDEntry();
  virtual bool ShouldPush(ITIDEntry* entry) override;
  virtual TID* GetTID() override;
  void SetCurrentTID(TID* _tid);
  TID* GetCurrentTID();
  ~StructTIDEntry();
  private:
  TID* tid_;
};
