#include "pch.h"
#include <filesystem>
#include <fstream>
#include <queue>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>

#include "..\Compiler\LexicAnalyzer.cpp"
#include "..\Compiler\LexicAnalyzer.h"
#include "..\Compiler\Token.h"
#include "..\Compiler\OperatorState.cpp"
#include "..\Compiler\BeginState.cpp"
#include "..\Compiler\IDState.cpp"
#include "..\Compiler\LitConstState.cpp"
#include "..\Compiler\NumberState.cpp"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LexicAnalyzerUnitTest {

TEST_CLASS(LexicAnalyzerUnitTest) {
 public:
  const std::wstring tests_directory = L"lexic_analyzer_tests_";
  const std::wstring project_name = L"Compiler";

  std::map<Token::Type, std::wstring> token_names {
      { Token::Type::RESERVED, L"RESERVED" },
      { Token::Type::IDENTIFIER, L"IDENTIFIER" }, 
      { Token::Type::NUMCONSTANT, L"NUMERIC_CONSTANT"},
      { Token::Type::OPERATOR, L"OPERATOR" },
      { Token::Type::PUNCTUATION, L"PUNCTUATION" },
      { Token::Type::LITCONSTANT, L"LITERAL_CONSTANT" }
  };

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
          Assert::IsTrue(line.compare(token_names[actual_token.type] + L" " 
                         + actual_token.symbol) == 0, L"TOKENS DO NOT MATCH");
          actual_tokens.pop();
      }
      Assert::IsTrue(actual_tokens.empty(),
                     L"QUEUE IS NOT EMPTY AFTER TESTING");
  }

  void RunExceptionTest(std::wstring input_filename) {
    bool caught = false;
    try {
      RunTest(input_filename, input_filename);
    } catch (std::runtime_error& e) {
      caught = true;
    }
    Assert::IsTrue(caught);
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
  

  TEST_METHOD(Numbers_1) {
    RunTest(L"numbers/1_input.txt", L"numbers/1_expected.txt");
  }

  TEST_METHOD(Numbers_2) {
    RunTest(L"numbers/2_input.txt", L"numbers/2_expected.txt");
  }

  TEST_METHOD(Numbers_3) {
    RunTest(L"numbers/3_input.txt", L"numbers/3_expected.txt");
  }

  TEST_METHOD(Numbers_4_exception) {
    RunExceptionTest(L"numbers/4_input.txt");
  }

  TEST_METHOD(Full_1) { 
    RunTest(L"full/1_input.txt", L"full/1_expected.txt"); 
  }

  TEST_METHOD(Full_2) {
    RunTest(L"full/2_input.txt", L"full/2_expected.txt");
  }

  TEST_METHOD(Full_3_exception) { 
    RunExceptionTest(L"full/3_input.txt");
  }

  TEST_METHOD(Full_4) { 
    RunTest(L"full/4_input.txt", L"full/4_expected.txt");
  }
};

}  // namespace LexicAnalyzerUnitTest
