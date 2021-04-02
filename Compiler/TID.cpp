#include "TID.h"

bool TID::LocalTIDSearch(ITIDEntry* entry) {
  try {
    return !IsConflicting(entry);
  } catch (SemanticException&) {
    return false;
  }
  return true;
}

bool TID::DeepTIDSearch(ITIDEntry* entry) {
  if(LocalTIDSearch(entry)) return true;
  if (parent) return parent->DeepTIDSearch(entry);
  return true;
}

void TID::PushInTID(ITIDEntry* entry) {
  if(!IsConflicting(entry))
    entries_.push_back(entry);
}

ITIDEntry* TID::FindByName(std::wstring name) { 
  NamedTIDEntry named(name);
  for (auto& e : entries_) {
    try {
      if(!named.ShouldPush(e)) return e;
    } catch (SemanticException&) {
      return e;
    }
  }
  if (parent) {
    return parent->FindByName(name);
  }
  return nullptr;
}
template <typename T>
T* TID::FindByName(std::wstring name) {
  NamedTIDEntry named(name);
  for (auto& e : entries_) {
    try {
      if (!named.ShouldPush(e)) { 
        if (dynamic_cast<T*>(e))
          return e;
      };
    } catch (SemanticException&) {
      if (dynamic_cast<T*>(e)) return e;
    }
  }
  if (parent) {
    return parent->FindByName(name);
  }
  return nullptr;
}
std::vector<ITIDEntry*> TID::FindAllByName(std::wstring name) {
  NamedTIDEntry named(name);
  std::vector<ITIDEntry*> list;
  for (auto& e : entries_) {
    try {
      if (!named.ShouldPush(e)) list.push_back(e);
    } catch (SemanticException&) {
      list.push_back(e);
    }
  }
  if (parent) {
    std::vector<ITIDEntry*> p_list =
        parent->FindAllByName(name);
    list.insert(list.end(), p_list.begin(), p_list.end());
  }
  return list;
}

void TID::MergeToTID(TID* tid) {
  for (auto& e : entries_) {
    tid->PushInTID(e);
  }
  entries_.clear();
}

TID::~TID() {
  for (auto& e: entries_) {
    delete e;
  }
}

bool TID::IsConflicting(ITIDEntry* entry) {
  for (auto& e : entries_) {
    if (!e->ShouldPush(entry)) return true;
  }
  return false;
}
