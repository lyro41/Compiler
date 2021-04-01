#pragma once
#include "TID.h"
#include "NamedTIDEntry.h"
class NamespaceTIDEntry : public NamedTIDEntry {
 public:
    NamespaceTIDEntry();
    virtual bool ShouldPush(ITIDEntry* entry);
    TID* GetCurrentTID();
    void SetCurrentTID(TID* tid);
    ~NamespaceTIDEntry();
 private:
    TID* tid_;

};
