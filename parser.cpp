#include <cstdlib>

#include "tokenizer.hpp"
#include "parser.hpp"

Parser::Parser(){
  tokenizer = new Tokenizer();
};

Node *Parser::parse(Token *_head_token){
  token_proccessing = _head_token;
  return expr();
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

Node *Parser::expr()
{
  return equality();
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
  else
  {
    return node;
  }
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
  else
  {
    return node;
  }
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
  //そうでなければ数値のはず
  return new_node_num(expect_number());
}

// maybe _op means operando
// 次のtokenが期待している記号の時にはトークンを１つ読み進めて真を返す。それ以外の場合は偽を返す。
bool Parser::consume(const char *_op)
{
  if (token_proccessing->type != TokenType::TK_SYMBOL || strlen(_op) != token_proccessing->length || memcmp(token_proccessing->str, _op, token_proccessing->length))
    return false;
  token_proccessing = token_proccessing->next;
  return true;
}

// 次のtokenが期待している記号の時にはトークンを１つ読み進めて真を返す。それ以外の場合はエラーを報告する。
void Parser::expect(const char *_op)
{
  if (token_proccessing->type != TokenType::TK_SYMBOL || strlen(_op) != token_proccessing->length || memcmp(token_proccessing->str, _op, token_proccessing->length))
  {
    fprintf(stderr, u8"'%s'ではありません", _op);
  }
  token_proccessing = token_proccessing->next;
}

int Parser::expect_number()
{
  if (token_proccessing->type != TokenType::TK_NUMBER)
  {
    fprintf(stderr, u8"数ではありません");
  }
  int value = token_proccessing->value;
  token_proccessing = token_proccessing->next;
  return value;
}

bool Parser::at_eof()
{
  return token_proccessing->type == TokenType::TK_EOF;
}