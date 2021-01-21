#pragma once
#pragma once
#include <fstream>
#include <queue>
#include <deque>

#include "Token.h"

class LexicAnalyzer {
 public:
  std::queue<Token> GetTokens(const std::ifstream& input);
};
