#include "Parser.h"

Parser::Parser(LexicAnalyzer* analyzer) : analyzer_(analyzer) {}

void Parser::Parse() {ParseProgram();}

Token Parser::get() { 
  if (analyzer_->HasNext()) {
    return analyzer_->GetToken();
  } else {
    throw std::runtime_error("Unexpected end of tokens");
  }
}

bool Parser::IsType(Token token) {
  return   token.symbol == L"int"
        || token.symbol == L"float"
        || token.symbol == L"string";
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
         token.symbol == L"--";
}

bool Parser::IsPostfixUnaryOperator(Token token) {
  return token.symbol == L"++" ||
         token.symbol == L"--" ||
         token.symbol == L"~" ||
         token.symbol == L"!" ||
         token.symbol == L"not" ||
         token.symbol == L"+" ||
         token.symbol == L"-" ||
         token.symbol == L"@" ||
         token.symbol == L"?";
}

void Parser::ThrowException(std::string message) {
  // to do
  std::cout << analyzer_->GetCursorPosition().first << 
            " " << analyzer_->GetCursorPosition().second << "\n";
  throw std::runtime_error(message.c_str());
}

void Parser::ParseProgram() { 
  curToken_ = get();
  ParsePreprocessor();
  ParseFunction();
  if (analyzer_->HasNextToken()) {
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

void Parser::ParseFunction() {  
  while (curToken_.symbol == L"func") {
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
    // else go find another function
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
  if (curToken_.type != Token::Type::IDENTIFIER
      && curToken_.type != Token::Type::RESERVED) {
    ThrowException("Expected identifier or reserved");
  }
  curToken_ = get();
  while (curToken_.symbol == L"@") {
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
  while (curToken_.symbol != L",") {
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

void Parser::ParseFuncbody() {
  if (curToken_.symbol != L"{") {
    ThrowException("Expected opening curly bracket");
  }
  curToken_ = get();
  ParseMultipleStatements();
  if (curToken_.symbol != L"}") {
    ThrowException("Expected closing curly bracket");
  }
  // potentionally dangerous!
  if (analyzer_->HasNextToken()) {
    curToken_ = get();
  }
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
    curToken_ = get();
    ParseMultipleStatements();
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
    ParseStatement();
  }
}

void Parser::ParseMultipleStatementsInCase() {
  // Used only in '{' <Statement> '}'
  while (curToken_.symbol != L"}" 
         || curToken_.symbol != L"case"
         || curToken_.symbol != L"default") {
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
  while (curToken_.symbol == L",")
  { 
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

void Parser::ParseExprList() {
  
  ParseExpr();
  while (curToken_.symbol == L",") {
    curToken_ = get();
    ParseExpr();
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
  if (curToken_.symbol == L"||"
      || curToken_.symbol == L"or") {
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

  ParseBitAnd();
  if (curToken_.symbol == L"|") {
    curToken_ = get();
    ParseBitOr();
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
  if (curToken_.symbol == L">=" || curToken_.symbol == L"=<" ||
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
      // calling function
      shouldLeave = false;
      curToken_ = get();
      if (curToken_.symbol != L")") {
        ParseExprList();
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
      ParseExprList();
      if (curToken_.symbol != L"]") {
        ThrowException("Expected closing rect bracket");
      }
      curToken_ = get();
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
    // buffer it`s a variable;
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
  // we left, its variable
  curToken_ = get();
}

void Parser::ParseOperand() {

  if (curToken_.symbol == L"(") {
    curToken_ = get();
    ParseExpr();
    if (curToken_.symbol != L")") {
      ThrowException("Expected closing brackets");
    }
    return;
  }
  if (curToken_.type != Token::Type::LITCONSTANT
      && curToken_.type != Token::Type::NUMCONSTANT) {
    ThrowException("Expected any constant");
  }
  curToken_ = get();
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
  if (curToken_.symbol == L"else") {
    curToken_ = get();
    ParseBody();
  }
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
    ParseMultipleStatementsInCase();
  }

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
  if (IsType(curToken_)
      || curToken_.symbol == L"const") {
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
  if (curToken_.symbol == L"return") {
    ParseReturn();
    return;
  }
}

void Parser::ParseReturn() {
  // we already have return
  curToken_ = get();
  if (curToken_.symbol != L";") {
    ParseExpr();
  }
}
