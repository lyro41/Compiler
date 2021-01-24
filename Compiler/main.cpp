#include <stdio.h>
#include <fstream>
#include <iostream>

#include "LexicAnalyzer.h"
#include "Token.h"


int main() {
  std::wifstream in("D:\\Coding\\Compiler\\lexic_analyzer_tests_");
  LexicAnalyzer anal(in);
  try {
    anal.GetTokens();
  } catch (std::runtime_error& e) {
    std::cout << e.what();
  }
  return 0;
}