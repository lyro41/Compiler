#include <stdio.h>
#include "LexicAnalyzer.h"
#include "Token.h"
#include <fstream>


int main() {
  std::ifstream input("1.txt");
  LexicAnalyzer anal;
  Token tk;
  anal.GetTokens(input);
  return 0;
}