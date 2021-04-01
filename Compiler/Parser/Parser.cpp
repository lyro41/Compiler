#include "Parser.h"

Parser::Parser(LexicAnalyzer* analyzer, SemanticAnalyzer* semantic)
    : analyzer_(analyzer), semantic_(semantic) {
  std::wifstream type_ifstream("lists/types.txt");
  if (!type_ifstream.is_open()) {
    throw std::runtime_error("exception thrown: unable to open list of types");
  }
  while (type_ifstream.good()) {
    std::wstring type_name;
    type_ifstream >> type_name;
    int priority;
    type_ifstream >> priority;
    type_map[type_name] = priority;
  }
  type_ifstream.close();
}

void Parser::Parse() { ParseProgram(); }

Token Parser::get() { return analyzer_->GetToken(); }

bool Parser::IsType(Token token) {
  return type_map.find(token.symbol) != type_map.end();
}

void Parser::CheckType(std::wstring lhs, std::wstring rhs) {
  if (lhs == rhs) return;
  if (lhs != rhs) { 
    CastToHighest(lhs,rhs); 
  }
}

TypeAttribute* Parser::CastToHighest(std::wstring lhs, std::wstring rhs) {
  if (type_map.find(lhs) == type_map.end()
      || type_map.find(rhs) == type_map.end()) {
    ThrowException("Unable to cast selected types");
  }
  int rhs_prior = type_map[rhs];
  int lhs_prior = type_map[lhs];
  if (lhs_prior == -1 || rhs_prior == -1) {
    if (lhs == L"bool") {
      if (CanConvertToBool(rhs)) {
        return new TypeAttribute(L"bool", false, false);
      }
    }
    if (rhs == L"bool") {
      if (CanConvertToBool(lhs)) {
        return new TypeAttribute(L"bool", false, false);
        
      }
    }
    ThrowException("Unable to cast selected types");
  }
  return lhs_prior > rhs_prior ? new TypeAttribute(lhs, false, false)
                               : new TypeAttribute(rhs, false ,false);
}

bool Parser::CanCast(std::wstring lhs, std::wstring rhs) { 
  if (type_map.find(lhs) == type_map.end() ||
      type_map.find(rhs) == type_map.end()) {
    return false;
  }
  if (lhs == L"bool") {
    return CanConvertToBool(rhs);  
  }
  if (rhs == L"bool") { 
    return CanConvertToBool(lhs);
  }
}

bool Parser::CanConvertToBool(std::wstring rhs) { 
  if (rhs.find(L"int") != rhs.npos || rhs == L"double" || rhs == L"float") {
    return true;
  }
  return false;
}

void Parser::CheckInts(std::wstring lhs, std::wstring rhs) { 
  if (lhs.find(L"int") == lhs.npos && lhs != L"CINT" && lhs != L"CHEX")
    ThrowException("Left side value must be int");
  if (rhs.find(L"int") == rhs.npos && rhs != L"CINT" && rhs != L"CHEX")
    ThrowException("Right side value must be int");
}

void Parser::CheckInt(std::wstring lhs) {
  if (lhs.find(L"int") == lhs.npos && lhs != L"CINT" && lhs != L"CHEX")
    ThrowException("Left side value must be int");
}

void Parser::CheckIntegral(std::wstring lhs) {
  if(lhs.find(L"int") == lhs.npos && lhs != L"float" && lhs != L"double" 
     && lhs != L"CINT" && lhs != L"CHEX" && lhs != L"CFLOAT")
    ThrowException("Expected integral type");

}

bool Parser::IsAssignmentOperator(Token token) {
  return token.symbol == L"=" || token.symbol == L"+=" ||
         token.symbol == L"-=" || token.symbol == L"/=" ||
         token.symbol == L"//=" || token.symbol == L"%=" ||
         token.symbol == L"**=" || token.symbol == L">>=" ||
         token.symbol == L"<<=" || token.symbol == L"->=" ||
         token.symbol == L"^=" || token.symbol == L"&=" ||
         token.symbol == L"|=";
}

bool Parser::IsPrefixUnaryOperator(Token token) {
  return token.symbol == L"++" || token.symbol == L"--" ||
         token.symbol == L"~" || token.symbol == L"!" ||
         token.symbol == L"not" || token.symbol == L"+" ||
         token.symbol == L"-" || token.symbol == L"@" || token.symbol == L"?" ||
         token.symbol == L"delete" || token.symbol == L"new";
}

bool Parser::IsPostfixUnaryOperator(Token token) {
  return token.symbol == L"++" || token.symbol == L"--";
}

void Parser::ThrowException(std::string message) {
  // TODO: need to improve
  std::string msg;
  msg.append(std::to_string(analyzer_->GetCursorPosition().first));
  msg.append(" ");
  msg.append(std::to_string(analyzer_->GetCursorPosition().second));
  msg.append("\n");
  msg.append(message);
  std::cout << msg << "\n";
  throw std::runtime_error(msg);
}

void Parser::AddType(std::wstring token_sym) {
  if (type_map.find(token_sym) != type_map.end()) {
    ThrowException("Struct redefinition");
  }
  type_map[token_sym] = -1;
}

void Parser::ParseProgram() {
  curToken_ = get();
  ParsePreprocessor();
  ParseGlobalStatement();
  if (curToken_.type != Token::Type::ENDOFFILE) {
    ThrowException("Unexpected token");
  }
}

void Parser::ParsePreprocessor() {
  while (curToken_.symbol == L"import") {
    curToken_ = get();
    if (curToken_.type != Token::Type::LITCONSTANT) {
      ThrowException("Token type of literal constant expected");
    }
    semantic_->TryToParseFile(curToken_.symbol);
    curToken_ = get();
  }
}

void Parser::ParseGlobalStatement() {
  bool shouldLeave = false;
  while (!shouldLeave) {
    shouldLeave = true;
    if (curToken_.symbol == L"func") {
      ParseFunction();
      shouldLeave = false;
    }
    if (curToken_.symbol == L"namespace") {
      ParseNamespaceDefinition();
      shouldLeave = false;
    }
    if (curToken_.symbol == L"struct") {
      ParseStructDefinition();
      shouldLeave = false;
    }
    if (IsType(curToken_)) {
      ParseVarDefs();
      if (curToken_.symbol != L";") {
        ThrowException("Expected ;");
      }
      curToken_ = get();
      shouldLeave = false;
    }
  }
}

void Parser::ParseNamespaceDefinition() {
  // we already have namespace
  curToken_ = get();
  if (curToken_.type != Token::Type::IDENTIFIER) {
    ThrowException("Expected identifier");
  }
#ifdef SEMANTIC
  NamespaceTIDEntry* nspace_entry = new NamespaceTIDEntry();
  nspace_entry->name = curToken_.symbol;
  semantic_->CreateNewTID();
#endif

  curToken_ = get();
  if (curToken_.symbol != L"{") {
    ThrowException("Expected curly opening bracket");
  }

  curToken_ = get();
  ParseGlobalStatement();
  if (curToken_.symbol != L"}") {
    ThrowException("Expected curly closing bracket");
  }
  curToken_ = get();
#ifdef SEMANTIC
  TID* curTID = semantic_->GetCurrentTID();
  semantic_->DisconnectCurrentTID();
  nspace_entry->SetCurrentTID(curTID);
  semantic_->PushInCurrentTID(nspace_entry);
#endif
}

void Parser::ParseStructDefinition() {
  // we already have struct
  curToken_ = get();
  if (curToken_.type != Token::Type::IDENTIFIER) {
    ThrowException("Expected identifier");
  }
  // AddType is semantic, but required for parser
  AddType(curToken_.symbol);
  #ifdef SEMANTIC
  StructTIDEntry* str_entry = new StructTIDEntry();
  str_entry->name = curToken_.symbol;
  semantic_->CreateNewTID();
  #endif
  curToken_ = get();
  if (curToken_.symbol != L"{") {
    ThrowException("Expected curly opening bracket");
  }
  curToken_ = get();
  ParseGlobalStatement();
  if (curToken_.symbol != L"}") {
    ThrowException("Expected curly closing bracket");
  }
#ifdef SEMANTIC
  TID* curTID = semantic_->GetCurrentTID();
  semantic_->DisconnectCurrentTID();
  str_entry->SetCurrentTID(curTID);
  semantic_->PushAttribute(new TypeAttribute(str_entry->name, false, true));
#endif
  curToken_ = get();
  if (curToken_.symbol != L";") {
    ParseVars();
  }
#ifdef SEMANTIC
  semantic_->PopAttribute();
#endif
  if (curToken_.symbol != L";") {
    ThrowException("Expected ;");
  }

  curToken_ = get();
}

void Parser::ParseFunction() {
  // we already have func
  curToken_ = get();
  if (curToken_.type != Token::Type::IDENTIFIER) {
    if (curToken_.type == Token::Type::RESERVED) {
      ThrowException("Misuse of reserved word");
    } else {
      ThrowException("Identifier expected");
    }
  }
#ifdef SEMANTIC
  FunctionTIDEntry* func_entry = new FunctionTIDEntry();
  func_entry->is_proto = true;
  func_entry->name = curToken_.symbol;
#endif
  curToken_ = get();
  if (curToken_.symbol != L"(") {
    ThrowException("Expected opening brackets");
  }

  curToken_ = get();
  ParseArguments();
  // We have empty stack of stack full of ArgumentAttribute`s
  if (curToken_.symbol != L")") {
    ThrowException("Expected closing brackets");
  }

  curToken_ = get();
  if (curToken_.symbol != L":") {
    ThrowException("Expected ':'");
  }

  curToken_ = get();
  ParseConcreteType();
#ifdef SEMANTIC
  TypeAttribute* return_type_attr =
      dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  if (!return_type_attr) ThrowException("Semantic Analyzer expected Type");
  func_entry->return_type = return_type_attr->type;
  semantic_->PopAttribute();
#endif
  if (curToken_.symbol != L";") {
    // If it`s not forward declaration
#ifdef SEMANTIC
    func_entry->is_proto = false;
    semantic_->PushInCurrentTID(func_entry);
    semantic_->CreateNewTID();
    while (!semantic_->AttributeStackEmpty()) {
      ArgumentAttribute* arg_attr =
          dynamic_cast<ArgumentAttribute*>(semantic_->TopStackAttribute());
      if (!arg_attr) ThrowException("Semantic Analyzer expected Argument");

      VariableTIDEntry* var_entry = new VariableTIDEntry(arg_attr->type);
      var_entry->name = arg_attr->name;
      func_entry->type_arguments.push_back(arg_attr->type);
      semantic_->PushInCurrentTID(var_entry);
      semantic_->PopAttribute();
      //semantic_->PushAttribute(new TypeAttribute(func_entry->return_type, false , false));
    }
#endif
    ParseFuncbody();
    //if (!semantic_->AttributeStackEmpty())
    // semantic_->PopAttribute();
#ifdef SEMANTIC
    semantic_->RemoveCurrentTID();
#endif
  } else {
#ifdef SEMANTIC
    while (!semantic_->AttributeStackEmpty()) {
      ArgumentAttribute* arg_attr =
          dynamic_cast<ArgumentAttribute*>(semantic_->TopStackAttribute());
      if (!arg_attr) ThrowException("Semantic Analyzer expected Argument");
      func_entry->type_arguments.push_back(arg_attr->type);
      semantic_->PopAttribute();
    }
    semantic_->PushInCurrentTID(func_entry);
#endif
    curToken_ = get();
  }
}

/// <summary>
/// Semantic Analyzer :
/// Pushes TypeAttribute to stack after successfull finish
/// </summary>
void Parser::ParseType() {
  if (curToken_.symbol == L"const") {
    ParseConstType();
  } else {
    ParseConcreteType();
  }
}

/// <summary>
/// Semantic Analyzer :
/// Pushes TypeAttribute to stack after successfull finish
/// </summary>
void Parser::ParseConcreteType() {
  if (curToken_.type != Token::Type::IDENTIFIER &&
      curToken_.type != Token::Type::RESERVED) {
    ThrowException("Expected identifier or reserved");
  }
  std::wstring type = curToken_.symbol;
  curToken_ = get();
  while (curToken_.symbol == L"@") {
    type.append(curToken_.symbol);
    curToken_ = get();
  }

  while (curToken_.symbol == L"[") {
    type.append(curToken_.symbol);
    curToken_ = get();
    while (curToken_.symbol == L",") {
      type.append(curToken_.symbol);
      curToken_ = get();
    }

    if (curToken_.symbol != L"]") {
      ThrowException("Expected closing rect closing bracket");
    }
    type.append(curToken_.symbol);
    curToken_ = get();

    while (curToken_.symbol == L"@") {
      type.append(curToken_.symbol);
      curToken_ = get();
    }
  }
#ifdef SEMANTIC
  TypeAttribute* type_attr = new TypeAttribute(type, false, true);
  semantic_->PushAttribute(type_attr);
#endif
}

void Parser::ParseConstType() {
  // curToken_ already contains const
  curToken_ = get();
  ParseConcreteType();
#ifdef SEMANTIC
  dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute())->is_constant =
      true;
#endif
}

void Parser::ParseArguments() {
  if (curToken_.symbol == L")") return;
  if (curToken_.symbol == L"void") {
    curToken_ = get();
    return;
  }
  ParseType();
#ifdef SEMANTIC
  // There is type at attribute stack
  TypeAttribute* type_attr =
      dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  if (!type_attr) ThrowException("Semantic Analyzer expected Type");
#endif
  if (curToken_.type != Token::Type::IDENTIFIER) {
    if (curToken_.type == Token::Type::RESERVED) {
      ThrowException("Misuse of reserved word");
    } else {
      ThrowException("Identifier expected");
    }
  }
#ifdef SEMANTIC
  ArgumentAttribute* argument_attr =
      new ArgumentAttribute(curToken_.symbol, type_attr);
  semantic_->PopAttribute();
  semantic_->PushAttribute(argument_attr);
#endif
  curToken_ = get();
  while (curToken_.symbol == L",") {
    curToken_ = get();
    ParseType();
#ifdef SEMANTIC
    type_attr = dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (!type_attr) ThrowException("Semantic Analyzer expected Type");
    argument_attr = new ArgumentAttribute(curToken_.symbol, type_attr);
    semantic_->PopAttribute();
    semantic_->PushAttribute(argument_attr);
#endif
    if (curToken_.type != Token::Type::IDENTIFIER) {
      if (curToken_.type == Token::Type::RESERVED) {
        ThrowException("Misuse of reserved word");
      } else {
        ThrowException("Identifier expected");
      }
    }
    curToken_ = get();
  }
}

/// <summary>
/// Parses complex body(func body)
/// Parses multiple statements
/// Dependency : Checks }
/// </summary>
void Parser::ParseFuncbody() {
  if (curToken_.symbol != L"{") {
    ThrowException("Expected opening curly bracket");
  }

  curToken_ = get();
  ParseMultipleStatements();
  if (curToken_.symbol != L"}") {
    ThrowException("Expected closing curly bracket");
  }

  curToken_ = get();
}

void Parser::ParseBody() {
  if (curToken_.symbol == L"{") {
    ParseFuncbody();
  } else {
    ParseStatement();
  }
}

void Parser::ParseStatement() {
  if (curToken_.symbol == L"if") {
    ParseIf();
    return;
  }
  if (curToken_.symbol == L"while") {
    ParseWhile();
    return;
  }
  if (curToken_.symbol == L"switch") {
    ParseSwitch();
    return;
  }
  if (curToken_.symbol == L"for") {
    ParseFor();
    return;
  }
  if (curToken_.symbol == L"do") {
    ParseDowhile();
    if (curToken_.symbol != L";") {
      ThrowException("Expected ;");
    }
    curToken_ = get();
    return;
  }
  if (curToken_.symbol == L"const" || IsType(curToken_)) {
    ParseVarDefs();
    if (curToken_.symbol != L";") {
      ThrowException("Expected ;");
    }
    curToken_ = get();
    return;
  }
  if (curToken_.type == Token::Type::RESERVED) {
    ParseGoto();
    if (curToken_.symbol != L";") {
      ThrowException("Expected ;");
    }
    curToken_ = get();
    return;
  }
  if (curToken_.symbol == L"{") {
#ifdef SEMANTIC
    semantic_->CreateNewTID();
#endif
    ParseFuncbody();
#ifdef SEMANTIC
    semantic_->RemoveCurrentTID();
#endif
    return;
  }

  ParseExpr();
  if (curToken_.symbol != L";") {
    ThrowException("Expected ;");
  }
#ifdef SEMANTIC
  semantic_->PopAttribute();
#endif
  curToken_ = get();
}

void Parser::ParseMultipleStatements() {
  // Used only in '{' <Statement> '}'
  while (curToken_.symbol != L"}") {
    if (curToken_.type == Token::Type::ENDOFFILE) {
      ThrowException("Expected statement or }, got end of file");
    }
    ParseStatement();
  }
}

void Parser::ParseMultipleStatementsInCase() {
  // Used only in '{' <Statement> '}'
  while (curToken_.symbol != L"}" && curToken_.symbol != L"case" &&
         curToken_.symbol != L"default") {
    ParseStatement();
  }
}

void Parser::ParseVarDefs() {
  // we have const or type
  ParseType();
  // Type is on stack
  ParseVars();
  // Get Type out
#ifdef SEMANTIC
  semantic_->PopAttribute();
#endif
}

void Parser::ParseVars() {
  ParseVarDef();
  while (curToken_.symbol == L",") {
    curToken_ = get();
    ParseVarDef();
  }
}

void Parser::ParseVarDef() {
  if (curToken_.type != Token::Type::IDENTIFIER) {
    ThrowException("Expected identifier");
  }
#ifdef SEMANTIC
  TypeAttribute* type_attr =
      dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  if (!type_attr) ThrowException("Semantic analyzer expected Type");
  VariableTIDEntry* var_entry = new VariableTIDEntry(*type_attr);
  var_entry->name = curToken_.symbol;
  semantic_->PushInCurrentTID(var_entry);
#endif
  curToken_ = get();
  if (curToken_.symbol == L"=") {
    curToken_ = get();
    ParseExpr();
#ifdef SEMANTIC
    TypeAttribute* expr_type_attr =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (!expr_type_attr) ThrowException("Semantic analyzer expected Type");
    CheckType(type_attr->type, expr_type_attr->type);
    // killing ParseExpr result type
    semantic_->PopAttribute();
#endif
  }
}

void Parser::ParseExpr() {
  ParseAssExpr();
  while (curToken_.symbol == L",") {
#ifdef SEMANTIC
    semantic_->PopAttribute();
#endif
    curToken_ = get();
    ParseAssExpr();
  }
}

void Parser::ParseAssExpr() {
  ParseLogImp();
  if (IsAssignmentOperator(curToken_)) {
#ifdef SEMANTIC
    TypeAttribute* lhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (!lhs_type->is_lrvalue) ThrowException("Type must be lvalue");
    std::wstring oper = curToken_.symbol;
#endif
    curToken_ = get();
    ParseInitExpr();
#ifdef SEMANTIC
    TypeAttribute* rhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (oper == L"//=" || oper == L"%=") {
      CheckInts(lhs_type->type, rhs_type->type);
    }
    TypeAttribute* casted = CastToHighest(lhs_type->type, rhs_type->type);
    semantic_->PopAttribute();
    semantic_->PopAttribute();
    semantic_->PushAttribute(casted);
#endif
  }
}

void Parser::ParseInitExpr() {
  //if (curToken_.symbol == L"{") {
  //  // Init List
  //  curToken_ = get();
  //  ParseAssExpr();
  //  while (curToken_.symbol == L",") {
  //    // Init list seq
  //    curToken_ = get();
  //    ParseAssExpr();
  //  }
  //} else {
  //  ParseAssExpr();
  //}
  ParseAssExpr();
}

void Parser::ParseLogImp() {
  ParseLogOr();
  if (curToken_.symbol == L"->") {
#ifdef SEMANTIC
    TypeAttribute* lhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (!CanConvertToBool(lhs_type->type))
      ThrowException("Type must be convertible to bool");
    semantic_->PopAttribute();
#endif
    curToken_ = get();
    ParseLogImp();
#ifdef SEMANTIC
    TypeAttribute* rhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (!CanConvertToBool(rhs_type->type))
      ThrowException("Type must be convertible to bool");
    semantic_->PopAttribute();
    semantic_->PushAttribute(new TypeAttribute(L"bool", false, false));
#endif
  }
}

void Parser::ParseLogOr() {
  ParseLogAnd();
  if (curToken_.symbol == L"||" || curToken_.symbol == L"or") {
#ifdef SEMANTIC
    TypeAttribute* lhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (!CanConvertToBool(lhs_type->type))
      ThrowException("Type must be convertible to bool");
    semantic_->PopAttribute();
#endif
    curToken_ = get();
    ParseLogOr();
#ifdef SEMANTIC
    TypeAttribute* rhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (!CanConvertToBool(rhs_type->type))
      ThrowException("Type must be convertible to bool");
    semantic_->PopAttribute();
    semantic_->PushAttribute(new TypeAttribute(L"bool", false, false));
#endif
  }
}

void Parser::ParseLogAnd() {
  ParseBitOr();
  if (curToken_.symbol == L"&&" || curToken_.symbol == L"and") {
#ifdef SEMANTIC
    TypeAttribute* lhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (!CanConvertToBool(lhs_type->type))
      ThrowException("Type must be convertible to bool");
    semantic_->PopAttribute();
#endif
    curToken_ = get();
    ParseLogAnd();
#ifdef SEMANTIC
    TypeAttribute* rhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (!CanConvertToBool(rhs_type->type))
      ThrowException("Type must be convertible to bool");
    semantic_->PopAttribute();
    semantic_->PushAttribute(new TypeAttribute(L"bool", false, false));
#endif
  }
}

void Parser::ParseBitOr() {
  ParseBitXor();
  if (curToken_.symbol == L"|") {
#ifdef SEMANTIC
    TypeAttribute* lhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    std::wstring oper = curToken_.symbol;
#endif
    curToken_ = get();
    ParseBitOr();
#ifdef SEMANTIC
    TypeAttribute* rhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    CheckInts(rhs_type->type, lhs_type->type);
    TypeAttribute* casted = CastToHighest(lhs_type->type, rhs_type->type);
    semantic_->PopAttribute();
    semantic_->PopAttribute();
    semantic_->PushAttribute(casted);
#endif
  }
}

void Parser::ParseBitXor() {
  ParseBitAnd();
  if (curToken_.symbol == L"^" || curToken_.symbol == L"xor") {
#ifdef SEMANTIC
    TypeAttribute* lhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    std::wstring oper = curToken_.symbol;
#endif
    curToken_ = get();
    ParseBitXor();
#ifdef SEMANTIC
    TypeAttribute* rhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    CheckInts(rhs_type->type, lhs_type->type);
    TypeAttribute* casted = CastToHighest(lhs_type->type, rhs_type->type);
    semantic_->PopAttribute();
    semantic_->PopAttribute();
    semantic_->PushAttribute(casted);
#endif
  }
}

void Parser::ParseBitAnd() {
  ParseCompeq();
  if (curToken_.symbol == L"&") {
#ifdef SEMANTIC
    TypeAttribute* lhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    std::wstring oper = curToken_.symbol;
#endif
    curToken_ = get();
    ParseBitAnd();
#ifdef SEMANTIC
    TypeAttribute* rhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    CheckInts(rhs_type->type, lhs_type->type);
    TypeAttribute* casted = CastToHighest(lhs_type->type, rhs_type->type);
    semantic_->PopAttribute();
    semantic_->PopAttribute();
    semantic_->PushAttribute(casted);
#endif
  }
}

void Parser::ParseCompeq() {
  ParseCompcomp();
  if (curToken_.symbol == L"==" || curToken_.symbol == L"!=") {
#ifdef SEMANTIC
    semantic_->PopAttribute();
#endif
    curToken_ = get();
    ParseCompeq();
#ifdef SEMANTIC
    semantic_->PopAttribute();
    semantic_->PushAttribute(new TypeAttribute(L"bool", false, false));
#endif
  }
}

void Parser::ParseCompcomp() {
  ParseShift();
  if (curToken_.symbol == L">=" || curToken_.symbol == L"<=" ||
      curToken_.symbol == L">" || curToken_.symbol == L"<") {
#ifdef SEMANTIC
    semantic_->PopAttribute();
#endif
    curToken_ = get();
    ParseCompcomp();
#ifdef SEMANTIC
    semantic_->PopAttribute();
    semantic_->PushAttribute(new TypeAttribute(L"bool", false, false));
#endif
  }
}

void Parser::ParseShift() {
  ParseSumsub();
  if (curToken_.symbol == L">>" || curToken_.symbol == L"<<") {
#ifdef SEMANTIC
    TypeAttribute* lhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    std::wstring oper = curToken_.symbol;
#endif
    curToken_ = get();
    ParseBitAnd();
#ifdef SEMANTIC
    TypeAttribute* rhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    CheckInts(rhs_type->type, lhs_type->type);
    TypeAttribute* casted = CastToHighest(lhs_type->type, rhs_type->type);
    semantic_->PopAttribute();
    semantic_->PopAttribute();
    semantic_->PushAttribute(casted);
#endif
  }
}

void Parser::ParseSumsub() {
  ParseMuldiv();
  if (curToken_.symbol == L"+" || curToken_.symbol == L"-") {
#ifdef SEMANTIC
    TypeAttribute* lhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
#endif
    curToken_ = get();
    ParseSumsub();
#ifdef SEMANTIC
    TypeAttribute* rhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    TypeAttribute* casted = CastToHighest(lhs_type->type, rhs_type->type);
    semantic_->PopAttribute();
    semantic_->PopAttribute();
    semantic_->PushAttribute(casted);
#endif
  }
}

void Parser::ParseMuldiv() {
  ParsePower();
  if (curToken_.symbol == L"//" || curToken_.symbol == L"/" ||
      curToken_.symbol == L"*" || curToken_.symbol == L"%") {
#ifdef SEMANTIC
    TypeAttribute* lhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    std::wstring oper = curToken_.symbol;
#endif
    curToken_ = get();
    ParseMuldiv();
#ifdef SEMANTIC
    TypeAttribute* rhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (oper == L"//" || oper == L"%") {
      CheckInts(lhs_type->type, rhs_type->type);
    }
    TypeAttribute* casted = CastToHighest(lhs_type->type, rhs_type->type);
    semantic_->PopAttribute();
    semantic_->PopAttribute();
    semantic_->PushAttribute(casted);
#endif
  }
}

void Parser::ParsePower() {
  ParseUnary();
  if (curToken_.symbol == L"**") {
#ifdef SEMANTIC
    TypeAttribute* lhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
#endif
    curToken_ = get();
    ParsePower();
#ifdef SEMANTIC
    TypeAttribute* rhs_type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    TypeAttribute* casted = CastToHighest(lhs_type->type, rhs_type->type);
    semantic_->PopAttribute();
    semantic_->PopAttribute();
    semantic_->PushAttribute(casted);
#endif
  }
}

void Parser::ParseUnary() {
  if (curToken_.symbol == L"new") {
    curToken_ = get();
    ParseTypeInstance();
    return;
  }
  #ifdef SEMANTIC
  std::stack<Token> prefixes;
  #endif
  while (IsPrefixUnaryOperator(curToken_)) {
#ifdef SEMANTIC
    prefixes.push(curToken_);
#endif
    curToken_ = get();

  }
  ParseGensec();
#ifdef SEMANTIC
  TypeAttribute* type_attr = dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  while (!prefixes.empty()) {
    Token tok = prefixes.top();
    if (tok.symbol == L"not" || tok.symbol == L"!") {
      if (!CanConvertToBool(type_attr->type)) ThrowException("Type must be convertible to bool");
      type_attr->type = L"bool";
      type_attr->is_constant = false;
      type_attr->is_lrvalue = false;
    }
    if (tok.symbol == L"~") {
      CheckInt(type_attr->type);
    }
    if (tok.symbol == L"++" || tok.symbol == L"--") {
      if (!type_attr->is_lrvalue) ThrowException("Type must be LValue");
      CheckIntegral(type_attr->type);
    }
    if (tok.symbol == L"+" || tok.symbol == L"-") {
      CheckIntegral(type_attr->type);
    }
    if (tok.symbol == L"delete") {
      if (type_attr->type.find(L"@") == type_attr->type.npos &&
          type_attr->type.find(L"[") == type_attr->type.npos) {
        ThrowException("Expected pointer type");
        type_attr->type = L"void";
        type_attr->is_constant = false;
        type_attr->is_lrvalue = false;
      }
    }
    if (tok.symbol == L"?") {
      if (!type_attr->is_lrvalue) ThrowException("Type must be LValue");
      type_attr->type.append(L"@");
      type_attr->is_constant = false;
      type_attr->is_lrvalue = false;
    }
    prefixes.pop();
  }
#endif
}

void Parser::ParseGensec() {
  if (curToken_.symbol == L"cast") {
    curToken_ = get();
    if (curToken_.symbol != L"<") {
      ThrowException("Expected opening triangle bracket");
    }

    curToken_ = get();
    ParseType();
    if (curToken_.symbol != L">") {
      ThrowException("Expected closing triangle bracket");
    }

    curToken_ = get();
    if (curToken_.symbol != L"(") {
      ThrowException("Expected opening bracket");
    }
    curToken_ = get();
    ParseExpr();
    if (curToken_.symbol != L")") {
      ThrowException("Expected closing bracket");
    }
    return;
  }

  ParseNamespace();
  ParsePostfixOperations();
}

void Parser::ParseNamespace() {
  if (curToken_.type == Token::Type::IDENTIFIER) {
    Token buffer = curToken_;
    curToken_ = get();
    if (curToken_.symbol == L"$") {
      // two options
      // 1. buffer is struct variable
      // 2. buffer is namespace
#ifdef SEMANTIC
      // save current tid
      semantic_->StartNamespaceParse();
      ITIDEntry* entry = semantic_->GetCurrentTID()->FindByName(buffer.symbol);
      if (!entry) ThrowException("No definition is found");
      NamespaceTIDEntry* nspace = dynamic_cast<NamespaceTIDEntry*>(entry);
      if (nspace) {
        semantic_->ChangeCurrentTID(nspace->GetCurrentTID());
      } else {
        ThrowException("Expected namespace");
      }
      
#endif
      curToken_ = get();
      ParseNestedNamespace();
#ifdef SEMANTIC
      semantic_->EndNamespaceParse();
#endif
      return;
    } else {
#ifdef SEMANTIC
      ITIDEntry* entry = semantic_->GetCurrentTID()->FindByName(buffer.symbol);
      if (!entry) {
        ThrowException("No such variable is defined");
      }
      VariableTIDEntry* var = dynamic_cast<VariableTIDEntry*>(entry);
      if (var) {
        semantic_->PushAttribute(new TypeAttribute(var->type));
      } else {
        FunctionTIDEntry* func = dynamic_cast<FunctionTIDEntry*>(entry);
        if (func) {
          semantic_->PushAttribute(new FunctionAttribute(buffer.symbol));
        } else {
          ThrowException("Unexpected identifier");
        }
      }
#endif
    }
    // buffer is a variable;
    // curtoken is next token!
    return;
  } else {
    if (curToken_.symbol == L"$") {
      // global smth here
      curToken_ = get();
      if (curToken_.type != Token::Type::IDENTIFIER)
        ThrowException("Identifier expected");
#ifdef SEMANTIC
      ITIDEntry* entry =
          semantic_->GetGlobalTID()->FindByName(curToken_.symbol);
      if (!entry) {
        ThrowException("No such variable is defined");
      }
      VariableTIDEntry* var = dynamic_cast<VariableTIDEntry*>(entry);
      if (var) {
        semantic_->PushAttribute(new TypeAttribute(var->type));
      } else {
        FunctionTIDEntry* func = dynamic_cast<FunctionTIDEntry*>(entry);
        if (func) {
          semantic_->PushAttribute(new TypeAttribute(L"CFUNC", true, false));
        } else {
          ThrowException("Unexpected identifier");
        }
      }
#endif
      curToken_ = get();
      return;
    }
  }
  ParseOperand();
}

void Parser::ParseNestedNamespace() {
  Token buffer;
  while (true) {
    if (curToken_.type != Token::Type::IDENTIFIER)
      ThrowException("Expected identifier");
    buffer = curToken_;
    curToken_ = get();
    if (curToken_.symbol == L"$") {
      // buffer is namespace
#ifdef SEMANTIC
      ITIDEntry* entry = semantic_->GetCurrentTID()->FindByName(buffer.symbol);
      if (!entry) ThrowException("No definition is found");
      NamespaceTIDEntry* nspace = dynamic_cast<NamespaceTIDEntry*>(entry);
      if (nspace) {
        semantic_->ChangeCurrentTID(nspace->GetCurrentTID());
      } else {
        ThrowException("Expected namespace");
      }
#endif
      curToken_ = get();
    } else {
      // buffer is variable
#ifdef SEMANTIC
      ITIDEntry* entry = semantic_->GetCurrentTID()->FindByName(buffer.symbol);
      if (!entry) {
        ThrowException("No such variable is defined");
      }
      VariableTIDEntry* var = dynamic_cast<VariableTIDEntry*>(entry);
      if (var) {
        semantic_->PushAttribute(new TypeAttribute(var->type));
      } else {
        FunctionTIDEntry* func = dynamic_cast<FunctionTIDEntry*>(entry);
        if (func) {
          semantic_->PushAttribute(new TypeAttribute(L"CFUNC", true, false));
        } else {
          ThrowException("Unexpected identifier");
        }
      }
#endif
      break;
    }
  }
}

void Parser::ParseOperand() {
  if (curToken_.symbol == L"(") {
    curToken_ = get();
    ParseExpr();
    if (curToken_.symbol != L")") {
      ThrowException("Expected closing brackets");
    }
    curToken_ = get();
    return;
  }
  if (curToken_.type != Token::Type::LITCONSTANT &&
      curToken_.type != Token::Type::NUMCONSTANT &&
      curToken_.symbol != L"NULL" && curToken_.symbol != L"NIL") {
    ThrowException("Expected any constant");
  }
#ifdef SEMANTIC
  if (semantic_->IsNumberInt(curToken_.symbol)) {
    semantic_->PushAttribute(new TypeAttribute(L"CINTEGER", true, false));
  } else {
    if (semantic_->IsNumberFloat(curToken_.symbol)) {
      semantic_->PushAttribute(new TypeAttribute(L"CFLOAT", true, false));
    } else {
      if (semantic_->IsNumberHex(curToken_.symbol)) {
        semantic_->PushAttribute(new TypeAttribute(L"CHEX", true, false));
      }
    }
  }
  if (curToken_.type == Token::Type::LITCONSTANT) {
    semantic_->PushAttribute(new TypeAttribute(L"CSTRING", true, false));     
  }
#endif
  curToken_ = get();
}

void Parser::ParseAttribute() {
  if (curToken_.type != Token::Type::IDENTIFIER) {
    ThrowException("Expected identifier");
  }
  curToken_ = get();
  ParsePostfixOperations();
}

void Parser::ParsePostfixOperations() {
  bool shouldLeave = false;
  while (!shouldLeave) {
    shouldLeave = true;
    if (IsPostfixUnaryOperator(curToken_)) {
      shouldLeave = false;
#ifdef SEMANTIC
      TypeAttribute* type_attr =
          dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
      CheckIntegral(type_attr->type);
      type_attr->is_lrvalue = false;
      type_attr->is_constant = false;
#endif
      curToken_ = get();
    }
    if (curToken_.symbol == L"(") {
      // call of function
      shouldLeave = false;
      curToken_ = get();
      std::vector<std::wstring> args;
      if (curToken_.symbol != L")") {
        ParseAssExpr();
#ifdef SEMANTIC
        TypeAttribute* type_attr =
            dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
        args.push_back(type_attr->type);
        semantic_->PopAttribute();
#endif
        while (curToken_.symbol == L",") {
          curToken_ = get();
          ParseAssExpr();
#ifdef SEMANTIC
          TypeAttribute* type_attr =
              dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
          args.push_back(type_attr->type);
          semantic_->PopAttribute();
#endif
        }
      }
      // bruh
      if (curToken_.symbol != L")") {
        ThrowException("Expected closing bracket");
      }
#ifdef SEMANTIC
      FunctionAttribute* f_attr =
          dynamic_cast<FunctionAttribute*>(semantic_->TopStackAttribute());
      std::vector<ITIDEntry*> functions =
          semantic_->GetCurrentTID()->FindAllByName(f_attr->name);
      bool is_found = true;
      
      for (auto& e : functions) {
        FunctionTIDEntry* func = dynamic_cast<FunctionTIDEntry*>(e);
        if (func->is_proto)
          ThrowException("Unable to call prototype of function");
        if (func->type_arguments.size() != args.size()) continue;

        for (int i = 0; i < args.size(); ++i) {
          if (args[i] != func->type_arguments[i].type) {
            is_found = false;
            break;
          }
        }
        if (is_found) {
          for (int j = 0; j < func->type_arguments.size(); ++j) {
            semantic_->PopAttribute();
          }
          semantic_->PushAttribute(new TypeAttribute(func->return_type, false, false));
        }
      }
      if (!is_found) {
        for (auto& e : functions) {
          FunctionTIDEntry* func = dynamic_cast<FunctionTIDEntry*>(e);
          if (func->type_arguments.size() != args.size()) continue;
          bool all_ok = true;
          for (int i = 0; i < args.size(); ++i) {
            if (!CanCast(args[i],func->type_arguments[i].type)) {
              all_ok = false;
              break;
            } 
          }
          if (all_ok) {
            if (is_found) {
              ThrowException("Ambigious function call");
            } else {
              is_found = true;
              semantic_->PushAttribute(
                  new TypeAttribute(func->return_type, false, false));
            }
          }
        }
      }
      if (!is_found)
        ThrowException("Unable to find called function overload");
#endif
      curToken_ = get();
    }
    if (curToken_.symbol == L"[") {
      // indexing
      shouldLeave = false;
      curToken_ = get();
#ifdef SEMANTIC
      TypeAttribute* type =
          dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
      std::wstring base_type = type->type;
      std::wstring suffix;
      size_t i = 0;
      for (i = 0; i < base_type.size(); ++i) {
        if (base_type[i] == L'@') break;
        if (base_type[i] == L'[') break;
      }
      size_t expected_count = 1;
      suffix = base_type.substr(i);
      if (suffix[0] == L'@') {
        suffix = L"@";
      } else {
        while (type->type[i] != L']') {
          ++expected_count;
        }
      }
      base_type = base_type.substr(0, i - 1);
      semantic_->PopAttribute();

      size_t actual_count = 1;
#endif
      ParseAssExpr();
      // TO DO : check for int

      while (curToken_.symbol == L",") {
#ifdef SEMANTIC
        ++actual_count;
        if (actual_count > expected_count) {
          ThrowException("Too much indexes");
        }
#endif
        curToken_ = get();
        ParseAssExpr();
      }
      if (curToken_.symbol != L"]") {
        ThrowException("Expected closing rect bracket");
      }
      curToken_ = get();
#ifdef SEMANTIC
      TypeAttribute* type_attr = new TypeAttribute(base_type + suffix.substr(actual_count), false, true);
#endif
    }
    if (curToken_.symbol == L".") {
      shouldLeave = false;
      curToken_ = get();
      ParseAttribute();
    }
  }
}

void Parser::ParseIf() {
  // we already have if
  curToken_ = get();
  if (curToken_.symbol != L"(") {
    ThrowException("Expected opening brackets");
  }

  curToken_ = get();
  ParseExpr();
  #ifdef SEMANTIC
  TypeAttribute* type =
      dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  if (!CanCast(L"bool", type->type)) {
    ThrowException("Unable to cast");
  }
  semantic_->PopAttribute();
  #endif
  if (curToken_.symbol != L")") {
    ThrowException("Expected closing brackets");
  }

  curToken_ = get();
  ParseBody();
  ParseElseAlternatives();
}

void Parser::ParseSwitch() {
  // we already have switch
  curToken_ = get();
  if (curToken_.symbol != L"(") {
    ThrowException("Expected opening bracket");
  }

  curToken_ = get();
  ParseExpr();
#ifdef SEMANTIC
  TypeAttribute* type_attr = dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  if (!CanConvertToBool(type_attr->type))
    ThrowException("Expected type converible to bool");
  semantic_->PopAttribute();
  type_attr = dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  semantic_->PushAttribute(new JumpAttribute(type_attr));
#endif
  if (curToken_.symbol != L")") {
    ThrowException("Expected closing bracket");
  }

  curToken_ = get();
  if (curToken_.symbol != L"{") {
    ThrowException("Expected opening curly bracket");
  }

  curToken_ = get();
  while (curToken_.symbol == L"case") {
    curToken_ = get();
    ParseExpr();
    if (curToken_.symbol != L":") {
      ThrowException("Expected :");
    }

    curToken_ = get();
    if (curToken_.symbol == L"case" || curToken_.symbol == L"default" ||
        curToken_.symbol == L"}") {
      continue;
    }
    ParseMultipleStatementsInCase();
  }
  if (curToken_.symbol == L"default") {
    curToken_ = get();
    if (curToken_.symbol != L":") {
      ThrowException("Expected :");
    }

    curToken_ = get();
    ParseMultipleStatementsInCase();
  }
  if (curToken_.symbol != L"}") {
    ThrowException("Expected }");
  }
  curToken_ = get();
}

void Parser::ParseWhile() {
  // we already have while
  curToken_ = get();
  if (curToken_.symbol != L"(") {
    ThrowException("Expected opening brackets");
  }

  curToken_ = get();
  ParseExpr();
#ifdef SEMANTIC
  TypeAttribute* type =
      dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  if (!CanCast(L"bool", type->type)) {
    ThrowException("Unable to cast");
  }
  semantic_->PopAttribute();
  type = dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  semantic_->PushAttribute(new JumpAttribute(type));
#endif
  if (curToken_.symbol != L")") {
    ThrowException("Expected closing brackets");
  }

  curToken_ = get();
  ParseBody();
  if (curToken_.symbol == L"else") {
    curToken_ = get();
    ParseBody();
  }
}

void Parser::ParseFor() {
  // we already have for
#ifdef SEMANTIC
  semantic_->CreateNewTID();
 //TypeAttribute* type = dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
 //if (!type) {
 //  JumpAttribute* jump =
 //      dynamic_cast<JumpAttribute*>(semantic_->TopStackAttribute());
 //  if (!jump) ThrowException("Where?");
 //  type = jump->return_type_ptr;
 //}
 //semantic_->PushAttribute(new JumpAttribute(type));
#endif
  curToken_ = get();
  if (curToken_.symbol != L"(") {
    ThrowException("Expected opening bracket");
  }

  curToken_ = get();
  if (IsType(curToken_) || curToken_.symbol == L"const") {
    ParseVarDefs();
  } else {
    if (curToken_.symbol != L";") {
      ParseVarDefs();
    }
  }
  if (curToken_.symbol != L";") {
    ThrowException("Expected ;");
  }

  curToken_ = get();
  if (curToken_.symbol != L";") {
    ParseExpr();
#ifdef SEMANTIC
    TypeAttribute* type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (!CanCast(L"bool", type->type)) {
      ThrowException("Unable to cast");
    }
    semantic_->PopAttribute();
#endif
  }
  // bruh moment, but we need it
  if (curToken_.symbol != L";") {
    ThrowException("Expected ;");
  }

  curToken_ = get();
  if (curToken_.symbol != L")") {
    ParseExpr();
  }

  if (curToken_.symbol != L")") {
    ThrowException("Expected closing bracket");
  }
  curToken_ = get();

  ParseBody();
#ifdef SEMANTIC
  semantic_->RemoveCurrentTID();
#endif
  if (curToken_.symbol == L"else") {
    curToken_ = get();
#ifdef SEMANTIC
    semantic_->CreateNewTID();
#endif
    ParseBody();
#ifdef SEMANTIC
    semantic_->RemoveCurrentTID();
#endif
  }
}

void Parser::ParseDowhile() {
  // we already have do
  curToken_ = get();
#ifdef SEMANTIC
  semantic_->CreateNewTID();
  TypeAttribute* ret_type =
      dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  semantic_->PushAttribute(new JumpAttribute(ret_type));
#endif
  ParseBody();
#ifdef SEMANTIC
  semantic_->RemoveCurrentTID();
#endif
  if (curToken_.symbol != L"while") {
    ThrowException("Expected while");
  }

  curToken_ = get();
  if (curToken_.symbol != L"(") {
    ThrowException("Expected opening brackets");
  }

  curToken_ = get();
  ParseExpr();
#ifdef SEMANTIC
  TypeAttribute* type =
      dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  if (!CanCast(L"bool", type->type)) {
    ThrowException("Unable to cast");
  }
  semantic_->PopAttribute();
#endif
  if (curToken_.symbol != L")") {
    ThrowException("Expected closing brackets");
  }

  curToken_ = get();
}

void Parser::ParseGoto() {
  if (curToken_.symbol == L"break") {
  #ifdef SEMANTIC
    JumpAttribute* jmpattr = dynamic_cast<JumpAttribute*>(semantic_->TopStackAttribute());
    if (!jmpattr) {
      ThrowException("Can`t break from nothing");
    }
  #endif
    curToken_ = get();
    return;
  }
  if (curToken_.symbol == L"continue") {
#ifdef SEMANTIC
    JumpAttribute* jmpattr =
        dynamic_cast<JumpAttribute*>(semantic_->TopStackAttribute());
    if (!jmpattr) {
      ThrowException("Can`t continue from nothing");
    }
#endif
    curToken_ = get();
    return;
  }
  if (curToken_.symbol == L"goto") {
    curToken_ = get();
    if (curToken_.type != Token::Type::IDENTIFIER) {
      ThrowException("Expected identifier");
    }
    curToken_ = get();
    return;
  }
  if (curToken_.symbol == L"label") {
    ParseLabelDef();
    return;
  }
  if (curToken_.symbol == L"return") {
    ParseReturn();
    return;
  }
  ThrowException("Unexpected reserved token");
}

void Parser::ParseReturn() {
  // we already have return
  curToken_ = get();
  #ifdef SEMANTIC
  //TypeAttribute* expected =
  //    dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  //if (!expected) {
  //  JumpAttribute* jmpattr = dynamic_cast<JumpAttribute*>(semantic_->TopStackAttribute());
  //  if (!jmpattr) ThrowException("Unable to return from nothing");
  //  expected = jmpattr->return_type_ptr;
  //}
  #endif
  if (curToken_.symbol != L";") {
    ParseExpr();
  }
#ifdef SEMANTIC
 //TypeAttribute* type =
 //    dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
 //if (!CanCast(expected->type, type->type)) {
 //  ThrowException("Unable to cast");
 //}
  semantic_->PopAttribute();
#endif
}

void Parser::ParseTypeInstance() {
  if (curToken_.type != Token::Type::IDENTIFIER) {
    ThrowException("Expected identifier");
  }
#ifdef SEMANTIC
  TypeAttribute* type = new TypeAttribute(curToken_.symbol, false, false);
#endif
  curToken_ = get();
  while (curToken_.symbol == L"@") {
#ifdef SEMANTIC
    type->type.append(curToken_.symbol);
#endif
    curToken_ = get();
  }

  while (curToken_.symbol == L"[") {
#ifdef SEMANTIC
    type->type.append(curToken_.symbol);
#endif
    curToken_ = get();
    ParseAssExpr();
    while (curToken_.symbol == L",") {
#ifdef SEMANTIC
      type->type.append(curToken_.symbol);
#endif
      curToken_ = get();
      ParseAssExpr();
    }
    if (curToken_.symbol != L"]") {
      ThrowException("Expected closing rect closing bracket");
    }
#ifdef SEMANTIC
    type->type.append(curToken_.symbol);
#endif
    curToken_ = get();
  }
#ifdef SEMANTIC
  semantic_->PushAttribute(type);
#endif
}

void Parser::ParseLabelDef() {
  // we already have label
  curToken_ = get();
  if (curToken_.type != Token::Type::IDENTIFIER) {
    ThrowException("Expected identifier");
  }
  curToken_ = get();
}

void Parser::ParseElseAlternatives() {
  if (curToken_.symbol == L"else") {
    curToken_ = get();
    ParseBody();
    return;
  }
  if (curToken_.symbol == L"elif") {
    curToken_ = get();
    if (curToken_.symbol != L"(") {
      ThrowException("Expected opening bracket");
    }

    curToken_ = get();
    ParseExpr();
#ifdef SEMANTIC
    TypeAttribute* type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (!CanCast(L"bool", type->type)) {
      ThrowException("Unable to cast");
    }
    semantic_->PopAttribute();
#endif
    if (curToken_.symbol != L")") {
      ThrowException("Expected closing bracket");
    }

    curToken_ = get();
    ParseBody();
    ParseElseAlternatives();
    return;
  }
  // else we are in some kind of garbage, that doesn`t belong to us
  // get out of here
}
