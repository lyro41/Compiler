#include "SemanticException.h"

SemanticException::SemanticException(std::string msg) : msg_(msg) {

}

const char* SemanticException::what() const { return msg_.c_str(); }
