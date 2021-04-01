#include "pch.h"
#include "CppUnitTest.h"
#include "LexicAnalyzer/LexicAnalyzer.h"
#include "LexicAnalyzer/LexicAnalyzer.cpp"
#include "LexicAnalyzer/OperatorState.cpp"
#include "LexicAnalyzer/BeginState.cpp"
#include "LexicAnalyzer/IDState.cpp"
#include "LexicAnalyzer/LitConstState.cpp"
#include "LexicAnalyzer/NumberState.cpp"

#include "../Compiler/Token.h"

#include "../Compiler/Parser/Parser.h"
#include "../Compiler/Parser/Parser.cpp"

#include "../Compiler/Semantic/Semantic.h"
#include "../Compiler/Semantic/Semantic.cpp"

#include "../Compiler/TID.h"
#include "../Compiler/TID.cpp"
#include "../Compiler/ITIDEntry.h"
#include "../Compiler/ITIDEntry.cpp"
#include "../Compiler/FunctionTIDEntry.h"
#include "../Compiler/FunctionTIDEntry.cpp"
#include "../Compiler/NamespaceTIDEntry.h"
#include "../Compiler/NamespaceTIDEntry.cpp"
#include "../Compiler/VariableTIDEntry.h"
#include "../Compiler/VariableTIDEntry.cpp"
#include "../Compiler/ImportedFileTIDEntry.h"
#include "../Compiler/ImportedFileTIDEntry.cpp"
#include "../Compiler/NamedTIDEntry.h"
#include "../Compiler/NamedTIDEntry.cpp"
#include "../Compiler/TypeAttribute.h"
#include "../Compiler/TypeAttribute.cpp"

#include "../Compiler/SemanticException.h"
#include "../Compiler/SemanticException.cpp"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ParserUnitTest {
  TEST_CLASS(ParserUnitTest) {
   private:
    const std::wstring tests_directory = L"parser_tests_";
    const std::wstring project_name = L"Compiler";

   public:
    void RunTest(std::wstring path, bool shouldThrow) {
      std::wstring cur_path = std::filesystem::current_path().wstring();
      size_t project_name_offset = cur_path.find(project_name);
      size_t first_backslash = cur_path.find(L'\\', project_name_offset);
      cur_path = cur_path.substr(0, first_backslash + 1) + tests_directory + L"\\";
      std::wifstream file_input(cur_path + path);

      Assert::IsTrue(file_input.is_open());
      LexicAnalyzer* lexer = nullptr;
      try {
        lexer = new LexicAnalyzer(file_input);
      } catch (const std::runtime_error& e) {
        std::string msg = e.what();
        std::wstring wmsg(msg.begin(), msg.end());
        Assert::IsTrue(shouldThrow, wmsg.c_str());
        return;
      }
      TID global_tid;
      SemanticAnalyzer semantic(&global_tid);
      Parser parser(lexer, &semantic);
      try {
        parser.Parse();
      } catch (std::exception& e) {
        std::string msg = e.what();
        std::wstring wmsg(msg.begin(), msg.end());
        Assert::IsTrue(shouldThrow, wmsg.c_str());
        return;
      }
      Assert::IsTrue(!shouldThrow, L"Expected thrown exception");
    }

    TEST_METHOD(Test1) { RunTest(L"1_input.txt", false); }
    TEST_METHOD(Test2) { RunTest(L"2_input.txt", false); }
    TEST_METHOD(Test3) { RunTest(L"3_input.txt", true); }
    TEST_METHOD(Test4) { RunTest(L"4_input.txt", false); }
    TEST_METHOD(Test5) { RunTest(L"5_input.txt", false); } 
    TEST_METHOD(Test_Switch) { RunTest(L"6_input.txt", false); } 
    TEST_METHOD(Test_DoWhile) { RunTest(L"7_input.txt", false); }

    // TEST_METHOD(Debug_Case) { RunTest(L"debug.txt", false); }
  };
}
