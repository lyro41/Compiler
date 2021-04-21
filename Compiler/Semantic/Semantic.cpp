#include "Semantic.h"
#include "LexicAnalyzer/LexicAnalyzer.h"
#include "Parser/Parser.h"


SemanticAnalyzer::SemanticAnalyzer(TID* global_tid, std::wstring file_path)
    : global_(global_tid), current_(global_tid), file_path_(file_path) {}

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
  if (global_->LocalTIDSearch(entry)) {
    global_->PushInTID(entry);
    std::wifstream file_stream(file_path_ + L'/' + symb);
    if (!file_stream.is_open()) {
      throw SemanticException(L"Unable to open " + symb + L" file");
    }
    LexicAnalyzer lexer(file_stream);
    SemanticAnalyzer semantic(global_, file_path_ + L'/' + symb);
    Parser parser(&lexer,&semantic);
    parser.Parse();
  }
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

void SemanticAnalyzer::CheckPrototypes() {
  for (auto& func : called_prototypes ) {
    if (func->is_proto) throw SemanticException((L"Unable to find implementation of prototype "  + func->name));
  }
}

void SemanticAnalyzer::ResolveGotos() {
  for (auto& o_goto : unresolved_gotos) {
    auto found_iter = defined_labels.find(o_goto);
    if (found_iter == defined_labels.end()) {
      throw SemanticException(o_goto + L" label is not defined in current function scope");
    }
  }
  unresolved_gotos.clear();
  defined_labels.clear();
}

void SemanticAnalyzer::AddGotoLabel(std::wstring label) {
  auto label_found = defined_labels.find(label);
  if (label_found != defined_labels.end()) { 
    throw SemanticException(L"\"" + label + L"\"" L" label is already defined");
  }
  defined_labels.insert(label);
}

void SemanticAnalyzer::AddGotoCall(std::wstring call) {
  unresolved_gotos.insert(call);
}
