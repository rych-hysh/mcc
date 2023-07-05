#pragma once

#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <string>

//Token(単語)の種別
enum TokenType : int
{
  TK_SYMBOL, //symbol
  TK_NUMBER, // integer
  TK_IDENTIFIER, //idintifier
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

class Tokenizer
{
  public:
  void error(std::string _message, ...);
  // TODO: STEP4　改良
  void error(const char *_fmt, ...);

  //新しいトークンを作成してcurrent tokenに繋げる
  Token *new_token(TokenType _type, Token *_current, char *_str, int _length);

  bool startswith(char *_p, const char *_q);

  ///入力文字列pをトークナイズして返す
  Token *tokenize(char *_p);
};
