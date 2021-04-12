#include "StructTIDEntry.h"

StructTIDEntry::StructTIDEntry() {}

bool StructTIDEntry::ShouldPush(ITIDEntry* entry) {
  StructTIDEntry* str = dynamic_cast<StructTIDEntry*>(entry);
  if (str) {
    if (name == str->name) {
      throw SemanticException("Struct redeclaration");
    }
    return true;
  }

  NamedTIDEntry* named = dynamic_cast<NamedTIDEntry*>(entry);
  if (named) {
    if (named->name == name)
      throw SemanticException("Struct with same name already exists");
  }
  return true;
}

TID* StructTIDEntry::GetTID() { return GetCurrentTID(); }

void StructTIDEntry::SetCurrentTID(TID* _tid) {
  tid_ = _tid; }

TID* StructTIDEntry::GetCurrentTID() { return tid_; }

StructTIDEntry::~StructTIDEntry() {
  delete tid_;
}
