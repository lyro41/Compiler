#ifndef BEGINSTATE
#define BEGINSTATE

#include "IState.h"

class LexicAnalyzer;

class BeginState : public IState {
 public:
  BeginState(LexicAnalyzer* fsm);

  virtual void Execute() final override;

 private:
  LexicAnalyzer* state_machine_;
};

#endif