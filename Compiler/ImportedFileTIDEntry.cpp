#include "ImportedFileTIDEntry.h"

bool ImportedFileTIDEntry::ShouldPush(ITIDEntry* entry) { 
  ImportedFileTIDEntry* file_entry = dynamic_cast<ImportedFileTIDEntry*>(entry);
  if (file_entry) {
    if (file_entry->file_name == file_name) {
      return false;
    }
  }
  return true;
}
