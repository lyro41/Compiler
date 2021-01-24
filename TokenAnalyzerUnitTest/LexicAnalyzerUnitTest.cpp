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
#include "..\Compiler\OperatorState.cpp"
#include "..\Compiler\BeginState.cpp"
#include "..\Compiler\IDState.cpp"
#include "..\Compiler\LitConstState.cpp"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LexicAnalyzerUnitTest {

TEST_CLASS(LexicAnalyzerUnitTest) {
 public:
  const std::wstring tests_directory = L"lexic_analyzer_tests_";
  const std::wstring project_name = L"Compiler";

  void RunTest(std::wstring input_filename, std::wstring expected_filename) {
      std::wstring cur_path = std::filesystem::current_path().wstring();
      size_t project_name_offset = cur_path.find(project_name);
      size_t first_backslash = cur_path.find(L'\\', project_name_offset);
      cur_path = cur_path.substr(0, first_backslash + 1) + 
                 tests_directory + L"\\";

      std::wifstream file_input(cur_path + input_filename);
      std::wifstream file_expected(cur_path + expected_filename);

      LexicAnalyzer analyzer(file_input);
      
      std::queue<Token> actual_tokens;
      actual_tokens = analyzer.GetTokens(); 

      Assert::IsTrue(file_input.is_open() && file_expected.is_open(),
                     L"UNABLE TO OPEN INPUT OR/AND EXPECTED FILE(S)"
                     L"\nCHECK IF THEY ARE IN TESTS DIRECTORY");

      std::wstring line;
      while (std::getline(file_expected, line)) {
          Assert::IsTrue(!actual_tokens.empty(), L"QUEUE IS EMPTY");

          Token actual_token = actual_tokens.front();
          Assert::IsTrue(line.compare(std::to_wstring((int)actual_token.type) +
                         L" " + actual_token.symbol), L"TOKENS DO NOT MATCH");
          actual_tokens.pop();
      }
      Assert::IsTrue(actual_tokens.empty(),
                     L"QUEUE IS NOT EMPTY AFTER TESTING");
  }
  TEST_METHOD(Operator_1) {
    RunTest(L"operators/1_input.txt", L"operators/1_expected.txt");
  }
  TEST_METHOD(Operator_2) {
    RunTest(L"operators/2_input.txt", L"operators/2_expected.txt");
  }
  TEST_METHOD(Operator_3) {
    RunTest(L"operators/3_input.txt", L"operators/3_expected.txt");
  }
  TEST_METHOD(Comments_1) {
    RunTest(L"comments/1_input.txt", L"comments/1_expected.txt");
  }

  TEST_METHOD(Id_1) {
    RunTest(L"id/1_input.txt", L"id/1_expected.txt");
  }
  
  TEST_METHOD(Full_2) {
    RunTest(L"full/2_input.txt", L"full/2_expected.txt");
  }
};

}  // namespace LexicAnalyzerUnitTest
