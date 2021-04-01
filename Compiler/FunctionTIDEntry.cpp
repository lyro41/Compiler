#include "FunctionTIDEntry.h"

bool FunctionTIDEntry::ShouldPush(ITIDEntry* entry) { 
  FunctionTIDEntry* func = dynamic_cast<FunctionTIDEntry*>(entry);
  if (func) {
    if (func->name != name) return true;
    if (func->type_arguments.size() != type_arguments.size()) return true;
    // std::mismatch return first unmatched pair
    if (std::mismatch(func->type_arguments.begin(), func->type_arguments.end(),
                      type_arguments.begin(), type_arguments.end())
                      .second == type_arguments.end()) {
      if (func->is_proto && !is_proto) {
        throw SemanticException(
            "Function prototype declared after function definition");
      } else {
        if (func->is_proto && is_proto) {
          throw SemanticException("Function prototype re-declared");
        } else {
          if (!func->is_proto && !is_proto) {
            throw SemanticException("Function re-declaration");
          }
          is_proto = false;
          return false;
        }
      }
    }
    return true;
  } else {
    NamedTIDEntry* named = dynamic_cast<NamedTIDEntry*>(entry);
    if (named) {
      if (named->name == name) {
        throw SemanticException("Function with same name already exists");
      }
    } else return true;
  }
  return true;
}
