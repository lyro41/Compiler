#include "VariableTIDEntry.h"

VariableTIDEntry::VariableTIDEntry(TypeAttribute* type_attr)
    : type(type_attr) {}

VariableTIDEntry::~VariableTIDEntry() { delete type; }
