#ifndef ISTATE
#define ISTATE

class IState {
 public:
  virtual void Execute() = 0;
};

#endif