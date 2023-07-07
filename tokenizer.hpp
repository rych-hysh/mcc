#pragma once

#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <string>

#include "types.hpp"

class Tokenizer
{
  const std::string RESERVED_WORDS[3] = {"return", "if", "else"};
  public:
  void error(std::string _message, ...);
  // TODO: STEP4　改良
  void error(const char *_fmt, ...);

  //新しいトークンを作成してcurrent tokenに繋げる
  Token *new_token(TokenType _type, Token *_current, char *_str, int _length);

  bool startswith(char *_p, const char *_q);

  bool is_token_char(char c);

  ///入力文字列pをトークナイズして返す
  Token *tokenize(char *_p);
};
