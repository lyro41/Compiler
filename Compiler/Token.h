#ifndef TOKEN
#define TOKEN

#include <string>

struct Token {
    enum class Type {
        RESERVED,
        IDENTIFIER,
        NUMCONSTANT,
        OPERATOR,
        PUNCTUATION,
        LITCONSTANT
    };
    std::wstring symbol;
    Type type;
};

#endif