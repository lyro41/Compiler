#ifndef IDSTATE
#define IDSTATE

#include "IState.h"

class LexicAnalyzer;

class IDState : public IState {
 public:
  IDState(LexicAnalyzer* fsm);

  virtual void Execute() final override;

 private:
  LexicAnalyzer* state_machine_;
};

#endif
