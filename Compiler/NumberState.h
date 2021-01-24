#ifndef NUMSTATE
#define NUMSTATE

#include <wctype.h>
#include <sstream>

#include "IState.h"

class LexicAnalyzer;

class NumberState : public IState {
 public:
  NumberState(LexicAnalyzer* fsm);
 
  virtual void Execute() final override;

 private:
  enum class State {
    INTEGER,
    HEX,
    EFOUND,
    WAITNUM,
    FLOATEXP
  };
  State state_;
  LexicAnalyzer* state_machine_;
};

#endif