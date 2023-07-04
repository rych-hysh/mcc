#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdbool>
#include <cstring>
#include <cstdarg>
#include <iostream>

using namespace std;

enum TokenType {
  TK_SYMBOL,  //symbol
  TK_NUMBER,       // integer
  TK_EOF,       //end of file
};

//Token は、単語単位
struct Token {
  TokenType type; //token type
  Token *next;    //next token
  int value;      //value when type = TK_INT
  char* str;      //token string
};


char* reading_program;
// token proccessing
Token *token_proccessing;
void error(string message, ...){
  va_list ap;
  va_start(ap, message);
  message.append("\n");
  fprintf(stderr, message.c_str(), ap);
  exit(1);
}
// TODO: STEP4　改良
void error(const char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);

  vfprintf(stderr, fmt, ap);
  
  fprintf(stderr, "\n");
  exit(1);
}



// maybe op means operando
// 次のtokenが期待している記号の時にはトークンを１つ読み進めて真を返す。それ以外の場合は偽を返す。
bool consume(char op){
  if(token_proccessing -> type != TokenType::TK_SYMBOL || token_proccessing -> str[0] != op) return false;
  token_proccessing = token_proccessing -> next;
  return true;
}

// 次のtokenが期待している記号の時にはトークンを１つ読み進めて真を返す。それ以外の場合はエラーを報告する。
void expect(char op){
  if(token_proccessing-> type != TokenType::TK_SYMBOL || token_proccessing-> str[0] != op){
    error(u8"'%c'ではありません", op);
  }
  token_proccessing= token_proccessing-> next;
}

int expect_number(){
  if(token_proccessing->type != TokenType::TK_NUMBER){
    error(u8"数ではありません");
  }
  int value = token_proccessing-> value;
  token_proccessing= token_proccessing->next;
  return value;
}

bool at_eof(){
  return token_proccessing->type == TokenType::TK_EOF;
}

//新しいトークンを作成してcurrent tokenに繋げる
Token *new_token(TokenType type, Token *current, char *str){
  Token *new_token = (Token*) calloc(1, sizeof(Token));
  new_token->type = type;
  if(str != "")new_token->str = str;
  current-> next = new_token;
  return new_token;
}

///入力文字列pをトークナイズして返す
Token *tokenize(char *p){
  Token head;
  head.next = NULL;
  Token *current = &head;

  while(*p){
    //空白文字をスキップ
    if(isspace(*p)){
      p++;
      continue;
    }

    //入力文字列が+か-ならTK_SYMBOL typeのトークンを追加して次の文字へ
    if(*p == '+' || *p == '-'){
      //引数にp++を渡すことでpを渡しつつpを一つ進めてる。テクい。++pとの違いが出てる。
      current = new_token(TokenType::TK_SYMBOL, current, p++);
      continue;
    }

    if(isdigit(*p)){
      current = new_token(TokenType::TK_NUMBER, current, p);
      //strtolは数値を読み込んだ後、第二引数のポインタをアップデートし読み込んだ最後の文字の次の文字を指すように値を更新する
      current->value = strtol(p, &p, 10);
      continue;
    }

    error("tokenizeできません");
  }
  new_token(TokenType::TK_EOF, current, p);
  return head.next;
}

int main(int argc, char **argv){
  if(argc != 2){
    fprintf(stderr, "invalid number of args");
    return 1;
  }
  reading_program = argv[1];
  //入力をtoken(単語)に分割し、最初のtokenをtoken_proccessingに代入
  token_proccessing = tokenize(reading_program);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");


  //式の最初は数でなければならないので、それをチェックして最初のmov命令を出力
  printf("  mov rax, %d\n", expect_number());

  while(!at_eof()){
    if(consume('+')){
      printf("  add rax, %d\n", expect_number());
      continue;
    }
    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");

  return 0;
}