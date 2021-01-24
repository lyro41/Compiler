#ifndef OPERATORSTATE
#define OPERATORSTATE

#include "IState.h"

class LexicAnalyzer;

class OperatorState : public IState {
 public:
  OperatorState(LexicAnalyzer* fsm);

  virtual void Execute() final override;

 private:
  LexicAnalyzer* state_machine_;
};

#endif