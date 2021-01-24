#ifndef LITCONSTSTATE
#define LITCONSTSTATE

#include "IState.h"

class LexicAnalyzer;

class LitConstState : public IState {
 public:
  LitConstState(LexicAnalyzer* fsm);

  virtual void Execute() final override;
  
  void SetState(bool state);

 private:
  LexicAnalyzer* state_machine_;
  void ResetState();
  bool is_char_;
  bool read_first_char_;
};

#endif