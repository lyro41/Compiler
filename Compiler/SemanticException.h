#pragma once
#include <stdexcept>
class SemanticException : public std::exception {
  public:
  SemanticException(std::string msg);
  SemanticException(std::wstring msg);
  virtual const char* what() const override;
  private:
  std::string msg_;
};
