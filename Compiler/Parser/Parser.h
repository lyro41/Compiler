#pragma once
#include <stack>
#include <iostream>
#include <map>
#include "Token.h"
#include "FunctionTIDEntry.h"
#include "VariableTIDEntry.h"
#include "NamespaceTIDEntry.h"
#include "StructTIDEntry.h"
#include "ArgumentAttribute.h"
#include "FunctionAttribute.h"
#include "JumpAttribute.h"
#include "StructAttribute.h"
#include <Semantic/Semantic.h>
#include <LexicAnalyzer/LexicAnalyzer.h>


#define SEMANTIC

#ifdef PARSER_UNIT_TEST
#undef SEMANTIC
#endif
// Parsing principles:
// 1 - before coming to next state, call get()
// if curToken already contains first term of grammar rule - don`t

class Parser
{
 public:
  Parser(LexicAnalyzer* analyzer, SemanticAnalyzer* semantic);
  void Parse();

 private:
  Token curToken_;
  LexicAnalyzer* analyzer_;
  SemanticAnalyzer* semantic_;
  Token get();
  std::map<std::wstring, int> type_map;

  bool IsType(Token token);
  void CheckType(std::wstring lhs, std::wstring rhs);
  TypeAttribute* CastToHighest(std::wstring lhs, std::wstring rhs);
  bool CanCast(std::wstring lhs, std::wstring rhs);
  bool CanConvertToBool(std::wstring rhs);
  void CheckInts(std::wstring lhs, std::wstring rhs);
  void CheckInt(std::wstring lhs);
  void CheckIntegral(std::wstring lhs);
  bool DoTypesMatch(std::wstring lhs, std::wstring rhs);
  bool IsIntegerType(std::wstring type);
  void DeclareJumpableConstruction();
  TypeAttribute* TryCast(TypeAttribute* target, TypeAttribute* expr);

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
  void ParsePostfixOperations();
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

