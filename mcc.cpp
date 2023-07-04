#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdbool>
#include <cstring>
#include <cstdarg>
#include <iostream>

using namespace std;

//抽象構文木のノードの種類
enum NodeType{
  ND_ADD,   // +
  ND_SUB,   // -
  ND_MUL,   // *
  ND_DIV,   // /
  ND_NUMBER,// 整数
};

//抽象構文木のノードの型
struct Node {
  NodeType type;
  Node *leftHandSideNode;
  Node *rightHandSideNode;
  int value;
};

Node *mul();
Node *expr();
Node *unary();
Node *primary();
bool consume(char _op);
void expect(char _op);
int expect_number();
void error(const char* _fmt, ...);

Node *new_node(NodeType _type, Node *_lhs, Node *_rhs){
  Node *new_node = (Node*) calloc(1, sizeof(Node));
  new_node->type = _type;
  new_node->leftHandSideNode = _lhs;
  new_node->rightHandSideNode = _rhs;
  return new_node;
}

Node *new_node_num(int _value){
  Node *new_node = (Node*) calloc(1, sizeof(Node));
  new_node->type = NodeType::ND_NUMBER;
  new_node->value = _value;
  return new_node;
}

Node *expr(){
  Node *node = mul();
  while (true)
  {
    if(consume('+')){
      node = new_node(NodeType::ND_ADD, node, mul());
    }else if(consume('-')){
      node = new_node(NodeType::ND_SUB, node, mul());
    }else{
      return node;
    }
  }
}

Node *mul(){
  Node *node = unary();
  while (true)
  {
    if(consume('*')){
      node = new_node(NodeType::ND_MUL, node, unary());
    }else if(consume('/')){
      node = new_node(NodeType::ND_DIV, node, unary());
    }else{
      return node;
    }
  }
}

Node *unary(){
  if(consume('+')){
    return primary();
  }
  if(consume('-')){
    return new_node(NodeType::ND_SUB, new_node_num(0), primary());
  }
  return primary();
}

Node *primary(){
  if(consume('(')){
    Node *node = expr();
    expect(')');
    return node;
  }
  //そうでなければ数値のはず
  return new_node_num(expect_number());
}

void gen(Node *node){
  if(node->type == NodeType::ND_NUMBER){
    printf("  push %d\n", node->value);
    return;
  }

  gen(node->leftHandSideNode);
  gen(node->rightHandSideNode);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->type)
  {
  case NodeType::ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case NodeType::ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case NodeType::ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case NodeType::ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  default:
    error("Invalide NodeType");
    break;
  }
  printf("  push rax\n");
}
//Token(単語)の種別
enum TokenType {
  TK_SYMBOL,  //symbol
  TK_NUMBER,  // integer
  TK_EOF,     //end of file
};
//Token は、単語単位
struct Token {
  TokenType type; //token _type
  Token *next;    //next token
  int value;      //value when _type = TK_INT
  char* str;      //token string
};


char* reading_program;
// token proccessing
Token *token_proccessing;
void error(string _message, ...){
  va_list ap;
  va_start(ap, _message);
  _message.append("\n");
  fprintf(stderr, _message.c_str(), ap);
  exit(1);
}
// TODO: STEP4　改良
void error(const char *_fmt, ...){
  va_list ap;
  va_start(ap, _fmt);

  vfprintf(stderr, _fmt, ap);
  
  fprintf(stderr, "\n");
  exit(1);
}



// maybe _op means operando
// 次のtokenが期待している記号の時にはトークンを１つ読み進めて真を返す。それ以外の場合は偽を返す。
bool consume(char _op){
  if(token_proccessing -> type != TokenType::TK_SYMBOL || token_proccessing -> str[0] != _op) return false;
  token_proccessing = token_proccessing -> next;
  return true;
}

// 次のtokenが期待している記号の時にはトークンを１つ読み進めて真を返す。それ以外の場合はエラーを報告する。
void expect(char _op){
  if(token_proccessing-> type != TokenType::TK_SYMBOL || token_proccessing-> str[0] != _op){
    error(u8"'%c'ではありません", _op);
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
Token *new_token(TokenType _type, Token *_current, char *_str){
  Token *new_token = (Token*) calloc(1, sizeof(Token));
  new_token->type = _type;
  if(_str != "")new_token->str = _str;
  _current-> next = new_token;
  return new_token;
}

///入力文字列pをトークナイズして返す
Token *tokenize(char *_p){
  Token head;
  head.next = NULL;
  Token *_current = &head;

  while(*_p){
    //空白文字をスキップ
    if(isspace(*_p)){
      _p++;
      continue;
    }

    //入力文字列が+か-ならTK_SYMBOL typeのトークンを追加して次の文字へ
    if(*_p == '+' || *_p == '-' || *_p == '*' || *_p == '/' || *_p == '(' || *_p == ')'){
      //引数に_p++を渡すことでpを渡しつつpを一つ進めてる。テクい。++pとの違いが出てる。
      _current = new_token(TokenType::TK_SYMBOL, _current, _p++);
      continue;
    }

    if(isdigit(*_p)){
      _current = new_token(TokenType::TK_NUMBER, _current, _p);
      //strtolは数値を読み込んだ後、第二引数のポインタをアップデートし読み込んだ最後の文字の次の文字を指すように値を更新する
      _current->value = strtol(_p, &_p, 10);
      continue;
    }

    error("tokenizeできません");
  }
  new_token(TokenType::TK_EOF, _current, _p);
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
  Node *node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");


  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");

  return 0;
}