#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>

#include "LexicAnalyzer.h"
#include "Token.h"


int main(int argc, const char* argv[]) {
  if (argc < 2) {
    std::cout << "Analyzed file is not defined!\n";
    std::cin.get();
    return -1;
  }

  std::wstring token_type[] = {
        L"RESERVED", 
        L"IDENTIFIER", 
        L"NUMERIC_CONSTANT", 
        L"LITERAL_CONSTANT", 
        L"OPERATOR", 
        L"PUNCTUATION"
  };

  std::wifstream file_input(argv[1]);
  if (!file_input.is_open()) {
    std::cout << "Unable to open analyzed file\n";
    std::cin.get();
    return -1;
  }

  LexicAnalyzer* analyzer;
  try {
    analyzer = new LexicAnalyzer(file_input);
  } catch (const std::runtime_error& e) {
    std::cout << "Error accured during initialization of lexic analyzer\n";
    std::cout << e.what() << "\n";
    std::cin.get();
    return -1;
  }

  std::string file_name = argv[1];
  auto file_name_offset = file_name.find_last_of('\\');
  if (file_name_offset == std::string::npos) file_name_offset = 0;
  auto file_name_extension_offset = file_name.find_first_of('.',
                                                            file_name_offset);
  std::string file_extension = file_name.substr(file_name_extension_offset);
  file_name = file_name.substr(file_name_offset,
                               file_name_offset - file_name_extension_offset);

  std::queue<Token> tokens;
  try {
    tokens = analyzer->GetTokens(); 
  } catch (const std::runtime_error& e) {
    std::cout << "Error accured during lexing\n";
    std::cout << e.what() << "\n";
    std::cin.get();
    return -1;
  }
  file_input.close();

  std::wfstream file_output("output_tokens.txt",
                            std::ios::out | std::ios::app);
  if (!file_output.is_open()) {
    std::cout << "Unable to open output stream\n";
    std::cin.get();
  }

  while (!tokens.empty()) {
    Token cur_token = tokens.front();
    std::wstring line;
    line += token_type[static_cast<int>(cur_token.type)] +
            L" " + cur_token.symbol + L"\n";
    file_output << line;
    tokens.pop();
  }
  file_output.close();
  delete analyzer;
  return 0;
}