#include "pch.h"
#include "CppUnitTest.h"
#include "LexicAnalyzer/LexicAnalyzer.h"
#include "LexicAnalyzer/LexicAnalyzer.cpp"
#include "LexicAnalyzer\OperatorState.cpp"
#include "LexicAnalyzer\BeginState.cpp"
#include "LexicAnalyzer\IDState.cpp"
#include "LexicAnalyzer\LitConstState.cpp"
#include "LexicAnalyzer\NumberState.cpp"

#include "../Compiler/Token.h"
#include "../Compiler/Parser.h"
#include "../Compiler/Parser.cpp"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ParserUnitTest
{
	TEST_CLASS(ParserUnitTest)
	{
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
      LexicAnalyzer* analyzer = nullptr;
      try {
        analyzer = new LexicAnalyzer(file_input);
      } catch (const std::runtime_error& e) {
        std::string msg = e.what();
        std::wstring wmsg(msg.begin(), msg.end());
        Assert::IsTrue(shouldThrow, wmsg.c_str());
        return;
        std::cout << "Error accured during initialization of lexic analyzer\n";
        std::cout << e.what() << "\n";
      }
      
      Parser parser(analyzer);
      try {
        parser.Parse();
      } catch (std::runtime_error& e) {
        std::string msg = e.what();
        std::wstring wmsg(msg.begin(), msg.end());
        Assert::IsTrue(shouldThrow, wmsg.c_str());
        return;
        std::cout << e.what() << "\n";
      }
      Assert::IsTrue(!shouldThrow, L"Expected thrown exception");
		}
		TEST_METHOD(Test1) {
      RunTest(L"1_input.txt", false);
		}
    TEST_METHOD(Test2) {
      RunTest(L"2_input.txt", false);
		}
    TEST_METHOD(Test3) {
      RunTest(L"3_input.txt", true);
		}
    TEST_METHOD(Test4) { RunTest(L"4_input.txt", false); }
	};
}
