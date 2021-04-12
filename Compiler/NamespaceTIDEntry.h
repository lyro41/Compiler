#pragma once
#include "TID.h"
#include "NamedTIDEntry.h"
class NamespaceTIDEntry : public NamedTIDEntry {
 public:
    NamespaceTIDEntry();
    virtual bool ShouldPush(ITIDEntry* entry);
    virtual TID* GetTID() override;
    TID* GetCurrentTID();
    void SetCurrentTID(TID* tid);
    ~NamespaceTIDEntry();
 private:
    TID* tid_;

};
