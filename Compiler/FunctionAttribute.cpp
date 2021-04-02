#include "FunctionAttribute.h"

FunctionAttribute::FunctionAttribute(std::wstring _name) : TypeAttribute(L"CFUNC", false, false), name(_name) {}
