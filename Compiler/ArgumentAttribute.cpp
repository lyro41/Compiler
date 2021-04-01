#include "ArgumentAttribute.h"

ArgumentAttribute::ArgumentAttribute(std::wstring name_,
                                     TypeAttribute* copy_from_) : type(*copy_from_) {
  name = name_;
}
