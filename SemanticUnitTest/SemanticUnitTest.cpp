#include "pch.h"
#include "CppUnitTest.h"
#include <filesystem>
#include "../Compiler/LexicAnalyzer/LexicAnalyzer.h"
#include "../Compiler/LexicAnalyzer/LexicAnalyzer.cpp"
#include "../Compiler/LexicAnalyzer/OperatorState.cpp"
#include "../Compiler/LexicAnalyzer/BeginState.cpp"
#include "../Compiler/LexicAnalyzer/IDState.cpp"
#include "../Compiler/LexicAnalyzer/LitConstState.cpp"
#include "../Compiler/LexicAnalyzer/NumberState.cpp"

#include "../Compiler/Token.h"

#include "../Compiler/Parser/Parser.h"
#include "../Compiler/Parser/Parser.cpp"

#include "../Compiler/Semantic/Semantic.h"
#include "../Compiler/Semantic/Semantic.cpp"
#include "../Compiler/Attribute.h"
#include "../Compiler/Attribute.cpp"
#include "../Compiler/TypeAttribute.h"
#include "../Compiler/TypeAttribute.cpp"
#include "../Compiler/ArgumentAttribute.h"
#include "../Compiler/ArgumentAttribute.cpp"
#include "../Compiler/FunctionAttribute.h"
#include "../Compiler/FunctionAttribute.cpp"
#include "../Compiler/JumpAttribute.h"
#include "../Compiler/JumpAttribute.cpp"
#include "../Compiler/StructAttribute.h"
#include "../Compiler/StructAttribute.cpp"

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
#include "../Compiler/StructTIDEntry.h"
#include "../Compiler/StructTIDEntry.cpp"

#include "../Compiler/SemanticException.h"
#include "../Compiler/SemanticException.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SemanticUnitTest
{
	TEST_CLASS(SemanticUnitTest)
	{
	public:
		
		private:
      const std::wstring tests_directory = L"semantic_tests_";
      const std::wstring project_name = L"Compiler";

    public:
    void RunTest(std::wstring path, bool shouldThrow) {
      std::wstring cur_path = std::filesystem::current_path().wstring();
      size_t project_name_offset = cur_path.find(project_name);
      size_t first_backslash = cur_path.find(L'\\', project_name_offset);
      cur_path = cur_path.substr(0, first_backslash + 1) +
                 tests_directory + L"\\";
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

    TEST_METHOD(Test_FunctionPrototype) {
      RunTest(L"1_input.txt", false);
    }
    TEST_METHOD(Test_FunctionRedeclare) {
      RunTest(L"2_input.txt", true);
    }
    TEST_METHOD(Test_FunctionOverload) {
      RunTest(L"3_input.txt", false);
    }
    TEST_METHOD(Test_Namespace) {
      RunTest(L"4_input.txt", false);
    }
    TEST_METHOD(Test_VariableRedefinition) {
      RunTest(L"5_input.txt", true);
    }
    TEST_METHOD(Test_NestedNamespace) { RunTest(L"6_input.txt", false); }
    TEST_METHOD(Test_GlobalNamespace) { RunTest(L"7_input.txt", false); }
    TEST_METHOD(Test_Bug) { RunTest(L"bug_input.txt", false); }
    TEST_METHOD(Test_AssignmentOperators) { RunTest(L"9_input.txt", false); }
    TEST_METHOD(TestExc_AmbigiousFunctionCall) { RunTest(L"10_input.txt", true); }
    TEST_METHOD(Test_Recursion) { RunTest(L"11_input.txt", false); } 
    TEST_METHOD(TestExc_WrongFunctionCall) { RunTest(L"12_input.txt", true); }
    TEST_METHOD(TestExc_UnimplementedPrototype) { RunTest(L"13_input.txt", true); }
    TEST_METHOD(TestExc_ContinueFromNonloop) { RunTest(L"14_input.txt", true); }
    TEST_METHOD(TestExc_BreakFromNonloop) { RunTest(L"15_input.txt", true); }
	  TEST_METHOD(Test_ForLoopBreak) { RunTest(L"16_input.txt", false); }
    TEST_METHOD(Test_WhileLoopBreak) { RunTest(L"17_input.txt", false); } 
    
    TEST_METHOD(Complex_1) { RunTest(L"complex_input.txt", false); }
	};
}
