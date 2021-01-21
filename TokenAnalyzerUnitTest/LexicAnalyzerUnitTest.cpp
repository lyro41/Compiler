#include "pch.h"
#include <filesystem>
#include <fstream>
#include <queue>
#include <sstream>
#include <string>

#include "..\Compiler\LexicAnalyzer.cpp"
#include "..\Compiler\LexicAnalyzer.h"
#include "..\Compiler\Token.h"
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LexicAnalyzerUnitTest {

TEST_CLASS(LexicAnalyzerUnitTest) {
 public:
  const std::string tests_directory = "lexic_analyzer_tests_";
  TEST_METHOD(Test1) {
    std::string currentPath = std::filesystem::current_path().string();
    if (currentPath.find("Debug") != std::string::npos) {
      currentPath = currentPath.substr(0, currentPath.find("Debug"));
    } 
    currentPath += "\\" + tests_directory + "\\";
    std::ifstream file_input(currentPath + "1_input.txt");
    LexicAnalyzer analyzer;
    std::queue<Token> actual_tokens = analyzer.GetTokens(file_input);
    std::ifstream file_expected(currentPath + "1_expected.txt");
    Assert::IsTrue(file_input.is_open() && file_expected.is_open(),
                   std::filesystem::current_path().c_str());
    std::string line;
    while (std::getline(file_expected, line)) {
      Assert::IsTrue(!actual_tokens.empty(), L"QUEUE IS EMPTY");
      Token actual_token = actual_tokens.front();
      Assert::IsTrue(line.compare(std::string(std::to_string((int)actual_token.type)) +
                       " " + actual_token.symbol), L"TOKENS DO NOT MATCH");
      actual_tokens.pop();
    }
  }
};

}  // namespace LexicAnalyzerUnitTest
