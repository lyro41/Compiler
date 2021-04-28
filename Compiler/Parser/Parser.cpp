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
    return lhs == rhs;
  }
  if (lhs == L"bool") {
    return CanConvertToBool(rhs);  
  }
  if (rhs == L"bool") { 
    return CanConvertToBool(lhs);
  }
  try {
    CheckIntegral(lhs); 
    CheckIntegral(rhs);
  } catch (...) {
    return lhs == rhs;
  }
  return true;
}

bool Parser::CanConvertToBool(std::wstring rhs) { 
  if (rhs == L"int8" || rhs == L"uint8" || rhs == L"char" || rhs == L"int16" ||
      rhs == L"uint16" || rhs == L"int32" || rhs == L"uint32" ||
      rhs == L"int64" || rhs == L"uint64" || rhs == L"float" ||
      rhs == L"double" || rhs == L"bool") {
    return true;
  }
  return false;
}

void Parser::CheckInts(std::wstring lhs, std::wstring rhs) { 
  if (!IsIntegerType(lhs))
    ThrowException("Left side value must be int");
  if (!IsIntegerType(rhs))
    ThrowException("Right side value must be int");
}

void Parser::CheckInt(std::wstring lhs) {
  if (!IsIntegerType(lhs))
    ThrowException("Left side value must be int");
}

void Parser::CheckIntegral(std::wstring lhs) {
  if(!IsIntegerType(lhs) && lhs != L"float" && lhs != L"double" && lhs != L"CFLOAT")
    ThrowException("Expected integral type");

}

bool Parser::DoTypesMatch(std::wstring lhs, std::wstring rhs) { 
  if (lhs.find(L"int") != lhs.npos) { 
    return lhs == rhs || rhs == L"CINTEGER";
  }
  if (lhs == L"float") { 
    return lhs == rhs || rhs == L"CFLOAT";
  }
  if (lhs == L"double") { 
    return lhs == rhs || rhs == L"CFLOAT";
  } 
  
  if (rhs.find(L"int") != rhs.npos) { 
    return rhs == lhs || lhs == L"CINTEGER";
  }
  if (rhs == L"float") { 
    return rhs == lhs || lhs == L"CFLOAT";
  }
  if (rhs == L"double") { 
    return rhs == lhs || lhs == L"CFLOAT";
  }

  return lhs == rhs;
}

bool Parser::IsIntegerType(std::wstring type) {
return type == L"int8" ||
       type == L"uint8" ||
       type == L"char" ||
       type == L"int16" ||
       type == L"uint16" ||
       type == L"int32" ||
       type == L"uint32" ||
       type == L"int64" ||
       type == L"uint64" ||
       type == L"CINTEGER" ||
       type == L"CHEX";
}

void Parser::DeclareJumpableConstruction() {
  TypeAttribute* type =
      dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  if (!type) {
    JumpAttribute* jump =
        dynamic_cast<JumpAttribute*>(semantic_->TopStackAttribute());
    if (!jump) ThrowException("Where?");
    type = jump->return_type_ptr;
  }
  semantic_->PushAttribute(new JumpAttribute(type));
}

TypeAttribute* Parser::TryCast(TypeAttribute* lhs, TypeAttribute* rhs) { 
  if (CanCast(lhs->type, rhs->type)) {
    return new TypeAttribute(lhs->type, false, false);
  }
  if (lhs->type == rhs->type) {
    TypeAttribute* ret = lhs->Clone();
    ret->is_lrvalue = false;
    return ret;
  }

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
  //if (type_map.find(token_sym) != type_map.end()) {
  //  ThrowException("Struct redefinition");
  //}
  type_map[token_sym] = -1;
}

void Parser::ParseProgram() {

  curToken_ = get();
  ParsePreprocessor();
  ParseGlobalStatement();
  if (curToken_.type != Token::Type::ENDOFFILE) {
    ThrowException("Unexpected token");
  }
#ifdef SEMANTIC
  semantic_->CheckPrototypes();
#endif
#ifdef GENERATOR
  for (size_t i = 0; i < generator_->program.size(); ++i) {
      std::wcout << i << ":" << generator_->program[i]->GetType();
      if (dynamic_cast<JumpElseItem*>(generator_->program[i])) {
          std::wcout << " index: " << dynamic_cast<JumpElseItem*>(generator_->program[i])->jump_ind_;
      } else if (dynamic_cast<JumpItem*>(generator_->program[i])) {
          std::wcout << " index: " << dynamic_cast<JumpItem*>(generator_->program[i])->jump_ind_;
      } else if (dynamic_cast<LabelItem*>(generator_->program[i])) {
          std::wcout << " index: " << dynamic_cast<LabelItem*>(generator_->program[i])->name_;
      }
      std::wcout << std::endl;
  }
#endif // GENERATOR
}

void Parser::ParsePreprocessor() {
  while (curToken_.symbol == L"import") {
    curToken_ = get();
    if (curToken_.type != Token::Type::LITCONSTANT) {
      ThrowException("Token type of literal constant expected");
    }
    #ifdef SEMANTIC
    semantic_->TryToParseFile(curToken_.symbol);
    #endif
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
  //curTID->parent =
  //    semantic_->GetCurrentTID();
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
    auto this_ptr = new VariableTIDEntry(new TypeAttribute(curToken_.symbol + L"@", false, false));
    this_ptr->name = L"this";
    semantic_->PushInCurrentTID(this_ptr);
#endif
#ifdef GENERATOR
    NameTable* n_table = new NameTable(curToken_.symbol, NameTable::TableType::STRUCT);
    generator_->current_table->add_son(n_table);
    generator_->current_table = n_table;
    n_table->jump_in = generator_->GetCurrentCursor() + 1;
#endif // GENERATOR


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
 //curTID->parent =
 //    semantic_->GetCurrentTID();
  semantic_->PushInCurrentTID(str_entry);
  semantic_->PushAttribute(new StructAttribute(str_entry->name, false, true, curTID));
#endif
#ifdef GENERATOR
  generator_->PushItemToProgram(new JumpItem(true, 0));
  generator_->current_table->update_size();
  generator_->current_table = generator_->current_table->parrent;
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
#ifdef GENERATOR
  generator_->PushItemToProgram(new ScopeBeginItem());
  generator_->should_skip = true;
#endif // GENERATOR

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
    TID* curTID = semantic_->GetCurrentTID();
    semantic_->CreateNewTID();
    while (!semantic_->AttributeStackEmpty()) {
      ArgumentAttribute* arg_attr =
          dynamic_cast<ArgumentAttribute*>(semantic_->TopStackAttribute());
      if (!arg_attr) ThrowException("Semantic Analyzer expected Argument");

      VariableTIDEntry* var_entry = new VariableTIDEntry(arg_attr->type.Clone());
      var_entry->name = arg_attr->name;
      func_entry->type_arguments.push_back(arg_attr->type);
      semantic_->PushInCurrentTID(var_entry);
      semantic_->PopAttribute();
      //semantic_->PushAttribute(new TypeAttribute(func_entry->return_type, false , false));
    }
    func_entry->is_proto = false;
    curTID->PushInTID(func_entry);
    semantic_->PushAttribute(
        new TypeAttribute(func_entry->return_type, false, false));
#endif
    ParseFuncbody();
    

#ifdef SEMANTIC
    if (!semantic_->AttributeStackEmpty()) semantic_->PopAttribute();
    semantic_->ResolveGotos();
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
#ifdef SEMANTIC
  StructTIDEntry* str_def = nullptr;
  if (curToken_.symbol == L"$") {
    str_def =
        dynamic_cast<StructTIDEntry*>(semantic_->GetCurrentTID()->FindByName(type));
    if (!str_def) {
      ThrowException("Expected struct type name");
    }
    semantic_->StartNamespaceParse();
    semantic_->ChangeCurrentTID(str_def->GetCurrentTID());
    Token buffer = get();
    curToken_ = get();
    str_def = dynamic_cast<StructTIDEntry*>(
        semantic_->GetCurrentTID()->FindByName(buffer.symbol));
    if (!str_def) {
      ThrowException("Expected struct type name");
    }
    type.append(L"$" + buffer.symbol);
    while (curToken_.symbol == L"$") {
      buffer = get();
      str_def = dynamic_cast<StructTIDEntry*>(
          semantic_->GetCurrentTID()->FindByName(buffer.symbol));
      if (!str_def) {
        ThrowException("Expected struct type name");
      }
      type.append(L"$"+buffer.symbol);
      curToken_ = get();
    }

  }
#endif
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
  if (str_def) {
    StructAttribute* str_var = new StructAttribute(
        type, false, true, str_def->GetCurrentTID());
    if (type.find(L"@") != type.npos || type.find(L"[") != type.npos) {
      str_var->is_ptr = true;
    }
    semantic_->PushAttribute(str_var);
    return;
  }
  ITIDEntry* entry =
      semantic_->GetCurrentTID()->FindByName(type);
  StructTIDEntry* struct_def = dynamic_cast<StructTIDEntry*>(entry);
  TypeAttribute* type_attr;
  if (struct_def) {
    type_attr = new StructAttribute(type, false, true, struct_def->GetCurrentTID());  
  } else {
    type_attr = new TypeAttribute(type, false, true);
  }
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
#ifdef GENERATOR
  generator_->PushItemToProgram(new ScopeBeginItem());
#endif // GENERATOR

  curToken_ = get();
  ParseMultipleStatements();
  if (curToken_.symbol != L"}") {
    ThrowException("Expected closing curly bracket");
  }

#ifdef GENERATOR
  generator_->PushItemToProgram(new ScopeEndItem());
#endif // GENERATOR


  curToken_ = get();
}

void Parser::ParseBody() {
#ifdef GENERATOR
    Item* it = new Item;
    it->SetType(L"ParseBody");
    generator_->PushItemToProgram(it);
#endif // GENERATOR
  if (curToken_.symbol == L"{") {
    ParseFuncbody();
  } else {
#ifdef GENERATOR
      generator_->PushItemToProgram(new ScopeBeginItem());
#endif // GENERATOR

    ParseStatement();
#ifdef GENERATOR
    generator_->PushItemToProgram(new ScopeEndItem());
#endif // GENERATOR

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
#ifdef GENERATOR
    Item* it = new Item;
    it->SetType(L"parseVarDefs");
    generator_->PushItemToProgram(it);
#endif // GENERATOR

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

  VariableTIDEntry* var_entry = new VariableTIDEntry(type_attr->Clone());
  var_entry->name = curToken_.symbol;
  semantic_->PushInCurrentTID(var_entry);
#endif
#ifdef GENERATOR
  
#endif // GENERATOR

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
#ifdef GENERATOR
    Item* it = new Item;
    it->SetType(L"ParseExpr");
    generator_->PushItemToProgram(it);
#endif // GENERATOR
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
      std::wstring base_type = type_attr->type;
      auto iter = type_attr->type.begin();
      for (; iter != type_attr->type.end(); ++iter) {
        if (*iter == L'@') {
          break;
        }
        if (*iter == L'[') {
          break;
        }
      }
      base_type = type_attr->type.substr(0, (iter - type_attr->type.begin()));
      std::wstring suffix =
          type_attr->type.substr((iter - type_attr->type.begin()));
      suffix.insert(suffix.begin(), L'@');
      type_attr->type = base_type + suffix;
      type_attr->is_constant = false;
      type_attr->is_lrvalue = false;
    }
    if (tok.symbol == L"@") {
      bool found_ptr = false;
      auto iter =
          type_attr->type.begin();
      for (; iter != type_attr->type.end();
           ++iter) {
        if (*iter == L'@') {
          type_attr->type.erase(iter, iter + 1);
          found_ptr = true;
          break;
        }
        if (*iter == L'[') {
          auto siter = iter;
          found_ptr = true;
          for (; *siter != L']'; ++siter);
          type_attr->type.erase(iter, siter);
          break;
        }
      }
      
      if (!found_ptr) {
        ThrowException("Only pointer type can be dereferenced");
      }
      StructAttribute* str_attr =
          dynamic_cast<StructAttribute*>(type_attr);
      if (str_attr) {
        if (str_attr->type.find('@') == str_attr->type.npos
            && str_attr->type.find('[') == str_attr->type.npos)
        {
          str_attr->is_ptr = false;
        }
      }
      type_attr->is_constant = false;
      type_attr->is_lrvalue = true;
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
#ifdef SEMANTIC
    TypeAttribute* to_cast =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
#endif
    if (curToken_.symbol != L">") {
      ThrowException("Expected closing triangle bracket");
    }

    curToken_ = get();
    if (curToken_.symbol != L"(") {
      ThrowException("Expected opening bracket");
    }
    curToken_ = get();
    ParseExpr();
#ifdef SEMANTIC
    TypeAttribute* from_cast =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
#endif
    if (curToken_.symbol != L")") {
      ThrowException("Expected closing bracket");
    }
    curToken_ = get();
#ifdef SEMANTIC
    TypeAttribute* result = TryCast(to_cast, from_cast);
    semantic_->PopAttribute();
    semantic_->PopAttribute();
    semantic_->PushAttribute(result);
#endif
    return;
  }

  ParseNamespace();
  #ifdef SEMANTIC
  semantic_->StartNamespaceParse();
  #endif
  ParsePostfixOperations();
#ifdef SEMANTIC
  semantic_->EndNamespaceParse();
#endif
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
        semantic_->PushAttribute(var->type->Clone());
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
        semantic_->PushAttribute(var->type->Clone());
      } else {
        FunctionTIDEntry* func = dynamic_cast<FunctionTIDEntry*>(entry);
        if (func) {
          semantic_->PushAttribute(new FunctionAttribute(curToken_.symbol));
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
        semantic_->PushAttribute(var->type->Clone());
      } else {
        FunctionTIDEntry* func = dynamic_cast<FunctionTIDEntry*>(entry);
        if (func) {
          semantic_->PushAttribute(new FunctionAttribute(func->name));
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
      curToken_.symbol != L"NULL" && curToken_.symbol != L"NIL" &&
      curToken_.symbol != L"true" && curToken_.symbol != L"false") {
    ThrowException("Expected any constant");
  }
#ifdef SEMANTIC
  if (curToken_.type == Token::Type::LITCONSTANT) {
    semantic_->PushAttribute(new TypeAttribute(L"char@", true, false));
  } else {
    if (curToken_.symbol == L"true" || curToken_.symbol == L"false") {
      semantic_->PushAttribute(new TypeAttribute(L"bool", true, false));
    } else {
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
    }
  }
#endif
  curToken_ = get();
}

void Parser::ParseAttribute() {
  if (curToken_.type != Token::Type::IDENTIFIER) {
    ThrowException("Expected identifier");
  }
#ifdef SEMANTIC
  semantic_->PopAttribute();
  ITIDEntry* entry = semantic_->GetCurrentTID()->FindByName(curToken_.symbol);
  if (!entry) {
    ThrowException("Can`t find attribute "  + std::string(curToken_.symbol.begin(), curToken_.symbol.end()));
  }
  VariableTIDEntry* var = dynamic_cast<VariableTIDEntry*>(entry);
  if (var) {
    semantic_->PushAttribute(var->type->Clone());
    curToken_ = get();
  } else {
    FunctionTIDEntry* func = dynamic_cast<FunctionTIDEntry*>(entry);
    if (func) {
      semantic_->PushAttribute(new FunctionAttribute(func->name));
      curToken_ = get();
    } else {
      NamespaceTIDEntry* nspace = dynamic_cast <NamespaceTIDEntry*>(entry);
      if (nspace) {
        curToken_ = get();
        if (curToken_.symbol != L"$") {
          ThrowException("Expected $");
        }
        curToken_ = get();
        semantic_->ChangeCurrentTID(nspace->GetCurrentTID());
        ParseNestedNamespace();
      } else {
        ThrowException("Unable to use struct as attribute");
      }
    }
  }
#endif
#ifndef SEMANTIC
  curToken_ = get();
#endif
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
      bool is_found = false;
      FunctionTIDEntry* found_func = nullptr;
      size_t min_argc = 0xdead, max_argc = 0xdead;
      semantic_->PopAttribute();
      for (auto& e : functions) {

        bool all_ok = true;
        FunctionTIDEntry* func = dynamic_cast<FunctionTIDEntry*>(e);
        if (min_argc == 0xdead) {
          min_argc = max_argc = func->type_arguments.size();
        }
        min_argc = std::min(min_argc, func->type_arguments.size());
        max_argc = std::max(max_argc, func->type_arguments.size());
        if (func->type_arguments.size() != args.size()) {
          all_ok = false;
          continue;
        }


        for (int i = 0; i < args.size(); ++i) {
          if (!DoTypesMatch(args[i], func->type_arguments[i].type)) {
            all_ok = false;
            break;
          }
        }

        if (all_ok) {
          if(found_func != nullptr) {
            ThrowException("Ambigious function call");
          }
          is_found = true;
          found_func = func;
        }
      }
      if (is_found) {
        if (found_func->is_proto) {
          semantic_->called_prototypes.push_back(found_func);
        }
        semantic_->PushAttribute(new TypeAttribute(found_func->return_type, false, false));
      }
      if (!is_found) {
        for (auto& e : functions) {
          FunctionTIDEntry* func = dynamic_cast<FunctionTIDEntry*>(e);
          if (func->type_arguments.size() != args.size()) {
            is_found = false;
            continue;
          }
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
              if (func->is_proto) {
                semantic_->called_prototypes.push_back(func);
              }
              is_found = true;
              semantic_->PushAttribute(
                  new TypeAttribute(func->return_type, false, false));
            }
          }
        }
      }
      
      if (!is_found) {  
        if (args.size() >= min_argc && args.size() <= max_argc) {
          ThrowException("Unable to find called function overload");
        } else {
          ThrowException(
              args.size() < min_argc
                  ? "Unable to find called function overload, too few arguments"
                  : "Unable to find called function overload, too many arguments");
        }
        
      }
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
      std::wstring fullsuffix = base_type.substr(i);
      suffix = base_type.substr(i);
      if (suffix.empty()) {
        ThrowException("Variable is not pointer or array type");
      }
      int j = 1;
      if (suffix[0] == L'@') {
        suffix = L"@";
      } else {
        while (base_type[i + j] != L']') {
          ++expected_count;
          ++j;
        }
      }
      base_type = base_type.substr(0, i);
      TID* struct_tid = nullptr;
      StructTIDEntry* str_entry =
          dynamic_cast<StructTIDEntry*>(semantic_->GetCurrentTID()->FindByName(base_type));
      if (str_entry) struct_tid = str_entry->GetCurrentTID();
      semantic_->PopAttribute();

      size_t actual_count = 1;
#endif
      ParseAssExpr();
#ifdef SEMANTIC
      TypeAttribute* ass_expr =
          dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
      CheckInt(ass_expr->type);
      semantic_->PopAttribute();
#endif

      while (curToken_.symbol == L",") {
#ifdef SEMANTIC
        ++actual_count;
        if (actual_count > expected_count) {
          ThrowException("Too much indexes");
        }
#endif
        curToken_ = get();
        ParseAssExpr();
#ifdef SEMANTIC
        TypeAttribute* ass_expr =
            dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
        CheckInt(ass_expr->type);
        semantic_->PopAttribute();
#endif
      }
      if (curToken_.symbol != L"]") {
        ThrowException("Expected closing rect bracket");
      }
      curToken_ = get();
#ifdef SEMANTIC
      if (struct_tid) {
        StructAttribute* str_attr = new StructAttribute(
            base_type + (j == fullsuffix.size() - 1
                             ? L""
                             : fullsuffix.substr(j)), false, true, struct_tid); 
        if (j != fullsuffix.size() - 1) str_attr->is_ptr = true;
        semantic_->PushAttribute(str_attr);
      } else {
        TypeAttribute* type_attr = new TypeAttribute(base_type + (j == fullsuffix.size() - 1 ? L"" : fullsuffix.substr(j)), false, true);
        semantic_->PushAttribute(type_attr);
      }
#endif
    }
    if (curToken_.symbol == L".") {
      shouldLeave = false;
#ifdef SEMANTIC
      StructAttribute* str_var =
          dynamic_cast<StructAttribute*>(semantic_->TopStackAttribute());

      if (str_var->is_ptr) {
        ThrowException("Unable to address attributes of pointer struct");
      }
      semantic_->ChangeCurrentTID(str_var->struct_tid_);
#endif
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
  DeclareJumpableConstruction();
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
  DeclareJumpableConstruction();
#endif
  if (curToken_.symbol != L")") {
    ThrowException("Expected closing brackets");
  }

  curToken_ = get();
  ParseBody();
  #ifdef SEMANTIC
  semantic_->PopAttribute();
  #endif
  if (curToken_.symbol == L"else") {
    curToken_ = get();
    ParseBody();
  }
}

void Parser::ParseFor() {
  // we already have for
#ifdef SEMANTIC
  semantic_->CreateNewTID();
  DeclareJumpableConstruction();
#endif
#ifdef GENERATOR
  generator_->PushItemToProgram(new ScopeBeginItem());
  size_t beg = 0, end = 0;
  size_t P_end, P_if, P_step, P_inc, P_else;
  
  generator_->incompleteLoopJumps.push_back(LoopJumpable());
#endif // GENERATOR


  curToken_ = get();
  if (curToken_.symbol != L"(") {
    ThrowException("Expected opening bracket");
  }

  //Bruh... moment, but we need it
  curToken_ = get();
  if (IsType(curToken_) || curToken_.symbol == L"const") {
    ParseVarDefs();
  } else {
    if (curToken_.symbol != L";") {
      ParseVarDefs();
    }
  }

#ifdef GENERATOR
  generator_->CompleteRPN();
  P_if = generator_->GetCurrentCursor() + 1;
#endif // GENERATOR


  if (curToken_.symbol != L";") {
    ThrowException("Expected ;");
  }



  curToken_ = get();
  if (curToken_.symbol != L";") {
#ifdef GENERATOR
      //generator_->PushItemToProgram(new LabelItem());
      //P_if = generator_->GetCurrentCursor();
#endif // GENERATOR
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
#ifdef GENERATOR
    generator_->CompleteRPN();

    generator_->PushItemToProgram(new JumpElseItem());
    P_else = generator_->GetCurrentCursor();

    generator_->PushItemToProgram(new JumpItem());
    P_step = generator_->GetCurrentCursor();
#endif // GENERATOR
  // bruh moment, but we need it
  if (curToken_.symbol != L";") {
    ThrowException("Expected ;");
  }

  curToken_ = get();
#ifdef GENERATOR
  P_inc = generator_->GetCurrentCursor() + 1;
#endif // GENERATOR
  if (curToken_.symbol != L")") {
    ParseExpr();
#ifdef SEMANTIC
    semantic_->PopAttribute();
#endif

  }
#ifdef GENERATOR
    generator_->CompleteRPN();
    generator_->PushItemToProgram(new JumpItem(P_if));
    dynamic_cast<JumpItem*>(generator_->program[P_step])->jump_ind_ = 
        generator_->GetCurrentCursor() + 1;
#endif // GENERATOR

  if (curToken_.symbol != L")") {
    ThrowException("Expected closing bracket");
  }
  curToken_ = get();

  ParseBody();
#ifdef SEMANTIC
  semantic_->PopAttribute();
  semantic_->RemoveCurrentTID();
#endif

#ifdef GENERATOR
  generator_->PushItemToProgram(new JumpItem(P_inc));
  dynamic_cast<JumpElseItem*>(generator_->program[P_else])->jump_ind_ = generator_->GetCurrentCursor() + 1;
  P_else = generator_->GetCurrentCursor();
#endif // GENERATOR
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
#ifdef GENERATOR
  P_end = generator_->GetCurrentCursor() + 1;
  generator_->incompleteLoopJumps[generator_->incompleteLoopJumps.size() - 1].end_ind_ = P_end;
  generator_->incompleteLoopJumps[generator_->incompleteLoopJumps.size() - 1].inc_ind_ = P_inc;
  std::wcout << " pend" << P_end << std::endl;
  //dynamic_cast<JumpElseItem*>(generator_->program[P_end])->jump_ind_ = generator_->GetCurrentCursor() + 1;
    for (auto jmp : generator_->incompleteLoopJumps[generator_->incompleteLoopJumps.size() - 1].loop_jumps) {
        if (jmp.second == LoopJumpable::Type::BREAK) {
            dynamic_cast<JumpItem*>(generator_->program[jmp.first])->jump_ind_ =
                generator_->incompleteLoopJumps[generator_->incompleteLoopJumps.size() - 1].end_ind_;
        } else {
            //Continue branch
            dynamic_cast<JumpItem*>(generator_->program[jmp.first])->jump_ind_ =
                generator_->incompleteLoopJumps[generator_->incompleteLoopJumps.size() - 1].inc_ind_; 
        }
    }
  generator_->incompleteLoopJumps.pop_back();

#endif // GENERATOR

}

void Parser::ParseDowhile() {
  // we already have do
  curToken_ = get();
#ifdef SEMANTIC
  semantic_->CreateNewTID();
  DeclareJumpableConstruction();
#endif
  ParseBody();
#ifdef SEMANTIC
  semantic_->RemoveCurrentTID();
  semantic_->PopAttribute();
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
  #ifdef GENERATOR
    generator_->PushItemToProgram(new JumpItem());
    generator_->incompleteLoopJumps[generator_->incompleteLoopJumps.size() - 1]
        .loop_jumps.push_back(std::pair<int, LoopJumpable::Type>(generator_->GetCurrentCursor(), LoopJumpable::Type::BREAK));
  #endif // GENERATOR

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
#ifdef GENERATOR
    generator_->PushItemToProgram(new JumpItem());
    generator_->incompleteLoopJumps[generator_->incompleteJumps.size() - 1]
        .loop_jumps.push_back(std::pair<int, LoopJumpable::Type>(generator_->GetCurrentCursor(), LoopJumpable::Type::CONTINUE));
#endif // GENERATOR
    curToken_ = get();
    return;
  }
  if (curToken_.symbol == L"goto") {
    curToken_ = get();
    #ifdef SEMANTIC
      semantic_->AddGotoCall(curToken_.symbol);
    #endif
    if (curToken_.type != Token::Type::IDENTIFIER) {
      ThrowException("Expected identifier");
    }
    curToken_ = get();
#ifdef GENERATOR
    generator_->PushItemToProgram(new JumpItem());
    generator_->incompleteJumps.push_back(std::pair<size_t, std::wstring>(generator_->GetCurrentCursor(), curToken_.symbol));
    generator_->incompleteLoopJumps[generator_->incompleteJumps.size() - 1]
        .loop_jumps.push_back(std::pair<size_t, LoopJumpable::Type>(generator_->GetCurrentCursor(), LoopJumpable::Type::BREAK));
#endif // GENERATOR
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
  TypeAttribute* expected =
      dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
  if (!expected) {
    JumpAttribute* jmpattr = dynamic_cast<JumpAttribute*>(semantic_->TopStackAttribute());
    if (!jmpattr) ThrowException("Unable to return from nothing");
    expected = jmpattr->return_type_ptr;
  }
  #endif
  if (curToken_.symbol != L";") {
#ifdef SEMANTIC
    if (expected->type == L"void") {
      ThrowException("Function doesn`t have return type");
    }
#endif
    ParseExpr();
#ifdef SEMANTIC
    TypeAttribute* type =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    if (!CanCast(expected->type, type->type)) {
      ThrowException("Unable to cast to return type");
    }
    semantic_->PopAttribute();
#endif
  } else {
#ifdef SEMANTIC
    if (expected->type != L"void") {
      ThrowException("Function must return value");
    }
#endif
  }

}

void Parser::ParseTypeInstance() {
  if (curToken_.type != Token::Type::IDENTIFIER) {
    ThrowException("Expected identifier");
  }
#ifdef SEMANTIC
  StructTIDEntry* str_entry =
      dynamic_cast<StructTIDEntry*>(semantic_->GetCurrentTID()->FindByName(curToken_.symbol));
  TypeAttribute* type;
  if (str_entry) {
    type = new StructAttribute(curToken_.symbol, false, false, str_entry->GetCurrentTID());
  }
  type = new TypeAttribute(curToken_.symbol, false, false);
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
#ifdef SEMANTIC
    TypeAttribute* expr =
        dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
    CheckInt(expr->type);
    semantic_->PopAttribute();
#endif
    while (curToken_.symbol == L",") {
#ifdef SEMANTIC
      type->type.append(curToken_.symbol);
#endif
      curToken_ = get();
      ParseAssExpr();
#ifdef SEMANTIC
      expr = dynamic_cast<TypeAttribute*>(semantic_->TopStackAttribute());
      CheckInt(expr->type);
      semantic_->PopAttribute();
#endif
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
  #ifdef SEMANTIC
  semantic_->AddGotoLabel(curToken_.symbol);
  #endif
  if (curToken_.type != Token::Type::IDENTIFIER) {
    ThrowException("Expected identifier");
  }
#ifdef GENERATOR
  generator_->PushItemToProgram(new LabelItem(curToken_.symbol));
  generator_->labels[curToken_.symbol] = generator_->GetCurrentCursor();
#endif // GENERATOR

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
