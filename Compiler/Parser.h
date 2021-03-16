#pragma once

#include <stack>
#include <iostream>

#include "Token.h"
#include "LexicAnalyzer/LexicAnalyzer.h"

// Parsing principles:
// 1 - before coming to next state, call get()
// if curToken already contains first term of grammar rule - don`t

class Parser {
 public:
  Parser(LexicAnalyzer* analyzer);
  void Parse();
  
 private:
  Token curToken_;
  LexicAnalyzer* analyzer_;
  std::set<std::wstring> type_set_;
  
  Token get();
  
  bool IsType(Token token);
  bool IsAssignmentOperator(Token token);
  bool IsPrefixUnaryOperator(Token token);
  bool IsPostfixUnaryOperator(Token token);
  void ThrowException(std::string message);
  void AddType(std::wstring token_sym);
  
  #pragma region PARSE_METHODS
  void ParseProgram();
  
  void ParsePreprocessor();
  
  void ParseGlobalStatement();
  
  void ParseNamespaceDefinition();
  
  void ParseStructDefinition();
  
  void ParseFunction();
  
  void ParseType();
  void ParseConcreteType();
  void ParseConstType();
  
  void ParseArguments();
  
  void ParseFuncbody();
  
  void ParseBody();
  
  void ParseStatement();
  void ParseMultipleStatements();
  void ParseMultipleStatementsInCase();
  
  void ParseVarDefs();
  void ParseVars();
  void ParseVarDef();
  
  void ParseExpr();
  void ParseAssExpr();  
  void ParseInitExpr();
  
  void ParseLogImp();
  void ParseLogOr();
  void ParseLogAnd();
  
  void ParseBitOr();
  void ParseBitXor();
  void ParseBitAnd();
  
  void ParseCompeq();
  void ParseCompcomp();
  
  void ParseShift();
  
  void ParseSumsub();
  
  void ParseMuldiv();
  
  void ParsePower();
  
  void ParseUnary();
  
  void ParseGensec();
  
  void ParseNamespace();
  void ParseNestedNamespace();
  
  void ParseOperand();
  
  void ParseAttribute();
  
  void ParseIf();
  void ParseSwitch();
  void ParseWhile();
  void ParseFor();
  void ParseDowhile();
  void ParseGoto();
  void ParseReturn();
  void ParseTypeInstance();
  void ParseLabelDef();
  void ParseElseAlternatives();
  #pragma endregion 
};
