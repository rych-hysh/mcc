#pragma once

#include <string>
#include <list>

//Token(単語)の種別
enum TokenType : int
{
  TK_SYMBOL, //symbol
  TK_NUMBER, // integer
  TK_IDENTIFIER, //idintifier
  TK_RETURN, // token of "return"
  TK_IF,      // token for "if"
  // TODO: replave TK_RETURN and TK_IF with TK_RESERVED
  TK_RESERVED,  // token of reserve word, such as "return", "if"
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
  ND_ADDR,   // &
  ND_DEREF,  // *
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
  ND_IF,
  ND_WHILE,
  ND_FOR,
  ND_BLOCK,
  ND_FUNC,
  ND_FUNCCALL,
  ND_NUMBER, // 整数
  ND_LVARDEF, // ローカル変数定義
};

struct LocalVariable {
  LocalVariable *next;  // 次の変数かNULL
  char *name;           // 変数の名前
  int length;           // 変数名の長さ
  int offset;           // RBPからのオフセット
};

struct Node;

struct Arg{
  Node *node;
  Arg *next;
};

//抽象構文木のノードの型
struct Node
{
  NodeType type;
  Node *leftHandSideNode;
  Node *rightHandSideNode;

  Node *next;

  Node *condNode;
  Node *thenNode;
  Node *elseNode;
  Node *initNode;
  Node *loopNode;

  Arg *arg;

  LocalVariable *local_vars;
  int value;  //typeがND_NUMBERの時その値
  int offset; //typeがND_LVALの場合のベースポインタからのオフセット
  char *identifier;
};

struct Function{
  Node *Func_top_node;
  LocalVariable *local_var;
};
