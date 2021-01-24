#include "OperatorState.h"
#include "LexicAnalyzer.h"

OperatorState::OperatorState(LexicAnalyzer* fsm) : state_machine_(fsm) {}

void OperatorState::Execute() {
  if (state_machine_->IsOperator(state_machine_->GetBuffer() + 
                                 state_machine_->Peek())) {
    state_machine_->AddNextCharToBuffer();
    return;
  } 
  state_machine_->AddBufferToQueue(Token::Type::OPERATOR);
  state_machine_->ChangeState(state_machine_->GetBeginState());
}