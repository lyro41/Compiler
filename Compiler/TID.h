#pragma once

#include "ITIDEntry.h"
#include "NamedTIDEntry.h"
#include <vector>
#include "SemanticException.h"
class TID {
 public:
  bool LocalTIDSearch(ITIDEntry* entry);
  bool DeepTIDSearch(ITIDEntry* entry);
  void PushInTID(ITIDEntry* entry);
  ITIDEntry* FindByName(std::wstring name, bool gettoparent = true);
  template<typename T>
  T* FindByName(std::wstring name);
  std::vector<ITIDEntry*> FindAllByName(std::wstring name);
  /// <summary>
  /// This kills current TID
  /// </summary>
  /// <param name="tid"></param>
  void MergeToTID(TID* tid);

  TID* parent;
  ~TID();
 private:
bool IsConflicting(ITIDEntry* entry);
  std::vector<ITIDEntry*> entries_;
};
