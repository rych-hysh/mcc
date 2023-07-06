#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdbool>
#include <cstring>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <string>

#include "parser.hpp"
#include "tokenizer.hpp"
#include "generator.hpp"

using namespace std;

char *reading_program;
// head token
Token *head_token;

bool debug_flag = true;

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "invalid number of args");
    return 1;
  }

  reading_program = argv[1];

  Tokenizer *tokenizer = new Tokenizer();
  head_token = tokenizer->tokenize(reading_program);

  if(debug_flag){
    Token *head = head_token;
    ofstream writing_file;
    string filename="tokenize_result.log";
    writing_file.open(filename, ios::out);
    while(head_token){
      writing_file << "'" << head_token->str <<  "', " << endl;
      head_token = head_token->next;
    }
    writing_file.close();
    head_token = head;
  }

  Parser *parser = new Parser();
  //入力をtoken(単語)に分割し、最初のtokenをtoken_proccessingに代入
  Node **statement = parser->parse(head_token);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  //prologue
  //変数26個分の領域を確保する(26 * 8 = 208)
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");


  Generator *generator = new Generator();
  int i = 0;
  while(statement[i]){
    //各文のトップノードを渡す
    generator->gen(statement[i++]);
    printf("  pop rax\n");
  }

  //epilogue
  //最後の式の結果がraxに残っているはずなのでそれが返り値になる

  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");

  return 0;
}