#include "IDState.h"
#include "LexicAnalyzer.h"

IDState::IDState(LexicAnalyzer* fsm) : state_machine_(fsm) {}

void IDState::Execute() {
  if (iswalnum(state_machine_->Peek()) || state_machine_->Peek() == L'_') {
    state_machine_->AddNextCharToBuffer();
    return;
  } 
  if (state_machine_->IsReserved(state_machine_->GetBuffer())) {
    state_machine_->AddBufferToQueue(Token::Type::RESERVED);
  } else {
    state_machine_->AddBufferToQueue(Token::Type::IDENTIFIER);
  }
  state_machine_->ChangeState(state_machine_->GetBeginState());
}
