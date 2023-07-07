#include <cstdlib>

#include "tokenizer.hpp"
#include "parser.hpp"
#include "util.hpp"

Parser::Parser(char *_r)
{
  locals = (LocalVariable *)calloc(1, sizeof(LocalVariable));
  locals->offset = 0;
  raw_input = _r;
};

Node **Parser::parse(Token *_head_token)
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

Node **Parser::program()
{
  while (!at_eof())
  {
    statement[statement_index++] = stmt();
  }
  statement[statement_index] = NULL;
  return statement;
}

Node *Parser::stmt()
{
  //if(consume(";", TokenType::TK_SYMBOL))return new_node_num(1);
  Node *node = (Node *)calloc(1, sizeof(Node));
  if (consume("return", TokenType::TK_RESERVED))
  {
    node = new_node(NodeType::ND_RETURN, expr(), NULL);
  } else if(consume("if", TokenType::TK_RESERVED)){
    expect("(");
    node = new_node(NodeType::ND_IF, expr(), NULL);
    expect(")");
    node->thenNode = stmt();
    return node;
  } else {
    node = expr();
  }
  expect(";");
  return node;
}

Node *Parser::expr()
{
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
      lvar = (LocalVariable *)calloc(1, sizeof(LocalVariable));
      lvar->next = locals;
      lvar->name = identifier->str;
      lvar->length = identifier->length;
      lvar->offset = locals->offset + 8;
      node->offset = lvar->offset;
      locals = lvar;
    }
    return node;
  }
  // そうでなければ数値のはず
  return new_node_num(expect_number());
}

LocalVariable *Parser::find_local_var(Token *_token)
{
  for (LocalVariable *var = locals; var; var = var->next)
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

// maybe _op means operando
// 次のtokenが期待している記号の時にはトークンを１つ読み進めて真を返す。それ以外の場合は偽を返す。
Token *Parser::consume(const char *_op)
{
  if (token_proccessing->type != TokenType::TK_SYMBOL || strlen(_op) != token_proccessing->length || memcmp(token_proccessing->str, _op, token_proccessing->length))
    return NULL;
  Token *consumed = token_proccessing;
  token_proccessing = token_proccessing->next;
  return consumed;
}

Token *Parser::consume_reserved(const char *_reserved)
{
  if (token_proccessing->type != TK_RESERVED || memcmp(token_proccessing->str, _reserved, token_proccessing->length))
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

bool Parser::at_eof()
{
  return token_proccessing->type == TokenType::TK_EOF;
}