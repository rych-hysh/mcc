#include <cstdlib>
#include <vector>

#include "tokenizer.hpp"
#include "parser.hpp"
#include "util.hpp"
#include <vector>

using std::vector;

using std::vector;

Parser::Parser(char *_r)
{
  raw_input = _r;
};

vector<Function*> Parser::parse(Token *_head_token)
{
  token_proccessing = _head_token;
  return program();
}

Node *Parser::new_node(NodeType _type, Node *_lhs, Node *_rhs)
{
  Node *new_node = (Node *)calloc(1, sizeof(Node));
  new_node->type = _type;
  new_node->leftHandSideNode = _lhs;
  new_node->rightHandSideNode = _rhs;
  return new_node;
}

Node *Parser::new_node_num(int _value)
{
  Node *new_node = (Node *)calloc(1, sizeof(Node));
  new_node->type = NodeType::ND_NUMBER;
  new_node->value = _value;
  return new_node;
}

Node *Parser::new_node_if(Node *_cond, Node *_then, Node *_else){
  Node *new_node = (Node *) calloc(1, sizeof(Node));
  new_node->type = NodeType::ND_IF;
  new_node->condNode = _cond;
  new_node->thenNode = _then;
  new_node->elseNode = _else;
  return new_node;
}

Node *Parser::new_node_while(Node *_cond, Node *_then){
  Node *new_node = (Node *) calloc(1, sizeof(Node));
  new_node->type = NodeType::ND_WHILE;
  new_node->condNode = _cond;
  new_node->thenNode = _then;
  return new_node;
}

Node *Parser::new_node_for(Node *_init, Node *_cond, Node *_loop, Node *_then){
  Node *new_node = (Node *) calloc(1, sizeof(Node));
  new_node->type = NodeType::ND_FOR;
  new_node->initNode = _init;
  new_node->condNode = _cond;
  new_node->loopNode = _loop;
  new_node->thenNode = _then;
  return new_node;
}

vector<Function *> Parser::program()
{
  while (!at_eof())
  {
    Function* new_func = (Function *)calloc(1, sizeof(Function));
    functions.push_back(new_func);
    functions[funcs_index]->local_var = NULL;
    functions[funcs_index]->Func_top_node = func();
    funcs_index++;
  }
  return functions;
}

Node *Parser::func(){
  Node *func_node = (Node *)calloc(1, sizeof(Node));
  expect_reserved("int");
  Token *func_tok = expect_identifier();
  func_node->identifier = (char*)calloc(func_tok->length, sizeof(char*));
  strncpy(func_node->identifier, func_tok->str, func_tok->length);
  expect("(");
  func_node->type = NodeType::ND_FUNC;
  LocalVariable *head_var = NULL;
  bool first = true;
  //Function型にlocalvariableをくっつける
  while(consume("int", TK_RESERVED)){
    // 関数定義時の引数から関数内で使用できるローカル変数の定義
    Node *node = (Node *)calloc(1, sizeof(Node));
    Token *consumed = expect_identifier();
    node->type=ND_LVAL;
    if(first){
      first = false;
      head_var = (LocalVariable *)calloc(1, sizeof(LocalVariable));
      head_var->offset = 0;
      functions[funcs_index]->local_var = head_var;
    }
    LocalVariable *arg_variable = (LocalVariable *)calloc(1, sizeof(LocalVariable));
    arg_variable->length = consumed->length;
    arg_variable->name = consumed->str;
    arg_variable->offset = functions[funcs_index]->local_var->offset + 8;
    functions[funcs_index]->local_var->next = arg_variable;
    functions[funcs_index]->local_var = functions[funcs_index]->local_var->next;

    if(is_proccessing(")", TokenType::TK_SYMBOL))break;
    expect(",");
  }
  if(head_var)functions[funcs_index]->local_var = head_var->next;
  expect(")");
  expect("{");
  Node *res_node = func_node;
  while(!consume("}")){   
    func_node->next = stmt();
    func_node = func_node->next;
  }
  return res_node;
}

Node *Parser::stmt()
{
  //if(consume(";", TokenType::TK_SYMBOL))return new_node_num(1);
  Node *node = (Node *)calloc(1, sizeof(Node));
  // if(is_identifier(token_proccessing)){
  //   if(is_token(token_proccessing->next, TK_SYMBOL, "(")){

  //   }
  // } 
  if (consume("return", TokenType::TK_RESERVED))
  {
    node = new_node(NodeType::ND_RETURN, expr(), NULL);
  } else if(consume("if", TokenType::TK_RESERVED)){
    expect("(");
    //node->type = NodeType::ND_IF;
    //node->condNode = expr();
    //node = new_node(ND_IF, expr(), NULL);
    node = new_node_if(expr(), NULL, NULL);
    expect(")");
    node->thenNode = stmt();
    if(consume("else", TokenType::TK_RESERVED)){
      node->elseNode = stmt();
    }
    return node;
  } else if(consume("while", TokenType::TK_RESERVED)){
    expect("(");
    node = new_node_while(expr(), NULL);
    expect(")");
    node->thenNode = stmt();
    return node;
  } else if(consume("for", TokenType::TK_RESERVED)){
    expect("(");
    node = new_node_for(NULL, NULL, NULL, NULL);
    if(!is_proccessing(";", TokenType::TK_SYMBOL)){
      node->initNode = expr();
    };
    expect(";");
    if(!is_proccessing(";", TokenType::TK_SYMBOL)){
      node->condNode = expr();
    }
    expect(";");
    if(!is_proccessing(")", TokenType::TK_SYMBOL)){
      node->loopNode = expr();
    }
    expect(")");
    node->thenNode = stmt();
    return node;
  } else if(consume("{")){
      node->type = NodeType::ND_BLOCK;
      Node *head = node;
    while(!consume("}")){
      node->thenNode = stmt();
      node = node->thenNode;
    }
    return head;
  } else {
    node = expr();
  }
  expect(";");
  return node;
}

Node *Parser::expr()
{
  if(consume("int", TK_RESERVED)){
      Node *node = (Node *)calloc(1, sizeof(Node));
      //　関数内のローカル変数定義
      Token* identifier = expect_identifier();
      LocalVariable* lvar = (LocalVariable *)calloc(1, sizeof(LocalVariable));
      lvar->next = functions[funcs_index]->local_var;
      lvar->name = identifier->str;
      lvar->length = identifier->length;
      lvar->offset = functions[funcs_index]->local_var ? functions[funcs_index]->local_var->offset + 8 : 8;
      functions[funcs_index]->local_var = lvar;
      node->type = NodeType::ND_LVARDEF;
      node->offset = lvar->offset;
      return node;
  } 
  return assign();
}

Node *Parser::assign()
{
  Node *node = equality();
  if (consume("="))
  {
    node = new_node(ND_ASSIGN, node, assign());
  }
  return node;
}

Node *Parser::equality()
{
  Node *node = relational();
  if (consume("=="))
  {
    node = new_node(NodeType::ND_EQ, node, relational());
  }
  else if (consume("!="))
  {
    node = new_node(NodeType::ND_NE, node, relational());
  }
  return node;
}

Node *Parser::relational()
{
  Node *node = add();
  if (consume("<="))
  {
    node = new_node(NodeType::ND_LEQ, node, add());
  }
  else if (consume(">="))
  {
    node = new_node(NodeType::ND_GEQ, node, add());
  }
  else if (consume("<"))
  {
    node = new_node(NodeType::ND_LT, node, add());
  }
  else if (consume(">"))
  {
    node = new_node(NodeType::ND_GT, node, add());
  }
  return node;
}

Node *Parser::add()
{
  Node *node = mul();
  while (true)
  {
    if (consume("+"))
    {
      node = new_node(NodeType::ND_ADD, node, mul());
    }
    else if (consume("-"))
    {
      node = new_node(NodeType::ND_SUB, node, mul());
    }
    else
    {
      return node;
    }
  }
}

Node *Parser::mul()
{
  Node *node = unary();
  while (true)
  {
    if (consume("*"))
    {
      node = new_node(NodeType::ND_MUL, node, unary());
    }
    else if (consume("/"))
    {
      node = new_node(NodeType::ND_DIV, node, unary());
    }
    else
    {
      return node;
    }
  }
}

Node *Parser::unary()
{
  if (consume("+"))
  {
    return primary();
  }
  if (consume("-"))
  {
    return new_node(NodeType::ND_SUB, new_node_num(0), primary());
  }
  if(consume("&")){
    return new_node(NodeType::ND_ADDR, unary(), NULL);
  }
  if(consume("*")){
    return new_node(NodeType::ND_DEREF, unary(), NULL);
  }
  return primary();
}

Node *Parser::primary()
{
  if (consume("("))
  {
    Node *node = expr();
    expect(")");
    return node;
  }
  Token *identifier = consume_identifier();
  if (identifier)
  {
    if(consume("(")){
      Node *call_node = (Node *)calloc(1, sizeof(Node));
      call_node->type = NodeType::ND_FUNCCALL;
      call_node->identifier = (char *)malloc(identifier->length * sizeof(char));
      strncpy(call_node->identifier, identifier->str, identifier->length);
      Arg *head_arg;
      bool first=true;
      while(!is_proccessing(")", TokenType::TK_SYMBOL)){
        if(first){
          first=false;
          call_node->arg = (Arg *)calloc(1, sizeof(Arg));
          head_arg=call_node->arg;
        }
        Arg *new_arg = (Arg *)calloc(1, sizeof(Arg));
        new_arg->node = expr();
        call_node->arg->next = new_arg;
        call_node->arg = call_node->arg->next;
        if(!consume(","))break;
      }
      expect(")");
      if(call_node->arg)call_node->arg=head_arg->next;
      return call_node;
    }
    Node *node = (Node *)calloc(1, sizeof(Node));
    node->type = ND_LVAL;

    LocalVariable *lvar = find_local_var(identifier);
    if (lvar)
    {
      // すでに使われた変数ならそのオフセットを取得することでアドレスを取得
      node->offset = lvar->offset;
    }
    else
    {
      error_at(raw_input, token_proccessing->str, "undefined var.");
    }
    return node;
  }
  // そうでなければ数値のはず
  return new_node_num(expect_number());
}

LocalVariable *Parser::find_local_var(Token *_token)
{
  for (LocalVariable *var = functions[funcs_index]->local_var; var; var = var->next)
  {
    if (var->length == _token->length && !memcmp(_token->str, var->name, var->length))
    {
      return var;
    }
  }
  return NULL;
}

// TODO: たぶんこれでconsumeとconsume_reservedをまとめられるのでconsume()とconsume_resserved()削除
Token *Parser::consume(const char *_str, TokenType _TK_TYPE){
  if (token_proccessing->type != _TK_TYPE || strlen(_str) != token_proccessing->length || memcmp(token_proccessing->str, _str, token_proccessing->length))
    return NULL;
  Token *consumed = token_proccessing;
  token_proccessing = token_proccessing->next;
  return consumed;
}

Token *Parser::consume_identifier()
{
  if (token_proccessing->type != TokenType::TK_IDENTIFIER)
  {
    return NULL;
  }
  Token *consumed = token_proccessing;
  token_proccessing = token_proccessing->next;
  return consumed;
}

// 次のtokenが期待している記号の時にはトークンを１つ読み進めて真を返す。それ以外の場合はエラーを報告する。
void Parser::expect(const char *_op)
{
  if (token_proccessing->type != TokenType::TK_SYMBOL || strlen(_op) != token_proccessing->length || memcmp(token_proccessing->str, _op, token_proccessing->length))
  {
    error_at(raw_input, token_proccessing->str, "'%s'ではありません", _op);
  }
  token_proccessing = token_proccessing->next;
}

int Parser::expect_number()
{
  if (token_proccessing->type != TokenType::TK_NUMBER)
  {
    error_at(raw_input, token_proccessing->str, "数ではありません");
  }
  int value = token_proccessing->value;
  token_proccessing = token_proccessing->next;
  return value;
}

Token *Parser::expect_identifier(){
  if(token_proccessing->type != TK_IDENTIFIER)error_at(raw_input, token_proccessing->str, "identifierではありません。");
  char *identifier = (char *)malloc(token_proccessing->length * sizeof(char));
  Token* consumed = token_proccessing;
  strncpy(identifier, token_proccessing->str, token_proccessing->length);
  token_proccessing = token_proccessing->next;
  return consumed;
}

void Parser::expect_reserved(const char* expected){
  if (token_proccessing->type != TokenType::TK_RESERVED || strlen(expected) != token_proccessing->length || memcmp(token_proccessing->str, expected, token_proccessing->length))
  {
    error_at(raw_input, token_proccessing->str, "'%s'ではありません", expected);
  }
  token_proccessing = token_proccessing->next;
}

bool Parser::is_proccessing(const char* _expected, TokenType _TK_TYPE){
  if(token_proccessing->type != _TK_TYPE || memcmp(token_proccessing->str, _expected, token_proccessing->length) ){
    return false;
  }
  return true;
}

bool Parser::is_identifier(Token *_token){
  if(_token->type != TK_IDENTIFIER)return false;
  return true;
}
bool Parser::at_eof()
{
  return token_proccessing->type == TokenType::TK_EOF;
}
