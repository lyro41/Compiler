#include "NumberState.h"
#include "LexicAnalyzer.h"

NumberState::NumberState(LexicAnalyzer* fsm) :
      state_machine_(fsm), 
      state_(State::INTEGER) {}

void NumberState::Execute() {
  wchar_t peek = towlower(state_machine_->Peek());
  switch (state_) {
    case NumberState::State::INTEGER:
      if (peek == L'.') {
        state_ = State::FLOATEXP;
        state_machine_->AddNextCharToBuffer();
        return;
      } 
      if (peek == L'e') {
        state_ = State::EFOUND;
        state_machine_->AddNextCharToBuffer();
        return;
      } 
      if (peek == L'x') {
        state_machine_->SkipChar();
        if (state_machine_->GetBuffer() != L"0") {
          throw std::runtime_error("error: hex value only can start with 0x");
        } 
        if (!(iswdigit(state_machine_->Peek()) ||
              state_machine_->Peek() >= L'a' &&
              state_machine_->Peek() <= L'f')) {
          throw std::runtime_error("error: hex value must have digit after x");
        }
        state_ = State::HEX;
        return;
      } 
      if (!iswdigit(peek)) {
        state_machine_->AddBufferToQueue(Token::Type::NUMCONSTANT);
        state_machine_->ChangeState(state_machine_->GetBeginState());
        state_ = State::INTEGER;
        return;
      }
      state_machine_->AddNextCharToBuffer();
      break;
    case NumberState::State::HEX:
      if (!(iswdigit(peek) || (peek >= L'a' && peek <= L'f'))) {
        std::wstringstream stringstream;
        stringstream << std::hex << state_machine_->GetBuffer();
        size_t tmp;
        stringstream >> tmp;
        state_machine_->SetBuffer(std::to_wstring(tmp));
        state_machine_->AddBufferToQueue(Token::Type::NUMCONSTANT);
        state_machine_->ChangeState(state_machine_->GetBeginState());
        state_ = State::INTEGER;
        return;
      } 
      state_machine_->AddNextCharToBuffer();
      break;
    case NumberState::State::EFOUND:
      if (iswdigit(peek)) {
        state_ = State::FLOATEXP;
        state_machine_->AddNextCharToBuffer();
        return;
      } 
      if (peek == '-' || peek == '+') {
        state_ = State::WAITNUM;
        state_machine_->AddNextCharToBuffer();
        return;
      }
      throw std::runtime_error("error: real number must have number after E");
      break;
    case NumberState::State::WAITNUM:
      if (iswdigit(peek)) {
        state_ = State::FLOATEXP;
        state_machine_->AddNextCharToBuffer();
        return;
      }
      throw std::runtime_error("error: real number must have number after E");
      break;
    case NumberState::State::FLOATEXP:
      if (iswdigit(peek)) {
        state_machine_->AddNextCharToBuffer();
        return;
      }
      state_machine_->AddBufferToQueue(Token::Type::NUMCONSTANT);
      state_machine_->ChangeState(state_machine_->GetBeginState());
      state_ = State::INTEGER;
      break;
  }
}