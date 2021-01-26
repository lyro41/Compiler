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
    PUNCTUATION
  };
  std::wstring symbol;
  Type type;
};

#endif