#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <string>

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

///入力文字列pをトークナイズして返す
Token *Tokenizer::tokenize(char *_p)
{
  Token head;
  head.next = NULL;
  Token *_current = &head;

  while (*_p)
  {
    //空白文字をスキップ
    if (isspace(*_p))
    {
      _p++;
      continue;
    }

    if(startswith(_p, "==") || startswith(_p, "!=") || startswith(_p, "<=") || startswith(_p, ">=") ){
      _current = new_token(TokenType::TK_SYMBOL, _current, _p, 2);
      _p+=2;
      continue;
    }

    //入力文字列が+か-ならTK_SYMBOL typeのトークンを追加して次の文字へ
    if (strchr("+-*/()=<>", *_p))
    {
      //引数に_p++を渡すことでpを渡しつつpを一つ進めてる。テクい。++pとの違いが出てる。
      _current = new_token(TokenType::TK_SYMBOL, _current, _p++, 1);
      continue;
    }

    if (isdigit(*_p))
    {
      _current = new_token(TokenType::TK_NUMBER, _current, _p, 0);
      //strtolは数値を読み込んだ後、第二引数のポインタをアップデートし読み込んだ最後の文字の次の文字を指すように値を更新する
      char *q = _p;
      _current->value = strtol(_p, &_p, 10);
      _current->length = _p - q;
      continue;
    }

    error("tokenizeできません");
  }
  new_token(TokenType::TK_EOF, _current, _p, 0);
  return head.next;
}
