#include "LexicAnalyzer.h"

LexicAnalyzer::LexicAnalyzer(std::wifstream& input_stream) : 
      input_stream_(input_stream),
      token_buffer_(L""),
      begin_state_(this),
      operator_state_(this),
      id_state_(this),
      lit_const_state_(this),
      number_state_(this),
      current_state_(&begin_state_) {
  std::wifstream list_ifstream;

  #pragma region OPERATORS
  list_ifstream.open("lists/operators.txt");
  if (!list_ifstream.is_open()) {
    throw std::runtime_error(
        "exception thrown: unable to open list of operators");
  }
  while (list_ifstream.good()) {
    std::wstring oper;
    std::getline(list_ifstream, oper);
    operators_.insert(oper);
  }
  list_ifstream.close();
  #pragma endregion OPERATORS

  #pragma region RESERVED_IDS
  list_ifstream.open("lists/reserved_ids.txt");
  if (!list_ifstream.is_open()) {
    throw std::runtime_error(
        "exception thrown: unable to open list of reserved ids");
  }
  while (list_ifstream.good()) {
    std::wstring id;
    std::getline(list_ifstream, id);
    reserved_.insert(id);
  }
  list_ifstream.close();
  #pragma endregion RESERVED_IDS

  #pragma region PUNCTUATIONS
  list_ifstream.open("lists/punctuations.txt");
  if (!list_ifstream.is_open()) {
    throw std::runtime_error(
        "exception thrown: unable to open list of punctuations");
  }
  while (list_ifstream.good()) {
    std::wstring punc;
    std::getline(list_ifstream, punc);
    punctuation_.insert(punc);
  }
  list_ifstream.close();
  #pragma endregion PUNCTUATIONS

  #pragma region BACKSLASHES
  list_ifstream.open("lists/backslashes.txt");
  if (!list_ifstream.is_open()) {
    throw std::runtime_error(
        "exception thrown: unable to open list of backslash symbols");
  }
  while (list_ifstream.good()) {
    wchar_t key = list_ifstream.get();
    wchar_t value = list_ifstream.get();
    backslashes_.insert({key, value});
  }
  list_ifstream.close();
  #pragma endregion BACKSLASHES
}

void LexicAnalyzer::ChangeState(IState* state) {
  current_state_ = state; 
}

wchar_t LexicAnalyzer::Peek() { return input_stream_.peek(); }

void LexicAnalyzer::SkipChar() { input_stream_.get(); }

void LexicAnalyzer::SkipLine() { 
  std::wstring tmp;
  std::getline(input_stream_, tmp);
}

bool LexicAnalyzer::HasNext() {
  input_stream_.peek();
  return !input_stream_.eof();
}

void LexicAnalyzer::AddNextCharToBuffer() {
  token_buffer_.push_back(input_stream_.get());
}

void LexicAnalyzer::AddCharToBuffer(wchar_t symbol) {
  token_buffer_.push_back(symbol);
}

void LexicAnalyzer::AddBufferToQueue(Token::Type token_type) {
  current_token_queue_.push(Token{token_buffer_, token_type});
  token_buffer_.clear();
}

std::queue<Token> LexicAnalyzer::GetTokens() {
  while (HasNext() || !token_buffer_.empty()) {
    Run();
  }
  return current_token_queue_;
}

BeginState* LexicAnalyzer::GetBeginState() { return &begin_state_; }

OperatorState* LexicAnalyzer::GetOperatorState() { return &operator_state_; }

IDState* LexicAnalyzer::GetIDState() { return &id_state_; }

LitConstState* LexicAnalyzer::GetLitConstState() { return &lit_const_state_; }

NumberState* LexicAnalyzer::GetNumberState() { return &number_state_; }

std::wstring LexicAnalyzer::GetBuffer() { return token_buffer_; }

void LexicAnalyzer::SetBuffer(std::wstring string) { token_buffer_ = string; }

bool LexicAnalyzer::IsPunctuation(wchar_t symbol) {
  std::wstring buff(1, symbol);
  return punctuation_.find(buff) != punctuation_.end();
}

bool LexicAnalyzer::IsOperator(std::wstring string) {
  return operators_.find(string) != operators_.end();
}

bool LexicAnalyzer::IsReserved(std::wstring string) {
  return reserved_.find(string) != reserved_.end();
}

wchar_t LexicAnalyzer::ToControl(wchar_t symbol) {
  return backslashes_[symbol];
}

void LexicAnalyzer::Run() { current_state_->Execute(); }
