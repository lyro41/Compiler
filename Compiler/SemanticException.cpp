#include "SemanticException.h"

SemanticException::SemanticException(std::string msg) : msg_(msg) {

}

SemanticException::SemanticException(std::wstring msg) { msg_ = std::string(msg.begin(), msg.end()); }

const char* SemanticException::what() const { return msg_.c_str(); }
