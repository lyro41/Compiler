#pragma once
#include "TypeAttribute.h"
#include "TID.h"
class StructAttribute : public TypeAttribute {
  public: 
  StructAttribute(std::wstring name, bool _is_constant, bool _is_lr, TID* struct_tid);
  virtual TypeAttribute* Clone() override;
  bool is_ptr = false;
  TID* struct_tid_;

};
