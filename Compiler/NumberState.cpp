#include "NumberState.h"
#include "LexicAnalyzer.h"

NumberState::NumberState(LexicAnalyzer* fsm) :
      state_machine_(fsm), 
      state_(State::INTEGER) {}

void NumberState::Execute() {
  wchar_t low_peek = towlower(state_machine_->Peek());
  switch (state_) {
    case NumberState::State::INTEGER:
      if (low_peek == L'.') {
        state_ = State::FLOATEXP;
        state_machine_->AddNextCharToBuffer();
        return;
      } 
      if (low_peek == L'e') {
        state_ = State::EFOUND;
        state_machine_->AddNextCharToBuffer();
        return;
      } 
      if (low_peek == L'x') {
        state_machine_->SkipChar();
        low_peek = towlower(state_machine_->Peek());
        if (state_machine_->GetBuffer() != L"0") {
          throw std::runtime_error("error: hex value only can start with 0x");
        } 
        if (!(iswdigit(low_peek) || low_peek >= L'a' && low_peek <= L'f')) {
          throw std::runtime_error("error: hex value must have digit after x");
        }
        state_ = State::HEX;
        return;
      } 
      if (!iswdigit(low_peek)) {
        state_machine_->SetBuffer(
              std::to_wstring(std::stoull(state_machine_->GetBuffer())));
        state_machine_->AddBufferToQueue(Token::Type::NUMCONSTANT);
        state_machine_->ChangeState(state_machine_->GetBeginState());
        state_ = State::INTEGER;
        return;
      }
      state_machine_->AddNextCharToBuffer();
      break;
    case NumberState::State::HEX:
      if (!(iswdigit(low_peek) || (low_peek >= L'a' && low_peek <= L'f'))) {
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
      if (iswdigit(low_peek)) {
        state_ = State::FLOATEXP;
        state_machine_->AddNextCharToBuffer();
        return;
      } 
      if (low_peek == '-' || low_peek == '+') {
        state_ = State::WAITNUM;
        state_machine_->AddNextCharToBuffer();
        return;
      }
      throw std::runtime_error("error: real number must have number after E");
      break;
    case NumberState::State::WAITNUM:
      if (iswdigit(low_peek)) {
        state_ = State::FLOATEXP;
        state_machine_->AddNextCharToBuffer();
        return;
      }
      throw std::runtime_error("error: real number must have number after E");
      break;
    case NumberState::State::FLOATEXP:
      if (iswdigit(low_peek)) {
        state_machine_->AddNextCharToBuffer();
        return;
      }
      state_machine_->AddBufferToQueue(Token::Type::NUMCONSTANT);
      state_machine_->ChangeState(state_machine_->GetBeginState());
      state_ = State::INTEGER;
      break;
  }
}