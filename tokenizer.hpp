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

  //新しいトークンを作成してcurrent tokenに繋げる
  Token *new_token(TokenType _type, Token *_current, char *_str, int _length);

  bool startswith(char *_p, const char *_q);

  bool is_token_char(char c);

  bool is_reserved(char **_p, Token **_current);
  public:

  ///入力文字列pをトークナイズして返す
  Token *tokenize(char *_p);
};
