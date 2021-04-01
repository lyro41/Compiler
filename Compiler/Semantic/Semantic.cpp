#include "Semantic.h"

SemanticAnalyzer::SemanticAnalyzer(TID* global_tid) : global_(global_tid), current_(global_tid) {}

bool SemanticAnalyzer::IsNumberInt(std::wstring symb) { 
  if (symb.find(L'.') == std::wstring::npos &&
      symb.find(L'e') == std::wstring::npos) {
    return true;
  }
  return false;
}

bool SemanticAnalyzer::IsNumberFloat(std::wstring symb) {
  if (symb.find(L'.') != std::wstring::npos ||
      symb.find(L'e') != std::wstring::npos) {
    return true;
  }
  return false;
}

bool SemanticAnalyzer::IsNumberHex(std::wstring symb) {
  return symb.find(L'x') != std::wstring::npos;
}

void SemanticAnalyzer::LocalCurrentTIDSearch(std::wstring symb) {
  NamedTIDEntry* entry = new NamedTIDEntry(symb);
  current_->LocalTIDSearch(entry);
  delete entry;
}

void SemanticAnalyzer::DeepCurrentTIDSearch(std::wstring symb) {
  NamedTIDEntry* entry = new NamedTIDEntry(symb);
  current_->DeepTIDSearch(entry);
  delete entry;
}

void SemanticAnalyzer::PushInCurrentTID(ITIDEntry* entry) {
  current_->PushInTID(entry);
}

void SemanticAnalyzer::PushInGlobalTID(ITIDEntry* entry) {
  global_->PushInTID(entry);
}


void SemanticAnalyzer::PushAttribute(Attribute* attr) {
  attribute_stack_.push(attr);
}

void SemanticAnalyzer::PopAttribute() {
  delete attribute_stack_.top();
  attribute_stack_.pop();
}

bool SemanticAnalyzer::AttributeStackEmpty() { return attribute_stack_.empty(); }

Attribute* SemanticAnalyzer::TopStackAttribute() {
  return attribute_stack_.top();
}

void SemanticAnalyzer::TryToParseFile(std::wstring symb) {
  // TO DO
  ImportedFileTIDEntry* entry = new ImportedFileTIDEntry();
  entry->file_name = symb;
  global_->PushInTID(entry);
}

void SemanticAnalyzer::StartNamespaceParse() {
  tid_prev_ = current_; }

void SemanticAnalyzer::EndNamespaceParse() {
  current_ = tid_prev_;
}

void SemanticAnalyzer::CreateNewTID() { 
  TID* newTid = new TID();
  newTid->parent = current_;
  current_ = newTid;
}

void SemanticAnalyzer::RemoveCurrentTID() {
  if (current_ == global_) {
    throw SemanticException("Attempted to remove global TID. Bad!");
  }
  TID* cur = current_;
  current_ = cur->parent;
  delete cur;
}

void SemanticAnalyzer::DisconnectCurrentTID() {
  current_ = current_->parent; }

void SemanticAnalyzer::ChangeCurrentTID(TID* target) {
  current_ = target;
}

TID* SemanticAnalyzer::GetCurrentTID() { return current_; }

TID* SemanticAnalyzer::GetGlobalTID() { return global_; }
