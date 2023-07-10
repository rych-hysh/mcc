#pragma once
#include "tokenizer.hpp"
#include "types.hpp"

class Parser
{
public:
  Parser(char *_raw_input);

  Node **parse(Token *_head_token);



private:
  char *raw_input;
  Token *token_proccessing;
  Node *statement[100];
  int statement_index = 0;
  LocalVariable *locals;

  //指定したNodeを生成
  Node *new_node(NodeType _type, Node *_lhs, Node *_rhs);
  Node *new_node_num(int _value);
  Node *new_node_if(Node* _cond, Node* _then ,Node* _else);
  Node *new_node_while(Node *_cond, Node *_then);
  Node *new_node_for(Node *_init, Node *_cond, Node *_loop, Node *_then);
  
  LocalVariable *find_local_var(Token *_token);

  //token_proccessingが指定した文字列とTokenTypeならそのトークンを返し、token_proccessingを次に進める
  Token *consume(const char *_str, TokenType _TK_TYPE = TokenType::TK_SYMBOL);
  Token *consume_identifier();

  // 次のtokenが期待している記号の時はtoken_proccessingを次に進める。それ以外の場合はエラーを報告する。
  void expect(const char *_op);
  // 次のtokenが数値の時はその値を返しtoken_proccessingを次に進める。それ以外の場合はエラーを報告する。
  int expect_number();

  //処理しているトークンの次のトークンを先読みし、期待するTokenならtrue,　それ以外ならfalseを返す。token_proccessingは変更しない。
  bool is_proccessing(const char *_expected, TokenType _expected_type);

  bool is_identifier(Token *_token);
  bool is_token(Token *_token, TokenType _expected_type, const char *_expected);
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
