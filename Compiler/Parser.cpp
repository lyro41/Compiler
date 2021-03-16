#include "Parser.h"

Parser::Parser(LexicAnalyzer* analyzer) : analyzer_(analyzer) {
  type_set_.insert(L"int");
  type_set_.insert(L"float");
  type_set_.insert(L"double");
  type_set_.insert(L"char");
  type_set_.insert(L"string");
  type_set_.insert(L"let");
}

void Parser::Parse() { ParseProgram(); }

Token Parser::get() { 
  return analyzer_->GetToken();
}

bool Parser::IsType(Token token) {
  return type_set_.find(token.symbol) != type_set_.end();
}

bool Parser::IsAssignmentOperator(Token token) {
  return token.symbol == L"="   ||
         token.symbol == L"+="  ||
         token.symbol == L"-="  ||
         token.symbol == L"/="  ||
         token.symbol == L"//=" ||
         token.symbol == L"%="  ||
         token.symbol == L"**=" ||
         token.symbol == L">>=" ||
         token.symbol == L"<<=" ||
         token.symbol == L"->=" ||
         token.symbol == L"^="  ||
         token.symbol == L"&="  ||
         token.symbol == L"|=";
}

bool Parser::IsPrefixUnaryOperator(Token token) {
  return token.symbol == L"++" ||
         token.symbol == L"--" ||
         token.symbol == L"~" ||
         token.symbol == L"!" ||
         token.symbol == L"not" ||
         token.symbol == L"+" ||
         token.symbol == L"-" ||
         token.symbol == L"@" ||
         token.symbol == L"?" ||
         token.symbol == L"delete" ||
         token.symbol == L"new";
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
  if (type_set_.find(token_sym) != type_set_.end()) {
    ThrowException("Struct redefinition");
  }
  type_set_.insert(token_sym);
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
    if (curToken_.type == Token::Type::IDENTIFIER) {
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
}

void Parser::ParseStructDefinition() {
  // we already have struct
  curToken_ = get();
  if (curToken_.type != Token::Type::IDENTIFIER) {
    ThrowException("Expected identifier");
  }
  
  AddType(curToken_.symbol);
  curToken_ = get();
  if (curToken_.symbol != L"{") {
    ThrowException("Expected curly opening bracket");
  }
  
  curToken_ = get();
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
    if (curToken_.type == Token::Type::IDENTIFIER) {
      ParseVarDefs();
      if (curToken_.symbol != L";") {
        ThrowException("Expected ;");
      }
      curToken_ = get();
      shouldLeave = false;
    }
  }
  
  if (curToken_.symbol != L"}") {
    ThrowException("Expected curly closing bracket");
  }
  
  curToken_ = get();
  if (curToken_.symbol != L";") {
    ParseVars();
  }
  
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

  curToken_ = get();
  if (curToken_.symbol != L"(") {
    ThrowException("Expected opening brackets");
  }

  curToken_ = get();
  ParseArguments();

  if (curToken_.symbol != L")") {
    ThrowException("Expected closing brackets");
  }

  curToken_ = get();
  if (curToken_.symbol != L":") {
    ThrowException("Expected ':'");
  }

  curToken_ = get();
  ParseConcreteType();
  if (curToken_.symbol != L";") {
    // If it`s not forward declaration
    ParseFuncbody();
  }
}

void Parser::ParseType() {
  if (curToken_.symbol == L"const") {
    ParseConstType();
  } else {
    ParseConcreteType();
  }
}

void Parser::ParseConcreteType() {
  if (curToken_.type != Token::Type::IDENTIFIER && 
      curToken_.type != Token::Type::RESERVED) {
    ThrowException("Expected identifier or reserved");
  }
  
  curToken_ = get();
  while (curToken_.symbol == L"@") {
    curToken_ = get();
  }
  
  while (curToken_.symbol == L"[") {
    curToken_ = get();
    while (curToken_.symbol == L",") {
      curToken_ = get();
    }
    
    if (curToken_.symbol != L"]") {
      ThrowException("Expected closing rect closing bracket");
    }
    curToken_ = get();
  }
}

void Parser::ParseConstType() {
  // curToken_ already contains const
  curToken_ = get();
  ParseConcreteType();
}

void Parser::ParseArguments() {
  if (curToken_.symbol == L")") return;
  if (curToken_.symbol == L"void") {
    curToken_ = get();
    return;
  }
  ParseType();

  if (curToken_.type != Token::Type::IDENTIFIER) {
    if (curToken_.type == Token::Type::RESERVED) {
      ThrowException("Misuse of reserved word");
    } else {
      ThrowException("Identifier expected");
    }
  }

  curToken_ = get();
  while (curToken_.symbol == L",") {
    curToken_ = get();
    ParseType();

    curToken_ = get();
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
    ParseFuncbody();
    return;
  }
  
  ParseExpr();
  if (curToken_.symbol != L";") {
    ThrowException("Expected ;");
  }
  
  curToken_ = get();
}

void Parser::ParseMultipleStatements() {
  // Used only in '{' <Statement> '}'
  while (curToken_.symbol != L"}" ) {
    if (curToken_.type == Token::Type::ENDOFFILE) {
      ThrowException("Expected statement or }, got end of file");
    }
    ParseStatement();
  }
}

void Parser::ParseMultipleStatementsInCase() {
  // Used only in '{' <Statement> '}'
  while (curToken_.symbol != L"}" && 
         curToken_.symbol != L"case" && 
         curToken_.symbol != L"default") {
    ParseStatement();
  }
}

void Parser::ParseVarDefs() {
  // we have const or type
  ParseType();
  ParseVars();
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
  curToken_ = get();
  if (curToken_.symbol == L"=") {
    curToken_ = get();
    ParseExpr();
  }
}

void Parser::ParseExpr() {
  ParseAssExpr();
  while (curToken_.symbol == L",") {
    curToken_ = get();
    ParseAssExpr();
  }
}

void Parser::ParseAssExpr() {
  ParseLogImp();
  if (IsAssignmentOperator(curToken_)) {
    curToken_ = get();
    ParseInitExpr();
  }
}

void Parser::ParseInitExpr() {
  if (curToken_.symbol == L"{") {
    // Init List
    curToken_ = get();
    ParseAssExpr();
    while (curToken_.symbol == L",") {
      // Init list seq
      curToken_ = get();
      ParseAssExpr();
    }
  } else {
    ParseAssExpr();
  }
}

void Parser::ParseLogImp() {
  ParseLogOr();
  if (curToken_.symbol == L"->") {
    curToken_ = get();
    ParseLogImp();
  }
}

void Parser::ParseLogOr() {
  ParseLogAnd();
  if (curToken_.symbol == L"||" || 
      curToken_.symbol == L"or") {
    curToken_ = get();
    ParseLogOr();
  }
}

void Parser::ParseLogAnd() {
  ParseBitOr();
  if (curToken_.symbol == L"&&" || curToken_.symbol == L"and") {
    curToken_ = get();
    ParseLogAnd();
  }
}

void Parser::ParseBitOr() {
  ParseBitXor();
  if (curToken_.symbol == L"|") {
    curToken_ = get();
    ParseBitOr();
  }
}

void Parser::ParseBitXor() {
  ParseBitAnd();
  if (curToken_.symbol == L"^" || curToken_.symbol == L"xor") {
    curToken_ = get();
    ParseBitXor();
  }
}

void Parser::ParseBitAnd() {
  ParseCompeq();
  if (curToken_.symbol == L"&") {
    curToken_ = get();
    ParseBitAnd();
  }
}

void Parser::ParseCompeq() {
  ParseCompcomp();
  if (curToken_.symbol == L"==" || curToken_.symbol == L"!=") {
    curToken_ = get();
    ParseCompeq();
  }
}

void Parser::ParseCompcomp() {
  ParseShift();
  if (curToken_.symbol == L">=" || curToken_.symbol == L"<=" ||
      curToken_.symbol == L">" || curToken_.symbol == L"<") {
    curToken_ = get();
    ParseCompcomp();
  }
}

void Parser::ParseShift() {
  ParseSumsub();
  if (curToken_.symbol == L">>" || curToken_.symbol == L"<<") {
    curToken_ = get();
    ParseShift();
  }
}

void Parser::ParseSumsub() {
  ParseMuldiv();
  if (curToken_.symbol == L"+" || curToken_.symbol == L"-") {
    curToken_ = get();
    ParseSumsub();
  }
}

void Parser::ParseMuldiv() {
  ParsePower();
  if (curToken_.symbol == L"//" || curToken_.symbol == L"/" ||
      curToken_.symbol == L"*" || curToken_.symbol == L"%") {
    curToken_ = get();
    ParseMuldiv();
  }
}

void Parser::ParsePower() {
  ParseUnary();
  if (curToken_.symbol == L"**") {
    curToken_ = get();
    ParsePower();
  }
}

void Parser::ParseUnary() {
  if (curToken_.symbol == L"new") {
    curToken_ = get();
    ParseTypeInstance();
    return;
  }
  
  while (IsPrefixUnaryOperator(curToken_)) {
    curToken_ = get();
  }
  ParseGensec();
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
  bool shouldLeave = false;
  while (!shouldLeave) {
    shouldLeave = true;
    if (IsPostfixUnaryOperator(curToken_)) {
      shouldLeave = false;
      curToken_ = get();
    }
    if (curToken_.symbol == L"(") {
      // call of function
      shouldLeave = false;
      curToken_ = get();
      if (curToken_.symbol != L")") {
        ParseAssExpr();
        while (curToken_.symbol == L",") {
          curToken_ = get();
          ParseAssExpr();
        }
      }
      // bruh
      if (curToken_.symbol != L")") {
        ThrowException("Expected closing bracket");
      }
      curToken_ = get();
    }
    if (curToken_.symbol == L"[") {
      // indexing
      shouldLeave = false;
      curToken_ = get();
      ParseAssExpr();
      while (curToken_.symbol == L",") {
        curToken_ = get();
        ParseAssExpr();
      }
      if (curToken_.symbol != L"]") {
        ThrowException("Expected closing rect bracket");
      }
      curToken_ = get();
    }
    if (curToken_.symbol == L".") {
      shouldLeave = false;
      curToken_ = get();
      ParseAttribute();
    }
  }
}

void Parser::ParseNamespace() { 
  if (curToken_.type == Token::Type::IDENTIFIER) {
    Token buffer = curToken_;
    curToken_ = get();
    if (curToken_.symbol == L"$") {
      ParseNestedNamespace();
    }
    // buffer is a variable;
    // curtoken is next token!
    return;
  }
  ParseOperand();
}

void Parser::ParseNestedNamespace() { 
  while (curToken_.symbol == L"$") {
    curToken_ = get();
    if (curToken_.type != Token::Type::IDENTIFIER) {
      ThrowException("Expected identifier");
    }
    Token buffer = curToken_;
    curToken_ = get();
  }
  // we left, it`s variable
  
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
      curToken_.symbol != L"NULL" && 
      curToken_.symbol != L"NIL") {
    ThrowException("Expected any constant");
  }
  curToken_ = get();
}

void Parser::ParseAttribute() { 
  if (curToken_.type != Token::Type::IDENTIFIER) {
    ThrowException("Expected identifier");
  }
  
  curToken_ = get();
  bool shouldLeave = false;
  while (!shouldLeave) {
    shouldLeave = true;
    if (curToken_.symbol == L"(") {
      // call of function
      shouldLeave = false;
      curToken_ = get();
      if (curToken_.symbol != L")") {
        ParseAssExpr();
        while (curToken_.symbol == L",") {
          curToken_ = get();
          ParseAssExpr();
        }
      }
      // bruh
      if (curToken_.symbol != L")") {
        ThrowException("Expected closing bracket");
      }
      curToken_ = get();
    }
    if (curToken_.symbol == L"[") {
      // indexing
      shouldLeave = false;
      curToken_ = get();
      ParseAssExpr();
      while (curToken_.symbol == L",") {
        curToken_ = get();
        ParseAssExpr();
      }
      if (curToken_.symbol != L"]") {
        ThrowException("Expected closing rect bracket");
      }
      curToken_ = get();
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
  if (curToken_.symbol != L")") {
    ThrowException("Expected closing bracket");
  }
  
  curToken_ = get();
  if (curToken_.symbol != L"{"){
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
  curToken_ = get();
  if (curToken_.symbol != L"("){
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
  if (curToken_.symbol != L";")
  {
    ParseExpr();
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
  if (curToken_.symbol == L"else") {
    curToken_ = get();
    ParseBody();
  }
}

void Parser::ParseDowhile() {
  // we already have do
  curToken_ = get();
  ParseBody();
  if (curToken_.symbol != L"while") {
    ThrowException("Expected while");
  }
  
  curToken_ = get();
  if (curToken_.symbol != L"(") {
    ThrowException("Expected opening brackets");
  }
  
  curToken_ = get();
  ParseExpr();
  if (curToken_.symbol != L")") {
    ThrowException("Expected closing brackets");
  }
  
  curToken_ = get();
}

void Parser::ParseGoto() { 
  if (curToken_.symbol == L"break") {
    curToken_ = get();
    return;
  }
  if (curToken_.symbol == L"continue") {
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
  if (curToken_.symbol != L";") {
    ParseExpr();
  }
}

void Parser::ParseTypeInstance() {
  if (curToken_.type != Token::Type::IDENTIFIER) {
    ThrowException("Expected identifier");
  }
  
  curToken_ = get();
  while (curToken_.symbol == L"@") {
    curToken_ = get();
  }
  
  while (curToken_.symbol == L"[") {
    curToken_ = get();
    ParseAssExpr();
    while (curToken_.symbol == L",") {
      curToken_ = get();
      ParseAssExpr();
    }
    
    if (curToken_.symbol != L"]") {
      ThrowException("Expected closing rect closing bracket");
    }
    curToken_ = get();
  }
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
