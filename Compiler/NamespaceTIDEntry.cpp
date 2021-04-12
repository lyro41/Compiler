#include "NamespaceTIDEntry.h"

NamespaceTIDEntry::NamespaceTIDEntry() {}

bool NamespaceTIDEntry::ShouldPush(ITIDEntry* entry) { 
  NamespaceTIDEntry* nspace =
      dynamic_cast<NamespaceTIDEntry*>(entry);
  if (nspace) {
    if (name == nspace->name) {
      nspace->GetCurrentTID()->MergeToTID(tid_);
      return false;
    }
    return true;
  }

  NamedTIDEntry* named = dynamic_cast<NamespaceTIDEntry*>(entry);
  if (named) {
    if (named->name == name)
      throw SemanticException("Namespace with same name already exists");
  }
  return true;
}

TID* NamespaceTIDEntry::GetCurrentTID() { return tid_; }

TID* NamespaceTIDEntry::GetTID() { return GetCurrentTID(); }

void NamespaceTIDEntry::SetCurrentTID(TID* tid) {
  tid_ = tid;
  tid->parent = nullptr;
}

NamespaceTIDEntry::~NamespaceTIDEntry() {
  delete tid_;
}
