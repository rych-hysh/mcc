#pragma once

//Token(単語)の種別
enum TokenType : int
{
  TK_SYMBOL, //symbol
  TK_NUMBER, // integer
  TK_IDENTIFIER, //idintifier
  TK_RETURN, // token of "return"
  TK_EOF,    //end of file
};
//Token は、単語単位
struct Token
{
  TokenType type; //token _type
  Token *next;    //next token
  int value;      //value when _type = TK_INT
  char *str;      //token string
  int length;     //length of token
};

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
  ND_ASSIGN, // =
  ND_LVAL,   // ローカル変数
  ND_RETURN, // return
  ND_NUMBER, // 整数
};

//抽象構文木のノードの型
struct Node
{
  NodeType type;
  Node *leftHandSideNode;
  Node *rightHandSideNode;
  int value;  //typeがND_NUMBERの時その値
  int offset; //typeがND_LVALの場合のベースポインタからのオフセット
};

struct LocalVariable {
  LocalVariable *next;  // 次の変数かNULL
  char *name;           // 変数の名前
  int length;           // 変数名の長さ
  int offset;           // RBPからのオフセット
};