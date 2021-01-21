#pragma once
#include <string>

struct Token {
    enum class Type {
        RESERVED,
        IDENTIFIER,
        CONSTANT,
        OPERATOR,
        PUNCTUATION,
        UNKNOWN
    };
    std::string symbol;
    Type type;
};

