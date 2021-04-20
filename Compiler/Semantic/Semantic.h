#pragma once
#include "ImportedFileTIDEntry.h"
#include "NamedTIDEntry.h"
#include "FunctionTIDEntry.h"
#include "TID.h"
#include "Attribute.h"
#include "TypeAttribute.h"
#include <fstream>
#include <stack>
#include <set>

class LexicAnalyzer;
class Parser;

class SemanticAnalyzer {
 public:
  SemanticAnalyzer(TID* global_tid, std::wstring file_input);
  bool IsNumberInt(std::wstring symb);
  bool IsNumberFloat(std::wstring symb);
  bool IsNumberHex(std::wstring symb);

  void LocalCurrentTIDSearch(std::wstring symb);
  void DeepCurrentTIDSearch(std::wstring symb);

  void PushInCurrentTID(ITIDEntry* entry);
  void PushInGlobalTID(ITIDEntry* entry);
  
  void PushAttribute(Attribute* attr);
  void PopAttribute();
  bool AttributeStackEmpty();
  Attribute* TopStackAttribute();

  void TryToParseFile(std::wstring symb);
  
  void StartNamespaceParse();
  void EndNamespaceParse();

  void CreateNewTID();
  void RemoveCurrentTID();
  /// <summary>
  /// Sets current TID to current`s parent
  /// Memory leaky
  /// </summary>
  void DisconnectCurrentTID();
  /// <summary>
  /// Sets current TID to target
  /// Memory leaky
  /// </summary>
  void ChangeCurrentTID(TID* target);
  TID* GetCurrentTID();
  TID* GetGlobalTID();
  void CheckPrototypes();


  void ResolveGotos();
  void AddGotoLabel(std::wstring label);
  void AddGotoCall(std::wstring label);

  std::vector<FunctionTIDEntry*> called_prototypes;
 private:
  TID* current_;
  TID* global_;
  TID* tid_prev_;
  std::stack<Attribute*> attribute_stack_;

  std::wstring file_path_;

  std::set<std::wstring> unresolved_gotos;
  std::set<std::wstring> defined_labels;
};