#include "BeginState.h"
#include "LexicAnalyzer.h"

BeginState::BeginState(LexicAnalyzer* fsm) : state_machine_(fsm) {}

void BeginState::Execute() {
  wchar_t peek = state_machine_->Peek();
  if (peek == L'\"') {
    state_machine_->SkipChar();
    state_machine_->GetLitConstState()->SetState(0);
    state_machine_->ChangeState(state_machine_->GetLitConstState());
    return;
  }
  if (peek == L'\'') {
    state_machine_->SkipChar();
    state_machine_->GetLitConstState()->SetState(1);
    state_machine_->ChangeState(state_machine_->GetLitConstState());
    return;
  }
  if (peek == L'\n' || peek == L' ' || peek == L'\t') {
    state_machine_->SkipChar();
    return;
  }
  if (peek == L'#') {
    state_machine_->SkipLine();
    return;
  }
  if (iswdigit(peek)) {
    //state_machine_->AddNextCharToBuffer();
    state_machine_->ChangeState(state_machine_->GetNumberState());
    return;
  }
  if (iswalpha(peek) || peek == L'_') {
    state_machine_->AddNextCharToBuffer();
    state_machine_->ChangeState(state_machine_->GetIDState());
    return;
  }
  if (state_machine_->IsPunctuation(peek)) {
    state_machine_->AddNextCharToBuffer();
    state_machine_->AddBufferToQueue(Token::Type::PUNCTUATION);
    return;
  }
  if (state_machine_->IsOperator(std::wstring(1, peek))) {
    state_machine_->AddNextCharToBuffer();
    state_machine_->ChangeState(state_machine_->GetOperatorState());
    return;
  }
  throw std::runtime_error("exception thrown: unexpected symbol " + state_machine_->Peek());
}