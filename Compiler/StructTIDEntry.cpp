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

void StructTIDEntry::SetCurrentTID(TID* _tid) {
  tid_ = _tid;
}

StructTIDEntry::~StructTIDEntry() {
  delete tid_;
}
