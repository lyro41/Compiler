#include "pch.h"
#include <filesystem>
#include <fstream>
#include <queue>
#include <sstream>
#include <string>
#include <algorithm>
#include "..\Compiler\LexicAnalyzer.cpp"
#include "..\Compiler\LexicAnalyzer.h"
#include "..\Compiler\Token.h"
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LexicAnalyzerUnitTest {

TEST_CLASS(LexicAnalyzerUnitTest) {
 public:
  const std::wstring tests_directory = L"lexic_analyzer_tests_";
  const std::wstring project_name = L"Compiler";
  void RunTest(std::wstring input_filename, std::wstring expected_filename)
  {
      std::wstring currentPath = std::filesystem::current_path().wstring();
      size_t project_name_offset = currentPath.find(project_name);
      size_t first_backslash = currentPath.find_first_of('\\', project_name_offset);
      currentPath = currentPath.substr(0, first_backslash + 1) + tests_directory + L"\\";
      std::ifstream file_input(currentPath + input_filename);
      LexicAnalyzer analyzer;
      std::queue<Token> actual_tokens = analyzer.GetTokens(file_input);
      std::ifstream file_expected(currentPath + expected_filename);
      Assert::IsTrue(file_input.is_open() && file_expected.is_open(),
                     L"UNABLE TO OPEN INPUT OR/AND EXPECTED FILE(S)\nCHECK IF THEY ARE IN TESTS DIRECTORY");
      std::string line;
      while (std::getline(file_expected, line)) {
          Assert::IsTrue(!actual_tokens.empty(), L"QUEUE IS EMPTY");
          Token actual_token = actual_tokens.front();
          Assert::IsTrue(line.compare(std::string(std::to_string((int)actual_token.type)) +
                         " " + actual_token.symbol), L"TOKENS DO NOT MATCH");
          actual_tokens.pop();
      }
  }

  TEST_METHOD(Test1) {
    RunTest(L"1_input.txt", L"1_expected.txt");
  }
};

}  // namespace LexicAnalyzerUnitTest
