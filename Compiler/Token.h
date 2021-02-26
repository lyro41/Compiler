#ifndef TOKEN
#define TOKEN

#include <string>

struct Token {
  enum class Type {
    RESERVED,
    IDENTIFIER,
    NUMCONSTANT,
    LITCONSTANT,
    OPERATOR,
    PUNCTUATION,
    ENDOFFILE
  };
  std::wstring symbol;
  Type type;
};

#endif