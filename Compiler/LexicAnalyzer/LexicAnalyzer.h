#ifndef LEXICANALYZER
#define LEXICANALYZER

#include <fstream>
#include <queue>
#include <string>
#include <set>
#include <map>
#include <exception>

#include "Token.h"
#include "IState.h"
#include "BeginState.h"
#include "OperatorState.h"
#include "IDState.h"
#include "LitConstState.h"
#include "NumberState.h"

class LexicAnalyzer {
 public:
  LexicAnalyzer(std::wifstream& input_stream);

  void ChangeState(IState* state);
  wchar_t Peek();
  void SkipChar();
  void SkipLine();
  bool HasNext();
  bool HasNextToken();

  std::pair<size_t, size_t> GetCursorPosition();

  void AddNextCharToBuffer();
  void AddCharToBuffer(wchar_t symbol);
  void AddBufferToQueue(Token::Type token_type);

  std::queue<Token> GetTokens();
  Token GetToken();
  BeginState* GetBeginState();
  OperatorState* GetOperatorState();
  IDState* GetIDState();
  LitConstState* GetLitConstState();
  NumberState* GetNumberState();
  std::wstring GetBuffer();
  void SetBuffer(std::wstring string);

  bool IsPunctuation(wchar_t symbol);
  bool IsOperator(std::wstring string);
  bool IsReserved(std::wstring string);
  wchar_t ToControl(wchar_t symbol);

  void ThrowException(const char* message);

 private:
  void Run();


  size_t current_line_;
  size_t current_character_;

  std::set<std::wstring> reserved_;
  std::set<std::wstring> operators_;
  std::set<std::wstring> punctuation_;
  std::map<wchar_t, wchar_t> backslashes_;

  std::wifstream& input_stream_;
  std::wstring token_buffer_;
  std::queue<Token> current_token_queue_;
  BeginState begin_state_;
  OperatorState operator_state_;
  IDState id_state_;
  LitConstState lit_const_state_;
  NumberState number_state_;
  IState* current_state_;
};

#endif