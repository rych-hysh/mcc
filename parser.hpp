#pragma once
#include "tokenizer.hpp"
#include "types.hpp"

class Parser
{
public:
  Parser(char *_raw_input);

  Node **parse(Token *_head_token);

  Node *new_node(NodeType _type, Node *_lhs, Node *_rhs);

  Node *new_node_num(int _value);

private:
  char *raw_input;
  Token *token_proccessing;
  Node *statement[100];
  LocalVariable *locals;

  LocalVariable *find_local_var(Token *_token);
  // 次のtokenが期待している記号の時にはトークンを１つ読み進めて真を返す。それ以外の場合は偽を返す。
  Token *consume(const char *_op);
  Token *consume_reserved(const char *_reserved);
  Token *consume_identifier();

  // 次のtokenが期待している記号の時にはトークンを１つ読み進めて真を返す。それ以外の場合はエラーを報告する。
  void expect(const char *_op);

  int expect_number();

  bool at_eof();

  Node **program();

  Node *stmt();

  Node *expr();

  Node *assign();

  Node *equality();

  Node *relational();

  Node *add();

  Node *mul();

  Node *unary();

  Node *primary();
};
