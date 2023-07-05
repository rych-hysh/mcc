#pragma once
#include "tokenizer.hpp"

enum NodeType
{
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_EQ,     // ==
  ND_NE,     // !=
  ND_LT,     // <
  ND_LEQ,    // <=
  ND_GT,     // >
  ND_GEQ,    // >=
  ND_NUMBER, // 整数
};

//抽象構文木のノードの型
struct Node
{
  NodeType type;
  Node *leftHandSideNode;
  Node *rightHandSideNode;
  int value;
};

class Parser
{
public:
  Parser();

  Node *parse(Token *_head_token);

  Node *new_node(NodeType _type, Node *_lhs, Node *_rhs);

  Node *new_node_num(int _value);

  Node *expr();

  Node *equality();

  Node *relational();

  Node *add();

  Node *mul();

  Node *unary();

  Node *primary();
  private:
  Tokenizer *tokenizer;
  Token *token_proccessing;

    // maybe _op means operando
  // 次のtokenが期待している記号の時にはトークンを１つ読み進めて真を返す。それ以外の場合は偽を返す。
  bool consume(const char *_op);

  // 次のtokenが期待している記号の時にはトークンを１つ読み進めて真を返す。それ以外の場合はエラーを報告する。
  void expect(const char *_op);

  int expect_number();

  bool at_eof();
};
