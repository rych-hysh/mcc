#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <string>
#include <bits/stdc++.h>

#include "tokenizer.hpp"

using namespace std;

void Tokenizer::error(string _message, ...)
{
  va_list ap;
  va_start(ap, _message);
  _message.append("\n");
  fprintf(stderr, _message.c_str(), ap);
  exit(1);
}
// TODO: STEP4　改良
void Tokenizer::error(const char *_fmt, ...)
{
  va_list ap;
  va_start(ap, _fmt);

  vfprintf(stderr, _fmt, ap);

  fprintf(stderr, "\n");
  exit(1);
}

//新しいトークンを作成してcurrent tokenに繋げる
Token *Tokenizer::new_token(TokenType _type, Token *_current, char *_str, int _length)
{
  Token *new_token = (Token *)calloc(1, sizeof(Token));
  new_token->type = _type;
  new_token->length = _length;
  if (_str != "")
    new_token->str = _str;
  _current->next = new_token;
  return new_token;
}

bool Tokenizer::startswith(char *_p, const char *_q){
  return memcmp(_p, _q, strlen(_q)) == 0;
}

bool Tokenizer::is_token_char(char _c){
  return ('a' <= _c && _c <= 'z') ||
         ('A' <= _c && _c <= 'Z') ||
         ('0' <= _c && _c <= '9') ||
         (_c == '_');
}

///入力文字列pをトークナイズして返す
Token *Tokenizer::tokenize(char *_p)
{
  Token head;
  head.next = NULL;
  Token *current = &head;

  while (*_p)
  {
    //空白文字をスキップ
    if (isspace(*_p))
    {
      _p++;
      continue;
    }

    if(startswith(_p, "==") || startswith(_p, "!=") || startswith(_p, "<=") || startswith(_p, ">=") ){
      current = new_token(TokenType::TK_SYMBOL, current, _p, 2);
      _p+=2;
      continue;
    }

    //入力文字列が+か-ならTK_SYMBOL typeのトークンを追加して次の文字へ
    if (strchr("+-*/()=<>;", *_p))
    {
      //引数に_p++を渡すことでpを渡しつつpを一つ進めてる。テクい。++pとの違いが出てる。
      current = new_token(TokenType::TK_SYMBOL, current, _p++, 1);
      continue;
    }

    if (strncmp(_p, "return", 6) == 0 && !is_token_char(_p[6])) {
      current = new_token(TokenType::TK_RESERVED, current, _p, 6);
      _p += 6;
      continue;
    }
    if (strncmp(_p, "if", 2) == 0 && !is_token_char(_p[2])) {
      current = new_token(TokenType::TK_RESERVED, current, _p, 2);
      _p += 2;
      continue;
    }

    if (isdigit(*_p))
    {
      current = new_token(TokenType::TK_NUMBER, current, _p, 0);
      //strtolは数値を読み込んだ後、第二引数のポインタをアップデートし読み込んだ最後の文字の次の文字を指すように値を更新する
      char *q = _p;
      current->value = strtol(_p, &_p, 10);
      current->length = _p - q;
      continue;
    }

    if(is_token_char(*_p)){
      char *token_char = _p;
      int len = 1;
      while(is_token_char(*++_p)){
        len++;
      }
      bool reserved = false;
      for(string w : RESERVED_WORDS){
        if(!memcmp(w.c_str(), token_char, len)){
          current = new_token(TokenType::TK_RESERVED, current, token_char, len);
          reserved = true;
          break;
        }
      }
      if(reserved)continue;
      current = new_token(TokenType::TK_IDENTIFIER, current, token_char, len);
      continue;
    }

    error("tokenizeできません");
  }
  current = new_token(TokenType::TK_EOF, current, _p, 0);
  return head.next;
}
