#include "NamedTIDEntry.h"

NamedTIDEntry::NamedTIDEntry(std::wstring name_) : name(name_) {}

bool NamedTIDEntry::ShouldPush(ITIDEntry* entry) {
  NamedTIDEntry* named = dynamic_cast<NamedTIDEntry*>(entry);
  if (named) {
    if (named->name == name) {
      throw SemanticException("Definition with same name already exists");
    }
  }
  return true;
}
