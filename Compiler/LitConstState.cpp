#include "LitConstState.h"
#include "LexicAnalyzer.h"

LitConstState::LitConstState(LexicAnalyzer* fsm) : 
      state_machine_(fsm), 
      is_char_(false), 
      read_first_char_(false) {}

void LitConstState::Execute() {
  if (state_machine_->Peek() == '\n' || !state_machine_->HasNext()) {
    throw std::runtime_error("error: unexpected end of literal constant");
  }
  if (state_machine_->Peek() == '\\') {
    if (is_char_ && read_first_char_) {
      throw std::runtime_error(
          "exception thrown: data-type char "
          "can only contain single character");
    }
    state_machine_->SkipChar();
    read_first_char_ = true;
    state_machine_->AddCharToBuffer(
        state_machine_->ToControl(state_machine_->Peek()));
    return;
  }

  if (is_char_) {
    if (state_machine_->Peek() != '\'') {
      if (read_first_char_) {
        throw std::runtime_error("exception thrown: data-type char"
                                 "can only contain single character");
      }
      state_machine_->AddNextCharToBuffer();
      read_first_char_ = true;
      return;
    }
    if (read_first_char_) {
      state_machine_->SkipChar(); 
      state_machine_->AddBufferToQueue(Token::Type::LITCONSTANT);
      state_machine_->ChangeState(state_machine_->GetBeginState());
      ResetState();
      return;
    }
    throw std::runtime_error("exception thrown: data-type char"
                             "can only contain single character");
  } else {
    if (state_machine_->Peek() != '\"') {
      state_machine_->AddNextCharToBuffer();
      return;
    }
  }
  state_machine_->SkipChar();
  state_machine_->AddBufferToQueue(Token::Type::LITCONSTANT);
  state_machine_->ChangeState(state_machine_->GetBeginState());
  ResetState();
}

void LitConstState::SetState(bool state) { is_char_ = state; }

void LitConstState::ResetState() {
  is_char_ = false;
  read_first_char_ = false;
}